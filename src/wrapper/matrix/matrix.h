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
#if __has_include(<optional>)
#include <optional>
namespace {
  template <typename T> using optional = std::optional<T>;
};
#else
#include <experimental/optional>
namespace {
  template <typename T>
  struct optional : public std::experimental::optional<T> {
    using std::experimental::optional<T>::optional;
    bool has_value() { return static_cast<bool>(*this); }
  };
}
#endif

namespace ketcpp::wrapper::matrix {
  template <typename T> class MatrixBase;
  template <typename T> class Matrix {
  private:
    using Base = MatrixBase<T>;
    friend Base;
    using unique_ptr = std::unique_ptr<Base>;
    unique_ptr base;

  public:
    Matrix(const Matrix &src) : base(src.base->copy()) {}
    Matrix(const Base &src) : base(src.copy()) {}
    Matrix(Matrix &&src) : base(std::move(src.base)) {}
    Matrix(unique_ptr &&src) : base(std::move(src)) {}
    Matrix &operator=(const Matrix &src) {
      base = src.base->copy();
      return *this;
    }

    MatrixBase<T> *operator->() { return base.get(); }
    const MatrixBase<T> *operator->() const { return base.get(); }

    operator Base &() { return *base; }
    operator const Base &() const { return *base; }

    bool operator==(const Base &rhs) const { return *base == rhs; }
    bool operator!=(const Base &rhs) const { return *base != rhs; }

    Matrix operator+(const Base &rhs) { return *base + rhs; }
    Matrix operator-(const Base &rhs) { return *base - rhs; }
    Matrix operator*(T rhs) const { return *base * rhs; }
    Matrix operator/(T rhs) const { return *base / rhs; }

    Matrix &operator+=(const Base &rhs) {
      *base += rhs;
      return *this;
    }
    Matrix &operator-=(const Base &rhs) {
      *base -= rhs;
      return *this;
    }
    Matrix &operator*=(T rhs) {
      *base *= rhs;
      return *this;
    }
    Matrix &operator/=(T rhs) {
      *base /= rhs;
      return *this;
    }

    typedef T value_type;
    typedef typename MatrixBase<T>::iterator iterator;
    typedef typename MatrixBase<T>::const_iterator const_iterator;

    size_t size() const { return base->size(); }
    iterator begin() { return base->begin(); }
    iterator end() { return base->end(); }
    const_iterator begin() const { return base->begin(); }
    const_iterator end() const { return base->end(); }
    const_iterator cbegin() const { return base->cbegin(); }
    const_iterator cend() const { return base->cend(); }
  };

  template <typename T>
  std::ostream &operator<<(std::ostream &ost, const Matrix<T> &matrix) {
    return ost << static_cast<const MatrixBase<T> &>(matrix);
  }

