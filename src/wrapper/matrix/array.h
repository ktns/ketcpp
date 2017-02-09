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

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>

#include "wrapper/matrix/matrix.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T, size_t m, size_t n = m>
      class MatrixArray : public MatrixBase<T> {
      public:
        constexpr static size_t num_rows = m;
        constexpr static size_t num_columns = n;
        constexpr static size_t row_size = n;
        constexpr static size_t column_size = m;
        size_t get_num_rows() const { return num_rows; }
        size_t get_num_columns() const { return num_columns; }
        size_t get_row_size() const { return row_size; }
        size_t get_column_size() const { return column_size; }

      private:
        using array = std::array<T, m * n>;
        array storage;
        using Base = MatrixBase<T>;

      public:
        T &at(size_t i, size_t j) { return storage[i * num_columns + j]; }

        T at(size_t i, size_t j) const { return storage[i * num_columns + j]; }

        MatrixArray(const std::initializer_list<std::initializer_list<T>> &list)
            : storage() {
          auto dest = storage.begin();
          for (auto i : list) {
            dest = std::copy(i.begin(), i.end(), dest);
          }
        }
        MatrixArray(const std::initializer_list<T> &list) : storage() {
          std::copy(list.begin(), list.end(), storage.begin());
        }
        MatrixArray() = default;

        bool operator==(const MatrixArray &rhs) const {
          return std::equal(this->storage.cbegin(), this->storage.cend(),
                            rhs.storage.cbegin());
        }
        Base &operator+=(const Base &rhsbase) {
          try {
            auto &rhs = dynamic_cast<const MatrixArray &>(rhsbase);
            std::transform(this->storage.cbegin(), this->storage.cend(),
                           rhs.storage.cbegin(), this->storage.begin(),
                           [](T l, T r) -> T { return l + r; });
            return *this;
          } catch (std::bad_cast &ex) {
            return Base::operator+=(rhsbase);
          }
        }

        Base &operator*=(T rhs) {
          std::transform(this->storage.cbegin(), this->storage.cend(),
                         this->storage.begin(),
                         [rhs](T l) -> T { return l * rhs; });
          return *this;
        }

        std::unique_ptr<MatrixBase<T>> copy() const {
          std::unique_ptr<MatrixBase<T>> copy;
          copy.reset(new MatrixArray(*this));
          return std::move(copy);
        }

        ~MatrixArray() {}
      };
    }
  }
}
