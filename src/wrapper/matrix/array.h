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
      class MatrixArrayCore : public MatrixBase<T> {
      protected:
        using array = std::array<T, m * n>;
        array storage;
        using Base = MatrixBase<T>;

      public:
        constexpr static size_t num_rows = m;
        constexpr static size_t num_columns = n;
        constexpr static size_t row_size = n;
        constexpr static size_t column_size = m;
        size_t get_num_rows() const override { return num_rows; }
        size_t get_num_columns() const override { return num_columns; }
        size_t get_row_size() const override { return row_size; }
        size_t get_column_size() const override { return column_size; }

        T &at(size_t i, size_t j) override {
          return storage[i * num_columns + j];
        }
        T at(size_t i, size_t j) const override {
          return storage[i * num_columns + j];
        }

        MatrixArrayCore(
            const std::initializer_list<std::initializer_list<T>> &list)
            : storage() {
          auto dest = storage.begin();
          for (auto i : list) {
            dest = std::copy(i.begin(), i.end(), dest);
          }
        }
        MatrixArrayCore(const std::initializer_list<T> &list) : storage() {
          std::copy(list.begin(), list.end(), storage.begin());
        }
        MatrixArrayCore() = default;

        virtual std::unique_ptr<MatrixBase<T>> copy() const override {
          std::unique_ptr<MatrixBase<T>> copy;
          copy.reset(new MatrixArrayCore(*this));
          return std::move(copy);
        }

        virtual ~MatrixArrayCore() override {}
      };

      template <typename T, size_t m, size_t n = m>
      class MatrixArray : public MatrixArrayCore<T, m, n> {
      private:
        using Base = typename MatrixArrayCore<T, m, n>::Base;

      public:
        using MatrixArrayCore<T, m, n>::MatrixArrayCore;

        bool operator==(const MatrixArray &rhs) const {
          return std::equal(this->storage.cbegin(), this->storage.cend(),
                            rhs.storage.cbegin());
        }
        virtual Base &operator+=(const Base &rhsbase) override {
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

        virtual Base &operator*=(T rhs) override {
          std::transform(this->storage.cbegin(), this->storage.cend(),
                         this->storage.begin(),
                         [rhs](T l) -> T { return l * rhs; });
          return *this;
        }

        virtual std::unique_ptr<MatrixBase<T>> copy() const override {
          std::unique_ptr<MatrixBase<T>> copy;
          copy.reset(new MatrixArray(*this));
          return std::move(copy);
        }

        virtual ~MatrixArray() override {}
      };
    }
  }
}
