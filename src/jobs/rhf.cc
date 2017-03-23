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
  prepared = true;
}

void RHF::release(std::unique_ptr<wrapper::molecule::Base> &mol) {
  prepared = false;
  mol.reset(const_cast<wrapper::molecule::Base *>(molecule.release()));
}

InitialGuessType RHF::make_initial_guess(InitialGuessMethod method) {
  assert(prepared);
  initial_guess_type = [this, method] {
    switch (method) {
    case InitialGuessMethod::CoreHamiltonian:
      fock.reset(new matrix_t(*core_hamiltonian));
      return InitialGuessType::Fock;
    default:
      throw std::logic_error("Unknown initial guess method");
    }
  }();
  assert(initial_guess_type.has_value());
  return initial_guess_type.value();
}

void RHF::update_orbital() {
  assert(overlap);
  assert(fock);
  const auto &S = *overlap, &F = *fock;
  wrapper::matrix::eigensolver::GeneralizedEigensolverHermite geh(S, F);
  auto[E, C] = geh.solve();
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
                 [thresh](T e) -> T { return e <= thresh ? 2 : 0; });
  const auto n = wrapper::matrix::make_diagonal_matrix(v.cbegin(), v.cend());
  density.reset(new matrix_t(C->transpose() * n * C));
}

void RHF::update_fock() {
  assert(density);
  fock.reset(new matrix_t(basis->get_rhf_fock(*density, *molecule)));
}
