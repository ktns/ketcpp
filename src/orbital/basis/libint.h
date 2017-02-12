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

#include <memory>

#include "wrapper/matrix/matrix.h"

namespace ketcpp {
  namespace orbital {
    namespace basis {
      class Libint2Basis {
      private:
        class Impl;
        std::unique_ptr<Impl> impl;

      public:
        Libint2Basis(const std::string &xyz_file,
                     const std::string &basisset_name);
        ~Libint2Basis();

        typedef wrapper::matrix::Matrix<double> matrix_t;
        matrix_t get_overlap();
        matrix_t get_kinetic();
      };
    }
  }
}

#endif
