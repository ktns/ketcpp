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

#include <iostream>
#include <memory>

#include "wrapper/matrix/base.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T> class Matrix {
      private:
        using Base = MatrixBase<T>;
        using unique_ptr = std::unique_ptr<Base>;
        unique_ptr base;

      public:
        Matrix(const Matrix &src) : base(std::move(src.base->copy())) {}
        Matrix(const Base &src) : base(std::move(src.copy())) {}
        Matrix(Matrix &&src) : base(std::move(src.base)) {}
        Matrix(std::unique_ptr<Base> &&src) : base(std::move(src)) {}

        size_t get_num_rows() const { return base->get_num_rows(); }
        size_t get_num_columns() const { return base->get_num_columns(); }
        size_t get_row_size() const { return base->get_row_size(); }
        size_t get_column_size() const { return base->get_column_size(); }

        auto rows() { return base->rows(); }
        auto rows() const { return base->rows(); }
        auto columns() { return base->columns(); }
        auto columns() const { return base->columns(); }

        bool operator==(const Matrix &rhs) const { return *base == *rhs.base; }

        Matrix operator+(const Matrix &rhs) const {
          return *this; // FIXME
        }

        Matrix &operator+=(const Matrix &rhs) {
          *this->base += rhs.base;
          return *this;
        }

        Matrix operator*(T rhs) {
          return std::move(Matrix(std::move(*this->base * rhs)));
        }

        Matrix operator*=(T rhs) {
          *this->base *= rhs;
          return *this;
        }

        Matrix operator*(const Matrix &rhs) const {
          return std::move(Matrix(std::move(*this->base * (*rhs.base))));
        }

        friend std::ostream &operator<<(std::ostream &out,
                                        const Matrix &matrix) {
          return out << *matrix.base;
        }
      };
    }
  }
}
