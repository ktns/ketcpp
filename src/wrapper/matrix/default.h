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

#include <cassert>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include "wrapper/matrix.h"

namespace ketcpp::wrapper::matrix {
  //! Factory method that creates an instance of default implementation of
  //! a fixed-size matrix.
  //! @param [in] list Nested initializer list
  template <typename T, size_t m, size_t n = m>
  Matrix<T> make_matrix(std::initializer_list<std::initializer_list<T>> list) {
    std::unique_ptr<MatrixBase<T>> ptr;
    ptr.reset(new MatrixArray<T, m, n>(list));
    return std::move(Matrix<T>(std::move(ptr)));
  }

  //! Factory method that creates an instance of default implementation of
  //! a fixed-size matrix.
  //! @param [in] list Non-nested initializer list
  template <typename T, size_t m, size_t n = m>
  Matrix<T> make_matrix(std::initializer_list<T> list) {
    std::unique_ptr<MatrixBase<T>> ptr;
    ptr.reset(new MatrixArray<T, m, n>(list));
    return std::move(Matrix<T>(std::move(ptr)));
  }

  //! Factory method that creates an instance of default implementation of
  //! a dynamic-size matrix.
  //! @param [in] list Nested initializer list
  template <typename T>
  Matrix<T> make_matrix(std::initializer_list<std::initializer_list<T>> list) {
    std::unique_ptr<MatrixBase<T>> ptr;
    ptr.reset(new MatrixVector<T>(list));
    return std::move(Matrix<T>(std::move(ptr)));
  }

  //! Factory method that creates an instance of default implementation of
  //! a dynamic-size zero-matrix.
  //! @param [in] m Number of rows in a matrix
  //! @param [in] n Number of columns in a matrix
  template <typename T> Matrix<T> make_zero_matrix(size_t m, size_t n) {
    if (n == 0)
      n = m;
    assert(n > 0 && m > 0);
    auto ptr = std::make_unique<MatrixVector<T>>(n, m);
    assert(ptr->for_each([&ptr](size_t i, size_t j) -> util::optional<bool> {
                if (ptr->at(i, j) == 0)
                  return {};
                else
                  return false;
              })
               .value_or(true));
    ;
    return Matrix<T>(std::move(ptr));
  }

  //! Factory method that creates an instance of default implementation of
  //! a dynamic-size symmetric matrix.
  //! @param [in] list Nested initializer list that contains lower
  //! triangular elements of the matrix in the row-major order.
  template <typename T>
  Matrix<T>
  make_symmetric_matrix(std::initializer_list<std::initializer_list<T>> list) {
    size_t i = 0;
    auto ptr = std::make_unique<MatrixVector<T>>(list.size());
    for (auto l : list) {
      assert(l.size() == i + 1);
      size_t j = 0;
      for (T v : l) {
        ptr->at(i, j) = ptr->at(j, i) = v;
        j++;
      }
      i++;
    }
    return Matrix<T>(std::move(ptr));
  }

  //! @brief Metafunction that transform an iterator type to a matrix type.
  //! @tparam Iter An iterator type over the type of elements in the matrix.
  //! @pre @p Iter is an input iterator.
  template <typename Iter>
  using matrix_from_input_iterator_t = std::enable_if_t<
      std::is_base_of_v<std::input_iterator_tag,
                        typename std::iterator_traits<Iter>::iterator_category>,
      Matrix<std::decay_t<typename std::iterator_traits<Iter>::value_type>>>;

  //! @brief Factory method that creates an instance of default implementation
  //! of a dynamic-size diagonal matrix.
  //! @param [in] begin,end iterators that iterates over diagonal elements
  //! of the matrix.
  template <typename Iter>
  matrix_from_input_iterator_t<Iter> make_diagonal_matrix(Iter begin,
                                                          Iter end) {
    typedef typename matrix_from_input_iterator_t<Iter>::value_type T;
    if
#ifdef __cpp_if_constexpr
      constexpr
#endif
          (std::is_base_of_v<
              std::forward_iterator_tag,
              typename std::iterator_traits<Iter>::iterator_category>) {
        const auto n = std::distance(begin, end);
        assert(n > 0);
        auto ret = make_zero_matrix<T>(n);
        size_t i = 0;
        for (Iter iter = begin; iter != end; i++, iter++) {
          ret->at(i, i) = *iter;
        }
        return ret;
      }
    else {
      std::vector<T> v(begin, end);
      return make_diagonal_matrix(v.cbegin(), v.cend());
    }
  }

  //! Factory method that creates an instance of default implementation of
  //! a dynamic-size diagonal matrix.
  //! @param [in] list A initializer list that holds diagonal elements of the
  //! returned matrix.
  template <typename T>
  Matrix<T> make_diagonal_matrix(std::initializer_list<T> list) {
    return make_diagonal_matrix(list.begin(), list.end());
  }

  //! Factory method that creates an instance of default implementation of
  //! a dynamic-size diagonal matrix.
  //! @param [in] container A container that contains diagonal elements of the
  //! returned matrix.
  template <typename Container>
  Matrix<typename Container::value_type>
  make_diagonal_matrix(const Container &container) {
    return make_diagonal_matrix(std::begin(container), std::end(container));
  }
}
