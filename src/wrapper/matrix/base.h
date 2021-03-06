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

#include <functional>
#include <memory>

#include <infix_iterator.h>

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T> class MatrixBase {
      public:
        virtual size_t get_num_rows() const = 0;
        virtual size_t get_num_columns() const = 0;
        virtual size_t get_row_size() const = 0;
        virtual size_t get_column_size() const = 0;

      private:
        using iter_traits = std::iterator<std::random_access_iterator_tag, T>;

      protected:
        template <bool is_const> class BaseGenericIterator {
          using unique_ptr = std::unique_ptr<BaseGenericIterator>;

        public:
          using difference_type = typename iter_traits::difference_type;

          virtual void advance_in_column() = 0;
          virtual void advance_in_row() = 0;
          virtual unique_ptr row_begin() = 0;
          virtual unique_ptr row_end() = 0;
          virtual unique_ptr column_begin() = 0;
          virtual unique_ptr column_end() = 0;
          virtual unique_ptr copy() = 0;
          virtual bool operator==(BaseGenericIterator &rhs) const
              throw(std::bad_cast &) = 0;
          virtual bool operator!=(BaseGenericIterator &rhs) const
              throw(std::bad_cast &) {
            return !(*this == rhs);
          }
          virtual typename std::conditional<is_const, const T &, T &>::type
          operator*() = 0;
          virtual difference_type
          operator-(const BaseGenericIterator &) const = 0;
        };
        template <bool is_const>
        class BaseDelegateGenericIterator : public iter_traits {
        protected:
          using BaseIterator = BaseGenericIterator<is_const>;
          using unique_ptr = std::unique_ptr<BaseIterator>;
          unique_ptr iterator;
          BaseDelegateGenericIterator(const BaseDelegateGenericIterator &src)
              : iterator(std::move(src.iterator->copy())) {}
          BaseDelegateGenericIterator(const BaseIterator &src) {
            src.copy_to(iterator);
          }
          BaseDelegateGenericIterator(unique_ptr &&src)
              : iterator(std::move(src)) {}

        public:
          bool operator==(const BaseDelegateGenericIterator &rhs) const {
            return *iterator == *rhs.iterator;
          }
          bool operator!=(const BaseDelegateGenericIterator &rhs) const {
            return *iterator != *rhs.iterator;
          }
          auto operator-(const BaseDelegateGenericIterator &rhs) const {
            return *iterator - *rhs.iterator;
          }
        };

      public:
        template <bool is_const>
        class RowElementGenericIterator
            : public BaseDelegateGenericIterator<is_const> {
          using BaseDelegateIterator = BaseDelegateGenericIterator<is_const>;
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          RowElementGenericIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_row();
            return *this;
          }
          typename std::conditional<is_const, const T &, T &>::type
          operator*() {
            return **this->iterator;
          }
        };
        template <bool is_const>
        class RowVectorGenericIterator
            : public BaseDelegateGenericIterator<is_const> {
          using BaseDelegateIterator = BaseDelegateGenericIterator<is_const>;
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          RowVectorGenericIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_column();
            return *this;
          }
          auto &operator*() { return *this; }
          auto begin() {
            auto new_ptr = std::move(this->iterator->row_begin());
            return RowElementGenericIterator<is_const>(std::move(new_ptr));
          }
          auto end() {
            auto new_ptr = std::move(this->iterator->row_end());
            return RowElementGenericIterator<is_const>(std::move(new_ptr));
          }
        };
        template <bool is_const>
        class ColumnElementGenericIterator
            : public BaseDelegateGenericIterator<is_const> {
          using BaseDelegateIterator = BaseDelegateGenericIterator<is_const>;
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          ColumnElementGenericIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_column();
            return *this;
          }
          typename std::conditional<is_const, const T &, T &>::type
          operator*() {
            return **this->iterator;
          }
        };
        template <bool is_const>
        class ColumnVectorGenericIterator
            : public BaseDelegateGenericIterator<is_const> {
          using BaseDelegateIterator = BaseDelegateGenericIterator<is_const>;
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          ColumnVectorGenericIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_row();
            return *this;
          }
          auto &operator*() { return *this; }
          auto begin() {
            auto new_ptr = std::move(this->iterator->column_begin());
            return ColumnElementGenericIterator<is_const>(std::move(new_ptr));
          }
          auto end() {
            auto new_ptr = std::move(this->iterator->column_end());
            return ColumnElementGenericIterator<is_const>(std::move(new_ptr));
          }
        };
        typedef RowVectorGenericIterator<false> RowVectorIterator;
        typedef RowElementGenericIterator<false> RowElementIterator;
        typedef ColumnVectorGenericIterator<false> ColumnVectorIterator;
        typedef ColumnElementGenericIterator<false> ColumnElementIterator;
        typedef RowVectorGenericIterator<true> RowVectorConstIterator;
        typedef RowElementGenericIterator<true> RowElementConstIterator;
        typedef ColumnVectorGenericIterator<true> ColumnVectorConstIterator;
        typedef ColumnElementGenericIterator<true> ColumnElementConstIterator;

      protected:
        virtual RowVectorIterator row_begin() = 0;
        virtual RowVectorIterator row_end() = 0;
        virtual ColumnVectorIterator column_begin() = 0;
        virtual ColumnVectorIterator column_end() = 0;
        virtual RowVectorConstIterator row_cbegin() const = 0;
        virtual RowVectorConstIterator row_cend() const = 0;
        virtual ColumnVectorConstIterator column_cbegin() const = 0;
        virtual ColumnVectorConstIterator column_cend() const = 0;
        virtual RowVectorConstIterator row_begin() const {
          return row_cbegin();
        }
        virtual RowVectorConstIterator row_end() const { return row_cend(); }
        virtual ColumnVectorConstIterator column_begin() const {
          return column_cbegin();
        }
        virtual ColumnVectorConstIterator column_end() const {
          return column_cend();
        }

      public:
        template <bool is_const> class rows_t {
          using MatrixRef =
              typename std::conditional<is_const, const MatrixBase &,
                                        MatrixBase &>::type;
          MatrixRef matrix;

        public:
          rows_t(MatrixRef mat) : matrix(mat) {}
          auto begin() const { return matrix.row_begin(); }
          auto end() const { return matrix.row_end(); }
          auto cbegin() const { return matrix.row_cbegin(); }
          auto cend() const { return matrix.row_cend(); }
        };
        friend rows_t<true>;
        friend rows_t<false>;
        auto rows() { return rows_t<false>(*this); }
        auto rows() const { return rows_t<true>(*this); }

        template <bool is_const> class columns_t {
          using MatrixRef =
              typename std::conditional<is_const, const MatrixBase &,
                                        MatrixBase &>::type;
          MatrixRef matrix;

        public:
          columns_t(MatrixRef &mat) : matrix(mat) {}
          auto begin() const { return matrix.column_begin(); }
          auto end() const { return matrix.column_end(); }
          auto cbegin() const { return matrix.column_cbegin(); }
          auto cend() const { return matrix.column_cend(); }
        };
        friend columns_t<true>;
        friend columns_t<false>;
        auto columns() { return columns_t<false>(*this); }
        auto columns() const { return columns_t<true>(*this); }

        virtual std::unique_ptr<MatrixBase> operator+(const MatrixBase &rhs) {
          auto new_ptr = std::move(this->copy());
          auto &new_matrix = *new_ptr;
          new_matrix += rhs;
          return std::move(new_ptr);
        }
        virtual std::unique_ptr<MatrixBase> operator*(T rhs) {
          auto new_ptr = std::move(this->copy());
          auto &new_matrix = *new_ptr;
          new_matrix *= rhs;
          return std::move(new_ptr);
        }
        virtual MatrixBase &operator+=(const MatrixBase &rhs) {
          for (auto riters = std::make_pair(rows().begin(), rhs.rows().begin());
               riters.first != rows().end() &&
                   riters.second != rhs.rows().end();
               ++riters.first, ++riters.second) {
            auto &lr = riters.first;
            auto &rr = riters.second;
            for (auto citers = std::make_pair(lr.begin(), rr.begin());
                 citers.first != lr.end() && citers.second != rr.end();
                 ++citers.first, ++citers.second) {
              auto &l = citers.first;
              auto &r = citers.second;
              *l += *r;
            }
          }
          return *this;
        };
        MatrixBase &operator+=(const std::unique_ptr<MatrixBase> &rhs) {
          return *this += *rhs;
        }
        virtual MatrixBase &operator*=(T rhs) = 0;

        virtual bool operator==(const MatrixBase &rhs) const {
          for (auto iters = std::make_pair(rows().begin(), rhs.rows().begin());
               iters.first != rows().end() && iters.second != rhs.rows().end();
               ++iters.first, ++iters.second) {
            auto &l = iters.first, &r = iters.second;
            if (!std::equal(l.begin(), l.end(), r.begin(), r.end())) {
              return false;
            }
          }
          return true;
        }

        virtual std::unique_ptr<MatrixBase> copy() const = 0;

        virtual ~MatrixBase(){};
      };

      template <typename T>
      std::ostream &operator<<(std::ostream &out, const MatrixBase<T> &matrix) {
        infix_ostream_iterator<std::string> lines(out, "}, {");
        out << "{{";
        std::transform(
            matrix.rows().cbegin(), matrix.rows().cend(), lines,
            [&out](typename MatrixBase<T>::RowVectorConstIterator &row)
                -> std::string {
                  std::stringstream ss;
                  infix_ostream_iterator<T> line(ss, ", ");
                  std::copy(row.begin(), row.end(), line);
                  return ss.str();
                });
        return out << "}}";
      }
    }
  }
}
