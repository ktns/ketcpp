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

#include "config/ketcpp_config.h"

#ifdef EIGEN3_FOUND

#include <type_traits>

#include "wrapper/matrix/eigen_fixed.h"

namespace ketcpp::wrapper::matrix {
  //! Matrix instance using dynamic-size Eigen::Matrix as underlying matrix
  template <typename T>
  class MatrixEigen<T, 0, 0> : public MatrixEigenCommon<T> {

  private:
    typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                          Eigen::StorageOptions::RowMajor>
        matrix_t;
    matrix_t matrix;
    using Base = MatrixBase<T>;
    using Common = MatrixEigenCommon<T>;
    using common_matrix_ref = typename Common::common_matrix_ref;
    using const_common_matrix_ref = typename Common::const_common_matrix_ref;

    common_matrix_ref matrix_instance() override { return matrix; }
    const_common_matrix_ref matrix_instance() const override { return matrix; }

  public:
    size_t get_num_rows() const override { return matrix.rows(); }
    size_t get_num_columns() const override { return matrix.cols(); }
    size_t get_row_size() const override { return matrix.cols(); }
    size_t get_column_size() const override { return matrix.rows(); }

    T &at(size_t i, size_t j) override { return matrix(i, j); }
    T at(size_t i, size_t j) const override { return matrix(i, j); }

  private:
    //! @return Maximum length in inner lists
    static size_t
    max_size(const std::initializer_list<std::initializer_list<T>> &list) {
      const auto max_size_element =
          std::max_element(list.begin(), list.end(), [](auto &a, auto &b) {
            return a.size() < b.size();
          });
      return max_size_element->size();
    }

  public:
    //! @brief Initialize a matrix to specified size
    //! @post Constructed matrix is a zero-matrix with a dimension @p m,n
    MatrixEigen(size_t m, size_t n = 0) : matrix(m, n == 0 ? m : n) {}

    //! @brief Initialize a matrix with nested list
    MatrixEigen(const std::initializer_list<std::initializer_list<T>> &list) {
      size_t m = list.size(), n = max_size(list);
      matrix.resize(m, n);
      size_t i = 0;
      for (auto l : list) {
        typedef Eigen::Matrix<T, 1, Eigen::Dynamic> row_t;
        matrix.row(i++) = Eigen::Map<const row_t>(l.begin(), 1, l.size());
      }
    }

    //! Copy constructor
    MatrixEigen(const MatrixEigen &from) : matrix(from.matrix){};
    std::unique_ptr<Base> copy() const override {
      std::unique_ptr<Base> copy;
      copy.reset(new MatrixEigen(*this));
      return copy;
    }

    //! Copy constructor
    MatrixEigen(const Base &from)
        : matrix(from.get_num_rows(), from.get_num_columns()) {
      this->Base::operator=(from);
    }

    //! Copy constructor
    //! @param from A reference to an underlying Eigen::Matrix
    MatrixEigen(const const_common_matrix_ref &from) : matrix(from) {}
  };
}
#endif
