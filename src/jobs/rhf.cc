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

#include "jobs/rhf.h"

using namespace ketcpp;
using namespace ketcpp::jobs;

void RHF::prepare(std::unique_ptr<const molecule_t> &&mol,
                  std::unique_ptr<const basisset_t> &&set) {
  molecule = std::move(mol);
  basisset = std::move(set);
  basis = basisset->get_basis(*molecule.get());
  overlap = std::make_unique<const wrapper::matrix::Matrix<double>>(
      std::move(basis->get_overlap()));
  core_hamiltonian = std::make_unique<wrapper::matrix::Matrix<double>>(
      std::move(basis->get_kinetic()));
  prepared = true;
}

void RHF::release(std::unique_ptr<wrapper::molecule::Base> &mol,
                  std::unique_ptr<orbital::basisset::Base> &set) {
  prepared = false;
  mol.reset(const_cast<wrapper::molecule::Base *>(molecule.release()));
  set.reset(const_cast<orbital::basisset::Base *>(basisset.release()));
}
