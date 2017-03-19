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

#include "wrapper/matrix.h"

namespace ketcpp::wrapper::matrix {
  //! @brief Matrix instance using std::array as its storage.
  //! @note No specializations are made for testing purpose.
  template <typename T, size_t m, size_t n = m>
  class MatrixArrayCore : public MatrixBase<T> {
  protected:
    //! @cond PRIVATE
    using array = std::array<T, m * n>;
    array storage;
    using Base = MatrixBase<T>;
    //! @endcond

  public:
    //! Number of rows in matrices
    constexpr static size_t num_rows = m;
    //! Number of rows in matrices
    constexpr static size_t num_columns = n;
    constexpr static size_t row_size = n;
    constexpr static size_t column_size = m;
    size_t get_num_rows() const override { return num_rows; }
    size_t get_num_columns() const override { return num_columns; }
    size_t get_row_size() const override { return row_size; }
    size_t get_column_size() const override { return column_size; }

    T &at(size_t i, size_t j) override { return storage[i * num_columns + j]; }
    T at(size_t i, size_t j) const override {
      return storage[i * num_columns + j];
    }

    //! Construction by nested initalizer list
    MatrixArrayCore(const std::initializer_list<std::initializer_list<T>> &list)
        : storage() {
      auto dest = storage.begin();
      for (auto i : list) {
        auto end = std::min(i.begin() + this->size(), i.end());
        dest = std::copy(i.begin(), end, dest);
      }
    }
    //! Construction by flat initializer list
    MatrixArrayCore(const std::initializer_list<T> &list) : storage() {
      auto end = std::min(list.begin() + this->size(), list.end());
      std::copy(list.begin(), end, storage.begin());
    }
    //! Default constructor
    MatrixArrayCore() = default;
    //! Copy constructor
    MatrixArrayCore(const MatrixArrayCore &other) = default;
    //! Assignment operator
    MatrixArrayCore &operator=(const MatrixArrayCore &other) {
      storage = other.storage;
      return *this;
    }

    virtual std::unique_ptr<MatrixBase<T>> copy() const override {
      std::unique_ptr<MatrixBase<T>> copy;
      copy.reset(new MatrixArrayCore(*this));
      return std::move(copy);
    }

    virtual ~MatrixArrayCore() override {}
  };

  //! Matrix instance using std::array as its storage.
  template <typename T, size_t m, size_t n = m>
  class MatrixArray : public MatrixArrayCore<T, m, n> {
  private:
    using Base = typename MatrixArrayCore<T, m, n>::Base;

  public:
    using MatrixArrayCore<T, m, n>::MatrixArrayCore;

    //! Inequality operator
    bool operator!=(const Base &rhs) const override {
      auto *prhs = dynamic_cast<const MatrixArray *>(&rhs);
      if (prhs == nullptr)
        return Base::operator!=(rhs);
      else
        return !std::equal(this->storage.cbegin(), this->storage.cend(),
                           prhs->storage.cbegin());
    }

    //! Addition assignment operator
    Base &operator+=(const Base &rhsbase) override {
      auto *prhs = dynamic_cast<const MatrixArray *>(&rhsbase);
      if (prhs == nullptr)
        return Base::operator+=(rhsbase);
      std::transform(this->storage.cbegin(), this->storage.cend(),
                     prhs->storage.cbegin(), this->storage.begin(),
                     [](T l, T r) -> T { return l + r; });
      return *this;
    }

    //! Subtraction assignment operator
    Base &operator-=(const Base &rhsbase) override {
      auto *prhs = dynamic_cast<const MatrixArray *>(&rhsbase);
      if (prhs == nullptr)
        return Base::operator-=(rhsbase);
      std::transform(this->storage.cbegin(), this->storage.cend(),
                     prhs->storage.cbegin(), this->storage.begin(),
                     [](T l, T r) -> T { return l - r; });
      return *this;
    }

    //! @brief Multiplication assignment operator with a scalar
    //! @post All elements of the matrix are scaled by a scalar
    Base &operator*=(T rhs) override {
      std::transform(this->storage.cbegin(), this->storage.cend(),
                     this->storage.begin(),
                     [rhs](T l) -> T { return l * rhs; });
      return *this;
    }

    Base &operator/=(T rhs) override {
      std::transform(this->storage.cbegin(), this->storage.cend(),
                     this->storage.begin(),
                     [rhs](T l) -> T { return l / rhs; });
      return *this;
    }

    std::unique_ptr<MatrixBase<T>> copy() const override {
      std::unique_ptr<MatrixBase<T>> copy;
      copy.reset(new MatrixArray(*this));
      return std::move(copy);
    }
  };
}
