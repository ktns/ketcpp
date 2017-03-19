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

#include <memory>

#include "wrapper/matrix.h"
#include "wrapper/molecule/base.h"

namespace ketcpp::orbital::basis {
  using matrix_t = wrapper::matrix::Matrix<double>;
  using pointcharge_t = ketcpp::wrapper::molecule::pointcharge_t;
  using atom_t = ketcpp::wrapper::molecule::atom_t;

  //! Base abstract class for the basis of LCAO orbitals for a molecule
  class Base {
  public:
    virtual ~Base() {}
    //! @return Overlap matrix of basis functions.
    virtual matrix_t get_overlap() = 0;
    //! @return Kinetic energy matrix of basis functions.
    virtual matrix_t get_kinetic() = 0;
    //! @return Nuclear attraction energy matrix of basis functions.
    //! @details Uses atoms or point-charges in the passed vector.
    virtual matrix_t get_nuclear(const std::vector<pointcharge_t> &charges) = 0;
    //! @return Nuclear attraction energy matrix of basis functions.
    //! @tparam Iter Iterator over point-charges.
    template <typename Iter>
    std::enable_if_t<
        std::is_convertible_v<typename std::iterator_traits<Iter>::value_type,
                              const pointcharge_t &>,
        matrix_t>
    get_nuclear(const Iter &begin, const Iter &end) {
      std::vector<pointcharge_t> charges;
      if (std::is_base_of_v<
              std::forward_iterator_tag,
              typename std::iterator_traits<Iter>::iterator_category>)
        charges.reserve(std::distance(begin, end));
      std::copy(begin, end, std::back_inserter(charges));
      return this->get_nuclear(charges);
    }
    //! @return Nuclear attraction energy matrix of basis functions.
    //! @param mol An instance representing a molecule.
    matrix_t get_nuclear(const wrapper::molecule::Base &mol) {
      return get_nuclear(mol.atoms().cbegin(), mol.atoms().cend());
    }
    //! Compute the electron repulsion energy part of the Fock matrix for
    //! the specified electron population.
    //! @param[in,out] fock The matrix to add up electron repulsion matrix
    //! @param[in] density The population matrix of electrons
    //! @return A reference to @p fock
    virtual matrix_t &add_rhf_electron_repulsion(matrix_t &fock,
                                                 const matrix_t &density) = 0;
    //! Compute the Fock matrix for the specified electron population.
    //! @param[in] density The population matrix of electrons
    //! @param[in] args Parameters to be passed to \p get_nuclear(args)
    //! @return Fock matrix
    template <typename... Args>
    matrix_t get_rhf_fock(const matrix_t &density, const Args &... args) {
      auto fock = get_kinetic() + get_nuclear(args...);
      return add_rhf_electron_repulsion(fock, density);
    }
  };
}
