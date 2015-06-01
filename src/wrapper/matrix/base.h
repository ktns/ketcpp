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

#include <functional>
#include <memory>

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T> class MatrixBase {
        using iter_traits = std::iterator<std::random_access_iterator_tag, T>;

      protected:
        class BaseIterator {
          using unique_ptr = std::unique_ptr<BaseIterator>;

        public:
          virtual void advance_in_column() = 0;
          virtual void advance_in_row() = 0;
          virtual unique_ptr row_begin() = 0;
          virtual unique_ptr row_end() = 0;
          virtual unique_ptr column_begin() = 0;
          virtual unique_ptr column_end() = 0;
          virtual unique_ptr copy() = 0;
          virtual bool operator==(BaseIterator &rhs) throw(std::bad_cast &) = 0;
          virtual bool operator!=(BaseIterator &rhs) throw(std::bad_cast &) {
            return !(*this == rhs);
          }
          virtual T &operator*() = 0;
        };
        class BaseDelegateIterator : public iter_traits {
        protected:
          using unique_ptr = std::unique_ptr<BaseIterator>;
          std::unique_ptr<BaseIterator> iterator;
          BaseDelegateIterator(const BaseDelegateIterator &src)
              : iterator(std::move(src.iterator->copy())) {}
          BaseDelegateIterator(const BaseIterator &src) {
            src.copy_to(iterator);
          }
          BaseDelegateIterator(unique_ptr &&src) : iterator(std::move(src)) {}

        public:
          bool operator==(const BaseDelegateIterator &rhs) {
            *iterator == *rhs.iterator;
          }
          bool operator!=(const BaseDelegateIterator &rhs) {
            return *iterator != *rhs.iterator;
          }
          auto operator-(const BaseDelegateIterator &rhs) {
            return *iterator - *rhs.iterator;
          }
        };

      public:
        class RowElementIterator : public BaseDelegateIterator {
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          RowElementIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_row();
            return *this;
          }
          T &operator*() { return **this->iterator; }
        };
        class RowVectorIterator : public BaseDelegateIterator {
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          RowVectorIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_column();
            return *this;
          }
          auto &operator*() { return *this; }
          auto begin() {
            auto new_ptr = std::move(this->iterator->row_begin());
            return RowElementIterator(std::move(new_ptr));
          }
          auto end() {
            auto new_ptr = std::move(this->iterator->row_end());
            return RowElementIterator(std::move(new_ptr));
          }
        };
        class ColumnElementIterator : public BaseDelegateIterator {
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          ColumnElementIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_column();
            return *this;
          }
          T &operator*() { return **this->iterator; }
        };
        class ColumnVectorIterator : public BaseDelegateIterator {
          typedef typename BaseDelegateIterator::unique_ptr unique_ptr;

        public:
          ColumnVectorIterator(unique_ptr &&src)
              : BaseDelegateIterator(std::move(src)) {}
          auto &operator++() {
            this->iterator->advance_in_row();
            return *this;
          }
          auto &operator*() { return *this; }
          auto begin() {
            auto new_ptr = std::move(this->iterator->column_begin());
            return ColumnElementIterator(std::move(new_ptr));
          }
          auto end() {
            auto new_ptr = std::move(this->iterator->column_end());
            return ColumnElementIterator(std::move(new_ptr));
          }
        };

      protected:
        virtual RowVectorIterator row_begin() = 0;
        virtual RowVectorIterator row_end() = 0;
        virtual ColumnVectorIterator column_begin() = 0;
        virtual ColumnVectorIterator column_end() = 0;

      public:
        class rows_t {
          MatrixBase &matrix;

        public:
          rows_t(MatrixBase &mat) : matrix(mat) {}
          auto begin() { return matrix.row_begin(); }
          auto end() { return matrix.row_end(); }
        };
        friend rows_t;
        rows_t rows() { return rows_t(*this); }

        class columns_t {
          MatrixBase &matrix;

        public:
          columns_t(MatrixBase &mat) : matrix(mat) {}
          auto begin() { return matrix.column_begin(); }
          auto end() { return matrix.column_end(); }
        };
        friend columns_t;
        auto columns() { return columns_t(*this); }

        virtual ~MatrixBase() = 0;
      };
      template <typename T> MatrixBase<T>::~MatrixBase() {}
    }
  }
}
