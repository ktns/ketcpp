/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2017 Katsuhiko Nishimra
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

#include "wrapper/matrix/matrix.h"

namespace ketcpp::wrapper::matrix {
  //! Matrix instance that is a transpose matrix of another matrix instance.
  template <typename T> class MatrixTransposed : public MatrixBase<T> {
  private:
    using Base = MatrixBase<T>;
    using unique_ptr = std::unique_ptr<Base>;
    friend Base;
    unique_ptr base;

    MatrixTransposed(unique_ptr &&ptr) : base(std::move(ptr)) {}

    size_t get_num_rows() const override { return base->get_num_columns(); }
    size_t get_num_columns() const override { return base->get_num_rows(); }
    size_t get_row_size() const override { return base->get_column_size(); }
    size_t get_column_size() const override { return base->get_row_size(); }

    virtual T &at(size_t irow, size_t icol) override {
      return base->at(icol, irow);
    }
    virtual T at(size_t irow, size_t icol) const override {
      return base->at(icol, irow);
    }

    virtual unique_ptr copy() const override {
      return std::unique_ptr<MatrixBase<T>>(new MatrixTransposed(base->copy()));
    }
  };

  template <typename T> Matrix<T> MatrixBase<T>::transpose() const {
    return MatrixTransposed<T>(this->copy());
  }
}
