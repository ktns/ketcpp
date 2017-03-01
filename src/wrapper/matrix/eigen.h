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

#include <type_traits>

#include <Eigen/Dense>

#include "wrapper/matrix/matrix.h"

namespace ketcpp::wrapper::matrix {
  template <typename T, size_t Rows = 0, size_t Cols = Rows>
  class MatrixEigen : public MatrixBase<T> {
    // This difinition is for constant sized matrix.
    static_assert(Rows > 0 && Cols > 0);

  private:
    typedef Eigen::Matrix<T, Rows, Cols, Eigen::StorageOptions::RowMajor>
        matrix_t;
    matrix_t matrix;
    using Base = MatrixBase<T>;

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
    MatrixEigen() : matrix(matrix_t::Zero()){};
    MatrixEigen(const std::initializer_list<T> &list) {
      matrix = Eigen::Map<const matrix_t>(list.begin());
    }
    MatrixEigen(const std::initializer_list<std::initializer_list<T>> &list) {
      size_t i = 0;
      for (auto l : list) {
        typedef Eigen::Matrix<T, 1, num_columns> row_t;
        matrix.row(i++) = Eigen::Map<const row_t>(l.begin(), 1, l.size());
      }
    }

    MatrixEigen(const MatrixEigen &from) : matrix(from.matrix){};
    std::unique_ptr<MatrixBase<T>> copy() const override {
      std::unique_ptr<MatrixBase<T>> copy;
      copy.reset(new MatrixEigen(*this));
      return copy;
    }

    // bool operator==(const MatrixEigen &rhs) const {
    //  return matrix == rhs.matrix;
    //}

    Base &operator+=(const Base &rhsbase) override {
      auto *prhs = dynamic_cast<const MatrixEigen *>(&rhsbase);
      if (prhs == nullptr)
        return Base::operator+=(rhsbase);

      matrix += prhs->matrix;
      return *this;
    }

    Base &operator*=(T rhs) override {
      matrix *= rhs;
      return *this;
    }
  };

  template <typename T> class MatrixEigen<T, 0, 0> : public MatrixBase<T> {
    // This difinition is for variable sized matrix.

  private:
    typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                          Eigen::StorageOptions::RowMajor>
        matrix_t;
    matrix_t matrix;
    using Base = MatrixBase<T>;

  public:
    size_t get_num_rows() const override { return matrix.rows(); }
    size_t get_num_columns() const override { return matrix.cols(); }
    size_t get_row_size() const override { return matrix.cols(); }
    size_t get_column_size() const override { return matrix.rows(); }

    T &at(size_t i, size_t j) override { return matrix(i, j); }
    T at(size_t i, size_t j) const override { return matrix(i, j); }

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
    MatrixEigen(const std::initializer_list<std::initializer_list<T>> &list) {
      size_t m = list.size(), n = max_size(list);
      matrix.resize(m, n);
      size_t i = 0;
      for (auto l : list) {
        typedef Eigen::Matrix<T, 1, Eigen::Dynamic> row_t;
        matrix.row(i++) = Eigen::Map<const row_t>(l.begin(), 1, l.size());
      }
    }

    MatrixEigen(const MatrixEigen &from) : matrix(from.matrix){};
    std::unique_ptr<MatrixBase<T>> copy() const override {
      std::unique_ptr<MatrixBase<T>> copy;
      copy.reset(new MatrixEigen(*this));
      return copy;
    }

    bool operator==(const MatrixEigen &rhs) const {
      return matrix == rhs.matrix;
    }

    Base &operator+=(const Base &rhsbase) override {
      auto *prhs = dynamic_cast<const MatrixEigen *>(&rhsbase);
      if (prhs == nullptr)
        return Base::operator+=(rhsbase);

      matrix += prhs->matrix;
      return *this;
    }

    Base &operator*=(T rhs) override {
      matrix *= rhs;
      return *this;
    }
  };
}
#endif
