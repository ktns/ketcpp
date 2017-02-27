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
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

#include "wrapper/matrix/matrix.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T> class MatrixVector : public MatrixBase<T> {
      private:
        const size_t num_rows;
        const size_t num_columns;
        const size_t &row_size = num_columns;
        const size_t &column_size = num_rows;

      public:
        size_t get_num_rows() const override { return num_rows; }
        size_t get_num_columns() const override { return num_columns; }
        size_t get_row_size() const override { return row_size; }
        size_t get_column_size() const override { return column_size; }

      private:
        using vector = std::vector<T>;
        vector storage;
        using Base = MatrixBase<T>;

      private:
        static size_t
        max_size(const std::initializer_list<std::initializer_list<T>> &list) {
          const auto max_size_element = std::max_element(
              list.begin(), list.end(), [](const std::initializer_list<T> &a,
                                           const std::initializer_list<T> &b) {
                return a.size() < b.size();
              });
          return max_size_element->size();
        }

      public:
        MatrixVector(const std::initializer_list<std::initializer_list<T>> list)
            : num_rows(list.size()), num_columns(max_size(list)),
              storage(num_rows * num_columns) {
          auto dest = storage.begin();
          for (auto i : list) {
            std::copy(i.begin(), i.end(), dest);
            dest += num_columns;
          }
        }
        MatrixVector(size_t m) : num_rows(m), num_columns(m), storage(m * m) {}
        MatrixVector(size_t m, size_t n)
            : num_rows(m), num_columns(n), storage(m * n) {}
        MatrixVector() = delete;

        T &at(size_t i, size_t j) override {
          return storage[i * num_columns + j];
        }

        T at(size_t i, size_t j) const override {
          return storage[i * num_columns + j];
        }

        bool operator==(const MatrixVector &rhs) const {
          return std::equal(this->storage.cbegin(), this->storage.cend(),
                            rhs.storage.cbegin());
        }

        Base &operator+=(const Base &rhsbase) override {
          auto *prhs = dynamic_cast<const MatrixVector *>(&rhsbase);
          if (prhs == nullptr)
            return Base::operator+=(rhsbase);
          std::transform(this->storage.cbegin(), this->storage.cend(),
                         prhs->storage.cbegin(), this->storage.begin(),
                         [](T l, T r) -> T { return l + r; });
          return *this;
        }

        Base &operator*=(T rhs) override {
          std::transform(this->storage.cbegin(), this->storage.cend(),
                         this->storage.begin(),
                         [rhs](T l) -> T { return l * rhs; });
          return *this;
        }

        std::unique_ptr<MatrixBase<T>> copy() const override {
          std::unique_ptr<MatrixBase<T>> copy;
          copy.reset(new MatrixVector(*this));
          return std::move(copy);
        }

        ~MatrixVector() {}
      };
    }
  }
}
