/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2017 Katsuhiko Nishimra
 *
 * This file is part of ketcpp.
 *
 * ketcpp is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or any later version.
 *
 * ketcpp is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ketcpp.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cassert>

#include "jobs/rhf.h"

#include "wrapper/matrix/eigensolver/generalized_eigensolver_hermite.h"

using namespace ketcpp;
using namespace ketcpp::jobs;

void RHF::prepare(std::unique_ptr<const molecule_t> &&mol,
                  const basisset_t &set) {
  initial_guess_type = {};
  molecule = std::move(mol);
  basis = set.get_basis(molecule);
  overlap = std::make_unique<const wrapper::matrix::Matrix<double>>(
      basis->get_overlap());
  core_hamiltonian = std::make_unique<wrapper::matrix::Matrix<double>>(
      basis->get_kinetic() + basis->get_nuclear(*molecule));
  assert(num_electrons() % 2 == 0);
  logger.initialize_scf(config);
  prepared = true;
}

void RHF::release(std::unique_ptr<wrapper::molecule::Base> &mol) {
  prepared = false;
  mol.reset(const_cast<wrapper::molecule::Base *>(molecule.release()));
}

double RHF::calc_energy() const {
  assert(prepared);
  assert(fock);
  assert(density);
  const auto &Hc = *core_hamiltonian, &F = *fock, &P = *density, FHc = F + Hc,
             PFHc = P * FHc;
  return PFHc->trace() + molecule->nuclear_repulsion_energy();
}

InitialGuessType RHF::make_initial_guess(InitialGuessMethod method) {
  assert(prepared);
  switch (method) {
  case InitialGuessMethod::CoreHamiltonian:
    fock.reset(new matrix_t(*core_hamiltonian));
    initial_guess_type = InitialGuessType::Fock;
    break;
  default:
    throw std::logic_error("Unknown initial guess method");
  }
  assert(initial_guess_type.has_value());
  return initial_guess_type.value();
}

void RHF::update_orbital() {
  typedef matrix_t::value_type T;
  auto make_unit_matrix = wrapper::matrix::make_unit_matrix<T>;
  assert(overlap);
  assert(fock);
  const auto &S = *overlap, &F = *fock;
  wrapper::matrix::eigensolver::GeneralizedEigensolverHermite<T> geh(F, S);
  auto[E, C] = geh.solve();

  const auto I[[maybe_unused]] = make_unit_matrix(S->get_num_rows()),
             error[[maybe_unused]] = C->transpose() * S * C - I;
  assert(error->max_absolute() < 1e-5);

  energies.reset(new matrix_t(std::move(E)));
  coefficients.reset(new matrix_t(std::move(C)));
}

// TODO: Degeneracy handling
void RHF::update_density() {
  typedef matrix_t::value_type T;
  assert(energies);
  assert(coefficients);
  const auto &E = *energies, &C = *coefficients;
  std::vector<T> v(E->size());
  std::copy(E->cbegin(), E->cend(), v.begin());
  std::sort(v.begin(), v.end());
  const T thresh = v.at(num_electrons() / 2);
  std::transform(E.cbegin(), E.cend(), v.begin(),
                 [thresh](T e) -> T { return e < thresh ? 1 : 0; });
  const auto n = wrapper::matrix::make_diagonal_matrix(v.cbegin(), v.cend());
  assert(std::abs(n->trace() - num_electrons() / 2) < 1e-5);
  density.reset(new matrix_t(C * n * C->transpose()));
}

void RHF::update_fock() {
  assert(density);
  fock.reset(new matrix_t(basis->get_rhf_fock(*density, *molecule)));
}

void RHF::solve() {
  assert(prepared);
  auto initial_guess_type = make_initial_guess(config.initial_guess_method);
  assert(this->initial_guess_type.has_value());
  switch (initial_guess_type) {
  case InitialGuessType::Fock:
    update_orbital();
    update_density();
    update_fock();
    update_orbital();
    std::swap(density, previous_density);
    update_density();
    break;
  case InitialGuessType::Density:
    update_fock();
    std::swap(density, previous_density);
    update_density();
    break;
  default:
    throw std::logic_error("Unknown type of initial guess");
  }
  for (size_t i = 0; i < config.max_iterations && !has_converged(); i++) {
    update_fock();
    update_orbital();
    std::swap(density, previous_density);
    update_density();
  }
}

bool RHF::has_converged() const {
  const auto &Pn = *density, &Pn_1 = *previous_density, dP = Pn - Pn_1;
  return dP->frobenius_norm() < config.convergence_criterion;
}
