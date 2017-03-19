/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2015-2017 Katsuhiko Nishimra
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
#include <functional>
#include <list>
#include <memory>
#include <ostream>
#include <type_traits>

#include "util/optional.h"

//! Matrix wrapper classes
namespace ketcpp::wrapper::matrix {
  using ketcpp::util::optional;

  template <typename T> class MatrixBase;
  //! Matrix wrapper class that conceals implementations of matrix instances.
  template <typename T> class Matrix {
  private:
    using Base = MatrixBase<T>;
    friend Base;
    using unique_ptr = std::unique_ptr<Base>;
    unique_ptr base;

  public:
    //! Type of each element of the matrix.
    typedef T value_type;
    //! Copy constructor
    Matrix(const Matrix &src) : base(src.base->copy()) {}
    //! Copy constructor
    Matrix(const Base &src) : base(src.copy()) {}
    //! Move constructor
    Matrix(Matrix &&src) : base(std::move(src.base)) {}
    //! Move constructor
    Matrix(unique_ptr &&src) : base(std::move(src)) {}
    //! Assignment operator
    Matrix &operator=(const Matrix &src) {
      base = src.base->copy();
      return *this;
    }

    //! Proxy all methods to the actual instance.
    MatrixBase<T> *operator->() { return base.get(); }
    //! Proxy all methods to the actual instance.
    const MatrixBase<T> *operator->() const { return base.get(); }

    //! Cast operator to <tt>MatrixBase<T>&</tt>.
    operator Base &() { return *base; }
    //! Cast operator to <tt>const MatrixBase<T>&</tt>.
    operator const Base &() const { return *base; }

    //! Proxy @p == operator to <tt>const MatrixBase<T>&</tt>.
    bool operator==(const Base &rhs) const { return *base == rhs; }
    //! Proxy @p != operator to <tt>const MatrixBase<T>&</tt>.
    bool operator!=(const Base &rhs) const { return *base != rhs; }

    //! Proxy @p + operator to <tt>const MatrixBase<T>&</tt>.
    Matrix operator+(const Base &rhs) const { return *base + rhs; }
    //! Proxy @p - operator to <tt>const MatrixBase<T>&</tt>.
    Matrix operator-(const Base &rhs) const { return *base - rhs; }
    //! Proxy @p * operator to <tt>const MatrixBase<T>&</tt>.
    Matrix operator*(const Base &rhs) const { return *base * rhs; }
    //! Proxy @p * operator to <tt>const MatrixBase<T>&</tt>.
    Matrix operator*(T rhs) const { return *base * rhs; }
    //! Proxy @p / operator to <tt>const MatrixBase<T>&</tt>.
    Matrix operator/(T rhs) const { return *base / rhs; }

    //! Proxy @p += operator to <tt>MatrixBase<T>&</tt>.
    Matrix &operator+=(const Base &rhs) {
      *base += rhs;
      return *this;
    }
    //! Proxy @p -= operator to <tt>MatrixBase<T>&</tt>.
    Matrix &operator-=(const Base &rhs) {
      *base -= rhs;
      return *this;
    }
    //! Proxy @p *= operator to <tt>MatrixBase<T>&</tt>.
    Matrix &operator*=(T rhs) {
      *base *= rhs;
      return *this;
    }
    //! Proxy @p /= operator to <tt>MatrixBase<T>&</tt>.
    Matrix &operator/=(T rhs) {
      *base /= rhs;
      return *this;
    }

    //! Iterator that traverse a matrix in the row-major order.
    typedef typename MatrixBase<T>::iterator iterator;
    //! Constant iterator that traverse a matrix in the column-major order.
    typedef typename MatrixBase<T>::const_iterator const_iterator;

    //! @brief Proxy @p size method to <tt>const MatrixBase<T>&</tt>.
    //! @note This is needed to handle @p Matrix as a container.
    size_t size() const { return base->size(); }
    //! @brief Proxy @p begin method to <tt>MatrixBase<T>&</tt>.
    //! @note This is needed to handle @p Matrix as a container.
    iterator begin() { return base->begin(); }
    //! @brief Proxy @p end method to <tt>MatrixBase<T>&</tt>.
    //! @note This is needed to handle @p Matrix as a container.
    iterator end() { return base->end(); }
    //! @brief Proxy @p begin method to <tt>const MatrixBase<T>&</tt>.
    //! @note This is needed to handle @p Matrix as a container.
    const_iterator begin() const { return base->cbegin(); }
    //! @brief Proxy @p end method to <tt>const MatrixBase<T>&</tt>.
    //! @note This is needed to handle @p Matrix as a container.
    const_iterator end() const { return base->cend(); }
    //! @brief Proxy @p cbegin method to <tt>const MatrixBase<T>&</tt>.
    //! @note This is needed to handle @p Matrix as a container.
    const_iterator cbegin() const { return base->cbegin(); }
    //! @brief Proxy @p cend method to <tt>const MatrixBase<T>&</tt>.
    //! @note This is needed to handle @p Matrix as a container.
    const_iterator cend() const { return base->cend(); }
  };

  //! Multiplication operator between a scalar and a matrix.
  template <typename T, typename U>
  std::enable_if_t<std::is_convertible<U, T>::value, Matrix<T>>
  operator*(U scalar, const Matrix<T> &mat) {
    return mat * static_cast<T>(scalar);
  }

  //! Outputs Matrix to an output stream
  template <typename T, typename Char, typename Traits>
  std::basic_ostream<Char, Traits> &
  operator<<(std::basic_ostream<Char, Traits> &out, const Matrix<T> &matrix) {
    return out << static_cast<const MatrixBase<T> &>(matrix);
  }
}

#include "wrapper/matrix/base.h"
