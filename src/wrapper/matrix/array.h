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

#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>

#include "wrapper/matrix/base.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T, size_t m, size_t n = m>
      class MatrixArray : public MatrixBase<T> {
      public:
        constexpr static size_t num_rows = m;
        constexpr static size_t num_columns = n;
        constexpr static size_t row_size = n;
        constexpr static size_t column_size = m;

      private:
        using array = std::array<T, m * n>;
        array storage;
        using Base = MatrixBase<T>;
        typedef typename Base::RowVectorIterator RowVectorIterator;
        typedef typename Base::RowElementIterator RowElementIterator;
        typedef typename Base::ColumnVectorIterator ColumnVectorIterator;
        typedef typename Base::ColumnElementIterator ColumnElementIterator;
        typedef typename Base::RowVectorConstIterator RowVectorConstIterator;
        typedef typename Base::RowElementConstIterator RowElementConstIterator;
        typedef
            typename Base::ColumnVectorConstIterator ColumnVectorConstIterator;
        typedef typename Base::ColumnElementConstIterator
            ColumnElementConstIterator;

        template <bool is_const>
        class GenericIterator : public Base::template BaseIterator<is_const> {
          using BaseIterator = typename Base::template BaseIterator<is_const>;
          using unique_ptr = std::unique_ptr<BaseIterator>;
          typename std::conditional<
              is_const, typename MatrixArray::array::const_iterator,
              typename MatrixArray::array::iterator>::type iterator;
          constexpr static size_t row_size = MatrixArray::row_size;
          constexpr static size_t column_size = MatrixArray::column_size;

        protected:
          void advance_in_column() { this->iterator += row_size; }
          void advance_in_row() { this->iterator++; }
          unique_ptr row_begin() {
            return std::move(std::make_unique<GenericIterator>(this->iterator));
          }
          unique_ptr row_end() {
            return std::move(
                std::make_unique<GenericIterator>(this->iterator + row_size));
          }
          unique_ptr column_begin() {
            return std::move(std::make_unique<GenericIterator>(this->iterator));
          }
          unique_ptr column_end() {
            return std::move(std::make_unique<GenericIterator>(
                this->iterator + row_size * column_size));
          }
          unique_ptr copy() {
            return std::move(std::make_unique<GenericIterator>(this->iterator));
          }
          bool operator==(BaseIterator &rhs) throw(std::bad_cast &) {
            auto &rhs_cast = dynamic_cast<decltype(*this)>(rhs);
            return this->iterator == rhs_cast.iterator;
          }
          bool operator!=(BaseIterator &rhs) throw(std::bad_cast &) {
            auto &rhs_cast = dynamic_cast<decltype(*this)>(rhs);
            return this->iterator != rhs_cast.iterator;
          }
          typename BaseIterator::difference_type
          operator-(const BaseIterator &rhs) const throw(std::bad_cast &) {
            auto &rhs_cast = dynamic_cast<decltype(*this)>(rhs);
            return this->iterator - rhs_cast.iterator;
          }
          typename std::conditional<is_const, const T &, T &>::type
          operator*() {
            return *iterator;
          }

        public:
          GenericIterator(decltype(iterator) src) : iterator(src) {}
        };
        using Iterator = GenericIterator<false>;
        using ConstIterator = GenericIterator<true>;

      protected:
        RowVectorConstIterator row_cbegin() const {
          return RowVectorConstIterator(
              std::make_unique<ConstIterator>(storage.cbegin()));
        }
        RowVectorConstIterator row_cend() const {
          return RowVectorConstIterator(
              std::move(std::make_unique<ConstIterator>(storage.cend())));
        }
        ColumnVectorConstIterator column_cbegin() const {
          return ColumnVectorConstIterator(
              std::move(std::make_unique<ConstIterator>(storage.cbegin())));
        }
        ColumnVectorConstIterator column_cend() const {
          return ColumnVectorConstIterator(std::move(
              std::make_unique<ConstIterator>(storage.cbegin() + row_size)));
        }
        RowVectorIterator row_begin() {
          return RowVectorIterator(std::make_unique<Iterator>(storage.begin()));
        }
        RowVectorIterator row_end() {
          return RowVectorIterator(
              std::move(std::make_unique<Iterator>(storage.end())));
        }
        ColumnVectorIterator column_begin() {
          return ColumnVectorIterator(
              std::move(std::make_unique<Iterator>(storage.begin())));
        }
        ColumnVectorIterator column_end() {
          return ColumnVectorIterator(std::move(
              std::make_unique<Iterator>(storage.begin() + row_size)));
        }

      public:
        MatrixArray(
            const std::initializer_list<std::initializer_list<float>> &list)
            : storage() {
          auto dest = storage.begin();
          for (auto i : list) {
            std::copy(i.begin(), i.end(), dest);
          }
        }
        MatrixArray(const std::initializer_list<float> &list) : storage() {
          std::copy(list.begin(), list.end(), storage.begin());
        }
        MatrixArray() = default;

        bool operator==(const MatrixArray &rhs) const {
          return std::equal(this->storage.cbegin(), this->storage.cend(),
                            rhs.storage.cbegin());
        }

        MatrixArray &operator+=(const MatrixArray &rhs) {
          std::transform(this->storage.cbegin(), this->storage.cend(),
                         rhs.storage.cbegin(), this->storage.begin(),
                         [](T l, T r) -> T { return l + r; });
          return *this;
        }

        template <typename T2>
        typename std::enable_if<std::is_convertible<T2, T>::value,
                                MatrixArray>::type &
        operator*=(T2 rhs) {
          std::transform(this->storage.cbegin(), this->storage.cend(),
                         this->storage.begin(),
                         [rhs](T l) -> T { return l * rhs; });
          return *this;
        }
        template <typename T2>
        typename std::enable_if<std::is_convertible<T2, T>::value,
                                MatrixArray>::type
        operator*(T2 rhs) {
          auto new_matrix = *this;
          return new_matrix *= rhs;
        }
        template <size_t l>
        MatrixArray<T, m, l> operator*(const MatrixArray<T, n, l> &rhs) const {
          MatrixArray<T, m, l> buf;
          auto lr = this->rows().begin();
          for (auto br = buf.rows().begin(); br != buf.rows().end();
               ++br, ++lr) {
            auto rc = rhs.columns().begin();
            for (auto b = br.begin(); b != br.end(); ++b, ++rc) {
              *b = std::inner_product(lr.begin(), lr.end(), rc.begin(), 0);
            }
          }
          return std::move(buf);
        }
        ~MatrixArray() {}
      };
    }
  }
}
