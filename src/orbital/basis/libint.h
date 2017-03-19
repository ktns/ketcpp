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

#include "wrapper/matrix.h"
#include "wrapper/molecule/base.h"

namespace ketcpp {
  namespace orbital {
    namespace basis {
      using pointcharge_t = ketcpp::wrapper::molecule::pointcharge_t;
      using atom_t = ketcpp::wrapper::molecule::atom_t;

      //! Represents the basis of LCAO gaussian orbitals for a molecule and
      //! computes integrals via libint2.
      class Libint2Basis {
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
        //! @return Overlap matrix of basis functions.
        matrix_t get_overlap();
        //! @return Kinetic energy matrix of basis functions.
        matrix_t get_kinetic();
        //! @return Nuclear attraction energy matrix of basis functions.
        //! @details Uses atoms in the molecule passed by initializer.
        matrix_t get_nuclear();
        //! @return Nuclear attraction energy matrix of basis functions.
        //! @details Uses atoms or point-charges in the passed vector.
        //! @note Atoms specified during initialization will be ignored.
        matrix_t get_nuclear(const std::vector<pointcharge_t> &charges);
        //! @return Nuclear attraction energy matrix of basis functions.
        //! @tparam Iter Iterator over point-charges.
        //! @note Atoms specified during initialization will be ignored.
        template <typename Iter>
        std::enable_if_t<std::is_convertible_v<
                             typename std::iterator_traits<Iter>::value_type,
                             const pointcharge_t &>,
                         matrix_t>
        get_nuclear(Iter begin, Iter end) {
          std::vector<pointcharge_t> charges;
          if (std::is_base_of_v<
                  std::forward_iterator_tag,
                  typename std::iterator_traits<Iter>::iterator_category>)
            charges.reserve(std::distance(begin, end));
          std::copy(begin, end, std::back_inserter(charges));
          this->get_nuclear(charges);
        }
        //! Compute the electron repulsion energy part of the Fock matrix for
        //! the specified electron population.
        //! @param[in,out] fock The matrix to add up electron repulsion matrix
        //! @param[in] density The population matrix of electrons
        //! @return A reference to @p fock
        matrix_t &add_rhf_electron_repulsion(matrix_t &fock,
                                             const matrix_t &density);
        template <typename... Args>
        //! Compute the Fock matrix for the specified electron population.
        //! @param[in] density The population matrix of electrons
        //! @param[in] args Parameters to be passed to \p get_nuclear(args)
        //! @return Fock matrix
        matrix_t get_rhf_fock(const matrix_t &density, Args... args) {
          auto fock = get_kinetic() + get_nuclear(args...);
          return add_rhf_electron_repulsion(fock, density);
        }
      };
    }
  }
}

#endif
