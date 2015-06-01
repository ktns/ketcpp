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
#include <sstream>

#include <infix_iterator.h>
#include "wrapper/matrix/base.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T, size_t m, size_t n = m>
      class MatrixArray : public MatrixBase<T> {
      public:
        constexpr static size_t row_size = n;
        constexpr static size_t column_size = m;

      private:
        using array = std::array<T, m * n>;
        array storage;

      public:
        class row_element_iterator
            : public std::iterator<std::random_access_iterator_tag, T> {
          typename array::iterator iterator;
          friend MatrixArray<T, m, n>;
          friend class row_iterator;
          row_element_iterator(const typename array::iterator &i)
              : iterator(i) {}

        public:
          typedef typename std::iterator<std::random_access_iterator_tag,
                                         row_element_iterator>::difference_type
              diff_t;
          row_element_iterator(const row_element_iterator &i)
              : iterator(i.iterator) {}
          ~row_element_iterator() {}
          bool operator==(const row_element_iterator &rhs) {
            return iterator == rhs.iterator;
          }
          bool operator!=(const row_element_iterator &rhs) {
            return iterator != rhs.iterator;
          }
          diff_t operator-(const row_element_iterator &rhs) {
            return iterator - rhs.iterator;
          }
          row_element_iterator operator++() {
            iterator++;
            return *this;
          }
          row_element_iterator operator++(int) {
            auto tmp = *this;
            iterator++;
            return tmp;
          }
          T operator*() { return *iterator; }
        };

        class row_iterator
            : public std::iterator<std::random_access_iterator_tag,
                                   row_element_iterator> {
          typename array::iterator iterator;
          friend MatrixArray<T, m, n>;
          row_iterator(const typename array::iterator &i) : iterator(i) {}

        public:
          row_iterator(const row_iterator &i) : iterator(i.iterator) {}
          ~row_iterator() {}

          bool operator==(const row_iterator &rhs) {
            return iterator == rhs.iterator;
          }
          bool operator!=(const row_iterator &rhs) {
            return iterator != rhs.iterator;
          }
          row_iterator operator++() {
            iterator += n;
            return *this;
          }
          row_iterator operator++(int) {
            auto tmp = *this;
            iterator += n;
            return tmp;
          }
          row_iterator operator*() { return *this; }

          row_element_iterator begin() {
            return row_element_iterator(iterator);
          }
          row_element_iterator end() {
            return row_element_iterator(iterator + n);
          }
        };

        class column_element_iterator
            : public std::iterator<std::random_access_iterator_tag, T> {
          typename array::iterator iterator;
          friend MatrixArray<T, m, n>;
          friend class column_iterator;
          column_element_iterator(const typename array::iterator &i)
              : iterator(i) {}

        public:
          column_element_iterator(const row_element_iterator &i)
              : iterator(i.iterator) {}
          ~column_element_iterator() {}
          bool operator==(const column_element_iterator &rhs) {
            return iterator == rhs.iterator;
          }
          bool operator!=(const column_element_iterator &rhs) {
            return iterator != rhs.iterator;
          }
          column_element_iterator operator++() {
            iterator += n;
            return *this;
          }
          column_element_iterator operator++(int) {
            auto tmp = *this;
            iterator += n;
            return tmp;
          }
          T operator*() { return *iterator; }
        };

        class column_iterator
            : public std::iterator<std::random_access_iterator_tag,
                                   column_element_iterator> {
          typename array::iterator iterator;
          friend MatrixArray<T, m, n>;
          column_iterator(const typename array::iterator &i) : iterator(i) {}

        public:
          column_iterator(const row_iterator &i) : iterator(i.iterator) {}
          ~column_iterator() {}

          bool operator==(const column_iterator &rhs) {
            return iterator == rhs.iterator;
          }
          bool operator!=(const column_iterator &rhs) {
            return iterator != rhs.iterator;
          }
          column_iterator operator++() {
            iterator++;
            return *this;
          }
          column_iterator operator++(int) {
            auto tmp = *this;
            iterator++;
            return tmp;
          }
          column_iterator operator*() { return *this; }

          column_element_iterator begin() {
            return column_element_iterator(iterator);
          }
          column_element_iterator end() {
            return column_element_iterator(iterator + m * n);
          }
        };

      private:
        row_iterator row_begin() { return row_iterator(storage.begin()); }
        row_iterator row_end() { return row_iterator(storage.end()); }
        column_iterator column_begin() {
          return column_iterator(storage.begin());
        }
        column_iterator column_end() {
          return column_iterator(storage.begin() + n);
        }

      public:
        class rows_t {
          MatrixArray &matrix;

        public:
          rows_t(MatrixArray &mat) : matrix(mat) {}
          row_iterator begin() { return matrix.row_begin(); }
          row_iterator end() { return matrix.row_end(); }
        };
        friend rows_t;
        rows_t rows() { return rows_t(*this); }

        class columns_t {
          MatrixArray &matrix;

        public:
          columns_t(MatrixArray &mat) : matrix(mat) {}
          column_iterator begin() { return matrix.column_begin(); }
          column_iterator end() { return matrix.column_end(); }
        };
        friend columns_t;
        columns_t columns() { return columns_t(*this); }

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
        ~MatrixArray() {}
      };

      template <typename T, size_t m, size_t n>
      std::ostream &operator<<(std::ostream &out, MatrixArray<T, m, n> matrix) {
        infix_ostream_iterator<std::string> lines(out, "}, {");
        out << "{{";
        std::transform(
            matrix.rows().begin(), matrix.rows().end(), lines,
            [&out](const typename MatrixArray<T, m, n>::row_iterator &row_c)
                -> std::string {
                  auto row = row_c;
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
