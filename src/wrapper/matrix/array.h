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

#include <array>
#include <iterator>
#include "wrapper/matrix/base.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T, int m, int n = m>
      class MatrixArray : public MatrixBase<T> {
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
          row_element_iterator(const row_element_iterator &i)
              : iterator(i.iterator) {}

        public:
          ~row_element_iterator() {}
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

          row_element_iterator begin() {
            return row_element_iterator(iterator);
          }
          row_element_iterator end() {
            return row_element_iterator(iterator + n);
          }
        };

        template <typename... E>
        MatrixArray(E &&... e)
            : storage{{std::forward<E>(e)...}} {}
        ~MatrixArray() {}

        row_iterator row_begin() { return row_iterator(storage.begin()); }
        row_iterator row_end() { return row_iterator(storage.end()); }
      };
    }
  }
}