  template <typename T> class MatrixBase {
  public:
    virtual size_t get_num_rows() const = 0;
    virtual size_t get_num_columns() const = 0;
    virtual size_t get_row_size() const = 0;
    virtual size_t get_column_size() const = 0;
    virtual T &at(size_t irow, size_t icol) = 0;
    virtual T at(size_t irow, size_t icol) const = 0;
    size_t size() const { return get_num_rows() * get_num_columns(); }

    template <typename F, typename R = typename std::result_of_t<
                              F(size_t, size_t)>::value_type>
    std::enable_if_t<
        std::is_same<std::result_of_t<F(size_t, size_t)>, optional<R>>::value,
        optional<R>>
    for_each(F lmd, size_t nr = 0, size_t nc = 0) const {
      if (nr == 0)
        nr = this->get_num_rows();
      if (nc == 0)
        nc = this->get_num_columns();
      for (size_t i = 0; i < nr; i++) {
        for (size_t j = 0; j < nc; j++) {
          auto ret = lmd(i, j);
          if (ret.has_value())
            [[unlikely]] return ret;
        }
      }
      return {};
    }
    template <typename F>
    std::enable_if_t<
        std::is_same<std::result_of_t<F(size_t, size_t)>, void>::value, void>
    for_each(F lmd, size_t nr = 0, size_t nc = 0) const {
      if (nr == 0)
        nr = this->get_num_rows();
      if (nc == 0)
        nc = this->get_num_columns();
      for (size_t i = 0; i < nr; i++) {
        for (size_t j = 0; j < nc; j++) {
          lmd(i, j);
        }
      }
    }

    Matrix<T> operator+(const MatrixBase &rhs) {
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) += rhs;
      return res;
    }
    Matrix<T> operator-(const MatrixBase &rhs) {
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) -= rhs;
      return res;
    }
    Matrix<T> operator*(T rhs) const {
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) *= rhs;
      return res;
    }
    Matrix<T> operator/(T rhs) const {
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) /= rhs;
      return res;
    }

    virtual MatrixBase &operator+=(const MatrixBase &rhs) {
      for_each(
          [this, &rhs](size_t i, size_t j) { this->at(i, j) += rhs.at(i, j); });
      return *this;
    }
    virtual MatrixBase &operator-=(const MatrixBase &rhs) {
      for_each(
          [this, &rhs](size_t i, size_t j) { this->at(i, j) -= rhs.at(i, j); });
      return *this;
    }
    virtual MatrixBase &operator*=(T rhs) {
      for_each([this, &rhs](size_t i, size_t j) { this->at(i, j) *= rhs; });
      return *this;
    }
    virtual MatrixBase &operator/=(T rhs) {
      for_each([this, &rhs](size_t i, size_t j) { this->at(i, j) /= rhs; });
      return *this;
    }

    virtual bool operator!=(const MatrixBase &rhs) const {
      auto ret = for_each([this, &rhs](size_t i, size_t j) -> optional<bool> {
        if (this->at(i, j) != rhs.at(i, j))
          return true; // abort loop and return true
        return {};     // resume loop
      });

      return ret.value_or(false); // ret has value when the loop has finished
    }
    virtual bool operator==(const MatrixBase &rhs) const {
      return !((*this) != rhs);
    }
    bool operator!=(const Matrix<T> &rhs) const {
      return (*this) != (*rhs.base.get());
    }
    bool operator==(const Matrix<T> &rhs) const {
      return (*this) == (*rhs.base.get());
    }

    virtual std::unique_ptr<MatrixBase> copy() const = 0;

  private:
    struct refwrapper : public std::reference_wrapper<T> {
      using std::reference_wrapper<T>::reference_wrapper;
      T &operator=(T v) { return static_cast<T &>(*this) = v; }
    };
    std::unique_ptr<std::list<refwrapper>> reflist = nullptr;
    void prepare_reflist() {
      if (reflist != nullptr)
        return;
      reflist.reset(new std::list<refwrapper>);
      for_each([this](size_t i, size_t j) { reflist->push_back(at(i, j)); });
    }
    void prepare_reflist_const() const {
      const_cast<MatrixBase<T> *>(this)->prepare_reflist();
    }

  public:
    auto begin() {
      prepare_reflist();
      return reflist->begin();
    }
    auto end() {
      prepare_reflist();
      return reflist->end();
    }
    auto cbegin() const {
      prepare_reflist_const();
      return reflist->cbegin();
    }
    auto cend() const {
      prepare_reflist_const();
      return reflist->cend();
    }
    auto begin() const { return cbegin(); }
    auto end() const { return cend(); }

    MatrixBase() : reflist(nullptr) {}
    MatrixBase(const MatrixBase<T> &) : reflist(nullptr) {}

    typedef T value_type;
    typedef decltype(reflist->begin()) iterator;
    typedef decltype(reflist->cbegin()) const_iterator;

    virtual ~MatrixBase(){};
  };

  template <typename T, typename U>
  std::enable_if_t<std::is_convertible<U, T>::value, Matrix<T>>
  operator*(U scalar, const Matrix<T> &mat) {
    return mat * static_cast<T>(scalar);
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &out, const MatrixBase<T> &matrix) {
    for (size_t i = 0; i < matrix.get_num_rows(); i++) {
      out << '{';
      for (size_t j = 0; j < matrix.get_num_columns(); j++) {
        out << matrix.at(i, j);
        if (j < matrix.get_num_columns() - 1)
          out << ',';
      }
      out << '}';
      if (i < matrix.get_num_rows() - 1)
        out << std::endl;
    }
    return out;
  }
}
