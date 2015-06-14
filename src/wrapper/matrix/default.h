/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2015 Katsuhiko Nishimra
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
#include <utility>

#include "wrapper/matrix/matrix.h"
#include "wrapper/matrix/array.h"
#include "wrapper/matrix/vector.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T, int m, int n = m>
      Matrix<T>
      make_matrix(std::initializer_list<std::initializer_list<T>> list) {
        std::unique_ptr<MatrixBase<T>> ptr;
        ptr.reset(new MatrixArray<T, m, n>(list));
        return std::move(Matrix<T>(std::move(ptr)));
      }

      template <typename T>
      Matrix<T>
      make_matrix(std::initializer_list<std::initializer_list<T>> list) {
        std::unique_ptr<MatrixBase<T>> ptr;
        ptr.reset(new MatrixVector<T>(list));
        return std::move(Matrix<T>(std::move(ptr)));
      }
    }
  }
}
