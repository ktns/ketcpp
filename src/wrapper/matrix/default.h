/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2015 Katsuhiko Nishimra
 *
 * This file is part of ketcpp.
 *
 * ketcpp is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3.0 of the License, or any later version.
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

#include "wrapper/matrix/array.h"
#include "wrapper/matrix/vector.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T, size_t m, size_t n = m>
      Matrix<T>
      make_matrix(std::initializer_list<std::initializer_list<T>> list) {
        std::unique_ptr<MatrixBase<T>> ptr;
        ptr.reset(new MatrixArray<T, m, n>(list));
        return std::move(Matrix<T>(std::move(ptr)));
      }

      template <typename T, size_t m, size_t n = m>
      Matrix<T> make_matrix(std::initializer_list<T> list) {
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

      template <typename T> Matrix<T> make_zero_matrix(size_t m, size_t n = 0) {
        if (n == 0)
          n = m;
        assert(n > 0 && m > 0);
        auto ptr = std::make_unique<MatrixVector<T>>(n, m);
        ptr->for_each(
            [&ptr](size_t i, size_t j) { assert(ptr->at(i, j) == 0); });
        return Matrix<T>(std::move(ptr));
      }

      template <typename T>
      Matrix<T> make_symmetric_matrix(
          std::initializer_list<std::initializer_list<T>> list) {
        size_t i = 0;
        auto ptr = std::make_unique<MatrixVector<T>>(list.size());
        for (auto l : list) {
          assert(l.size() == i + 1);
          size_t j = 0;
          for (T v : l) {
            ptr->at(i, j) = ptr->at(j, i) = v;
            j++;
          }
          i++;
        }
        return Matrix<T>(std::move(ptr));
      }
    }
  }
}
