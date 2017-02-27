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

#include "wrapper/matrix/matrix.h"

namespace ketcpp::wrapper::matrix {
  template <typename T> class MatrixDummy : public MatrixBase<T> {
  public:
    MatrixDummy() {}

    size_t get_num_rows() const override {
      throw std::logic_error("Dummy matrix is in use!");
    }
    size_t get_num_columns() const override {
      throw std::logic_error("Dummy matrix is in use!");
    };
    size_t get_row_size() const override {
      throw std::logic_error("Dummy matrix is in use!");
    };
    size_t get_column_size() const override {
      throw std::logic_error("Dummy matrix is in use!");
    };
    T &at(size_t, size_t) override {
      throw std::logic_error("Dummy matrix is in use!");
    };
    T at(size_t, size_t) const override {
      throw std::logic_error("Dummy matrix is in use!");
    };
    std::unique_ptr<MatrixBase<T>> copy() const override {
      return std::make_unique<MatrixDummy>();
    };
  };

  template <typename T> Matrix<T> make_dummy_matrix() {
    return Matrix<T>(std::make_unique<MatrixDummy<T>>());
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &os, const MatrixDummy<T> &dummy) {
    return os << "{Dummy Matrix}";
  }
}
