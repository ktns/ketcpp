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

#pragma once

#include "config/ketcpp_config.h"

#ifdef LIBINT2_FOUND

#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#include "orbital/basis/base.h"

namespace ketcpp::orbital::basis {

  //! Represents the basis of LCAO gaussian orbitals for a molecule and
  //! computes integrals via libint2.
  class Libint2Basis : public Base {
  private:
    //! @cond PRIVATE
    class Impl;
    std::unique_ptr<Impl> impl;
    //! @endcond

  public:
    //// for test
    // Libint2Basis(const std::string &xyz_file,
    //             const std::string &basisset_name);

    //! @brief Initialize a basis object for a molecule
    //! @param[in] mol A molecule
    //! @param[in] basisset_name Specify the basisset to be used by the name
    //! @todo Implement a parser for basisset definition files
    Libint2Basis(const wrapper::molecule::Base &mol,
                 const std::string &basisset_name);

    ~Libint2Basis();

    typedef wrapper::matrix::Matrix<double> matrix_t;
    matrix_t get_overlap() const override;
    matrix_t get_kinetic() const override;
    matrix_t
    get_nuclear(const std::vector<pointcharge_t> &charges) const override;
    using Base::get_nuclear;
    matrix_t &
    add_rhf_electron_repulsion(matrix_t &fock,
                               const matrix_t &density) const override;
  };
}

#endif
