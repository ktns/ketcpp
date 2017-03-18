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
#else
#include <experimental/optional>
#endif

//! Matrix wrapper classes
namespace ketcpp::wrapper::matrix {
//! @cond PRIVATE
#if __has_include(<optional>)
  template <typename T> using optional = std::optional<T>;
#else
  template <typename T>
  struct optional : public std::experimental::optional<T> {
    using std::experimental::optional<T>::optional;
    bool has_value() { return static_cast<bool>(*this); }
  };
#endif
  //! @endcond

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

  template <typename T> Matrix<T> make_zero_matrix(size_t m, size_t n = 0);

  //! Base class for each matrix wrapper.
  template <typename T> class MatrixBase {
  public:
    //! Number of rows in the matrix.
    virtual size_t get_num_rows() const = 0;
    //! Number of columns in the matrix.
    virtual size_t get_num_columns() const = 0;
    virtual size_t get_row_size() const = 0;
    virtual size_t get_column_size() const = 0;
    //! Element accessor
    virtual T &at(size_t irow, size_t icol) = 0;
    //! Constant element accessor
    virtual T at(size_t irow, size_t icol) const = 0;
    //! @brief Dimension of the matrix.
    //! @return <tt>std::pair<size_t, size_t></tt> that represents the
    //! dimension.
    auto dimension() const {
      return std::make_pair(get_num_rows(), get_num_columns());
    }
    //! Number of elements in total.
    size_t size() const { return get_num_rows() * get_num_columns(); }

    //! @brief Call @p lmd for each index pair.
    //! @details Call @p lmd in the column-major order, and abort if
    //! lmd returns valid value.
    //! @tparam F Functor that accepts two index @p size_t and
    //! returns @p optional<R> of any type.
    //! @tparam R Return type of functor @p F.
    //! @param [in] lmd Functor that accepts two index @p size_t and
    //! returns @p optional of any type.
    //! @param [in] nr,nc Index ranges. Defaults to numbers of rows and
    //! columns, respectively.
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
    //! @brief Call @p lmd for each index pair.
    //! @details Call @p lmd in the column-major order
    //! @tparam F Functor that accepts two index @p size_t and
    //! returns nothing.
    //! @param [in] lmd Functor that accepts two index @p size_t and
    //! returns nothing.
    //! @param [in] nr,nc Index ranges. Defaults to numbers of rows and
    //! columns, respectively.
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

    //! @brief Assignment operator.
    //! @pre @p lhs and @p rhs have the same dimension.
    //! @post @p lhs equals to @p rhs.
    MatrixBase &operator=(const MatrixBase &rhs) {
      assert(this->dimension() == rhs.dimension());
      for_each(
          [this, &rhs](size_t i, size_t j) { this->at(i, j) = rhs.at(i, j); });
      assert(*this == rhs);
      return *this;
    }

    //! @brief Addition operator.
    //! @pre @p lhs and @p rhs have the same dimension.
    //! @returns <tt>lhs + rhs</tt>
    Matrix<T> operator+(const MatrixBase &rhs) const {
      assert(this->dimension() == rhs.dimension());
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) += rhs;
      return res;
    }
    //! @brief Subtraction operator.
    //! @pre @p lhs and @p rhs have the same dimension.
    //! @returns <tt>lhs - rhs</tt>
    Matrix<T> operator-(const MatrixBase &rhs) const {
      assert(this->dimension() == rhs.dimension());
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) -= rhs;
      return res;
    }
    //! @brief Multiplication operator with a scalar.
    //! @returns Scaled matrix <tt>lhs * rhs</tt>
    Matrix<T> operator*(T rhs) const {
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) *= rhs;
      return res;
    }
    //! @brief Division operator with a scalar.
    //! @returns Scaled matrix <tt>lhs / rhs</tt>
    Matrix<T> operator/(T rhs) const {
      Matrix<T> res(*this);
      static_cast<MatrixBase<T> &>(res) /= rhs;
      return res;
    }
    //! @brief Multiplication operator with a matrix.
    //! @pre The number of columns in @p lhs and the number of rows in @p rhs
    //! are the same.
    //! @returns Multiplied matrix <tt>lhs * rhs</tt>
    virtual Matrix<T> operator*(const MatrixBase &rhs) const {
      assert(this->get_num_columns() == rhs.get_num_rows());
      const size_t l = this->get_num_columns(), m = this->get_num_rows(),
                   n = rhs.get_num_columns();
      auto res = make_zero_matrix<T>(m, n);
      res->for_each([this, &rhs, &res, l](size_t i, size_t j) {
        for (size_t k = 0; k < l; k++) {
          res->at(i, j) += this->at(i, k) * rhs.at(k, j);
        }
      });
      assert(this->get_num_rows() == res->get_num_rows());
      assert(rhs.get_num_columns() == res->get_num_columns());
      return res;
    }

    //! @brief Addition assignment operator with a matrix.
    //! @pre @p lhs and @p rhs have the same dimension.
    //! @post Matrix equals to <tt>lhs + rhs</tt>
    virtual MatrixBase &operator+=(const MatrixBase &rhs) {
      assert(this->dimension() == rhs.dimension());
      for_each(
          [this, &rhs](size_t i, size_t j) { this->at(i, j) += rhs.at(i, j); });
      return *this;
    }
    //! @brief Subtraction assignment operator with a matrix.
    //! @pre @p lhs and @p rhs have the same dimension.
    //! @post Matrix equals to <tt>lhs + rhs</tt>
    virtual MatrixBase &operator-=(const MatrixBase &rhs) {
      for_each(
          [this, &rhs](size_t i, size_t j) { this->at(i, j) -= rhs.at(i, j); });
      return *this;
    }
    //! @brief Multiplication assignment operator with a scalar.
    //! @post Matrix equals to scaled matrix <tt>lhs * rhs</tt>
    virtual MatrixBase &operator*=(T rhs) {
      for_each([this, &rhs](size_t i, size_t j) { this->at(i, j) *= rhs; });
      return *this;
    }
    //! @brief Division assignment operator with a scalar.
    //! @post Matrix equals to scaled matrix <tt>lhs * rhs</tt>
    virtual MatrixBase &operator/=(T rhs) {
      for_each([this, &rhs](size_t i, size_t j) { this->at(i, j) /= rhs; });
      return *this;
    }

    //! @brief Inequality operator.
    //! @pre @p lhs and @p rhs have the same dimension.
    virtual bool operator!=(const MatrixBase &rhs) const {
      assert(this->dimension() == rhs.dimension());
      auto ret = for_each([this, &rhs](size_t i, size_t j) -> optional<bool> {
        if (this->at(i, j) != rhs.at(i, j))
          return true; // abort loop and return true
        return {};     // resume loop
      });

      return ret.value_or(false); // ret has value when the loop has finished
    }
    //! @brief Equality operator.
    //! @pre @p lhs and @p rhs have the same dimension.
    virtual bool operator==(const MatrixBase &rhs) const {
      return !((*this) != rhs);
    }

    //! Creates a copied matrix.
    virtual std::unique_ptr<MatrixBase> copy() const = 0;

  private:
    //! @cond PRIVATE
    class reflist_t : private std::list<std::reference_wrapper<T>> {

    private:
      using base_t = std::list<std::reference_wrapper<T>>;
      template <typename Iter>
      using iterator_base_t = std::iterator<
          typename std::iterator_traits<Iter>::iterator_category, T,
          typename std::iterator_traits<Iter>::difference_type,
          typename std::conditional_t<
              std::is_same_v<Iter, typename base_t::const_iterator>, const T *,
              T *>,
          typename std::conditional_t<
              std::is_same_v<Iter, typename base_t::const_iterator>, T, T &>>;
      template <typename Iter> class iterator_t : public iterator_base_t<Iter> {
      private:
        Iter iter;

      public:
        typedef typename iterator_base_t<Iter>::difference_type difference_type;
        typedef typename iterator_base_t<Iter>::reference reference;

        iterator_t() : iter() {}
        iterator_t(Iter iter) : iter(iter) {}
        iterator_t(const iterator_t &iter) : iter(iter.iter) {}
        reference operator*() { return *iter; }
        iterator_t &operator++() {
          ++iter;
          return *this;
        }
        iterator_t operator++(int) {
          auto ret = *this;
          ++(*this);
          return ret;
        }
        bool operator==(iterator_t other) const { return iter == other.iter; }
        bool operator!=(iterator_t other) const { return iter != other.iter; }
      };

    public:
      reflist_t() = default;
      reflist_t(const reflist_t &) = delete;
      void push_back(T &ref) { base_t::push_back(ref); }
      typedef iterator_t<typename base_t::iterator> iterator;
      typedef iterator_t<typename base_t::const_iterator> const_iterator;
      iterator begin() { return base_t::begin(); }
      iterator end() { return base_t::end(); }
      const_iterator begin() const { return base_t::cbegin(); }
      const_iterator end() const { return base_t::cend(); }
      const_iterator cbegin() const { return base_t::cbegin(); }
      const_iterator cend() const { return base_t::cend(); }
    };
    std::unique_ptr<reflist_t> reflist = nullptr;
    void prepare_reflist() {
      if (reflist != nullptr)
        return;
      reflist.reset(new reflist_t);
      for_each([this](size_t i, size_t j) { reflist->push_back(at(i, j)); });
    }
    void prepare_reflist_const() const {
      const_cast<MatrixBase<T> *>(this)->prepare_reflist();
    }
    //! @endcond

  public:
    //! Iterator that traverse a matrix in the column-major order.
    typedef decltype(reflist->begin()) iterator;
    //! Constant iterator that traverse a matrix in the column-major order.
    typedef decltype(reflist->cbegin()) const_iterator;

    //! Returns a iterator to the beginning of the matrix.
    auto begin() {
      prepare_reflist();
      return reflist->begin();
    }
    //! Returns a iterator to the ending of the matrix.
    auto end() {
      prepare_reflist();
      return reflist->end();
    }
    //! Returns a iterator to the beginning of the matrix.
    auto cbegin() const {
      prepare_reflist_const();
      return reflist->cbegin();
    }
    //! Returns a iterator to the ending of the matrix.
    auto cend() const {
      prepare_reflist_const();
      return reflist->cend();
    }
    //! Returns a iterator to the beginning of the matrix.
    auto begin() const { return cbegin(); }
    //! Returns a iterator to the ending of the matrix.
    auto end() const { return cend(); }

    //! Transpose matrix.
    //! @return Transposed matrix
    //! @post Does not change the instance.
    Matrix<T> transpose() const;

  protected:
    //! Protected default constructor.
    MatrixBase() : reflist(nullptr) {}
    //! Protected copy constructor.
    MatrixBase(const MatrixBase<T> &) : reflist(nullptr) {}

  public:
    //! Virtual destructor
    virtual ~MatrixBase() {}
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

  //! Outputs a matrix to an output stream
  template <typename T, typename Char, typename Traits>
  std::basic_ostream<Char, Traits> &
  operator<<(std::basic_ostream<Char, Traits> &out,
             const MatrixBase<T> &matrix) {
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

#include "wrapper/matrix/transposed.h"
