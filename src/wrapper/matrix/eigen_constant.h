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

#if __has_include("Eigen/Dense")

#include <cassert>
#include <type_traits>

#include "wrapper/matrix/eigen_common.h"

namespace ketcpp::wrapper::matrix {
  template <typename T, size_t Rows = 0, size_t Cols = Rows>
  class MatrixEigen : public MatrixEigenCommon<T> {
    // This difinition is for constant sized matrix.
    static_assert(Rows > 0 && Cols > 0);

  private:
    typedef Eigen::Matrix<T, Rows, Cols, Eigen::StorageOptions::RowMajor>
        matrix_t;
    matrix_t matrix;
    using Common = MatrixEigenCommon<T>;
    using common_matrix_ref = typename Common::common_matrix_ref;
    using const_common_matrix_ref = typename Common::const_common_matrix_ref;
    common_matrix_ref matrix_instance() override { return matrix; }
    const_common_matrix_ref matrix_instance() const override { return matrix; }

  public:
    constexpr static size_t num_rows = Rows;
    constexpr static size_t num_columns = Cols;
    constexpr static size_t row_size = Cols;
    constexpr static size_t column_size = Rows;
    size_t get_num_rows() const override { return num_rows; }
    size_t get_num_columns() const override { return num_columns; }
    size_t get_row_size() const override { return row_size; }
    size_t get_column_size() const override { return column_size; }

    T &at(size_t i, size_t j) override { return matrix(i, j); }
    T at(size_t i, size_t j) const override { return matrix(i, j); }

  public:
    MatrixEigen() : matrix(matrix_t::Zero()){};
    MatrixEigen(const std::initializer_list<T> &list) {
      if (list.size() < this->size()) {
        throw std::logic_error("Too short initialization list!");
      }
      matrix = Eigen::Map<const matrix_t>(list.begin());
    }
    MatrixEigen(const std::initializer_list<std::initializer_list<T>> &list) {
      size_t i = 0;
      for (auto l : list) {
        if (l.size() < this->get_row_size()) {
          throw std::logic_error("Too short initialization list!");
        }
        typedef Eigen::Matrix<T, 1, num_columns> row_t;
        matrix.row(i++) = Eigen::Map<const row_t>(l.begin(), 1, l.size());
      }
    }

    MatrixEigen(const Common &from) : matrix(from.matrix_instance()) {
      assert(row_size == from.get_row_size() &&
             column_size == from.get_column_size());
    };

    std::unique_ptr<MatrixBase<T>> copy() const override {
      std::unique_ptr<MatrixBase<T>> copy;
      copy.reset(new MatrixEigen(*this));
      return copy;
    }
  };
}
#endif
