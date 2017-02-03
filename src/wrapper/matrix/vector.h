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

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

#include "wrapper/matrix/base.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T> class MatrixVector : public MatrixBase<T> {
      private:
        const size_t num_rows;
        const size_t num_columns;
        const size_t &row_size = num_columns;
        const size_t &column_size = num_rows;

      public:
        size_t get_num_rows() const { return num_rows; }
        size_t get_num_columns() const { return num_columns; }
        size_t get_row_size() const { return row_size; }
        size_t get_column_size() const { return column_size; }

      private:
        using vector = std::vector<T>;
        vector storage;
        using Base = MatrixBase<T>;

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
        MatrixVector(const std::initializer_list<std::initializer_list<T>> list)
            : num_rows(list.size()), num_columns(max_size(list)),
              storage(num_rows * num_columns) {
          auto dest = storage.begin();
          for (auto i : list) {
            std::copy(i.begin(), i.end(), dest);
            dest += num_columns;
          }
        }
        MatrixVector(size_t m, size_t n) : num_rows(m), num_columns(n) {}
        MatrixVector() = delete;

				T& at(size_t i, size_t j){
					return storage[i*num_columns+j];
				}

				T at(size_t i, size_t j) const{
					return storage[i*num_columns+j];
				}

				/* FIXME
        bool operator==(const MatrixVector &rhs) const {
          return std::equal(this->storage.cbegin(), this->storage.cend(),
                            rhs.storage.cbegin());
        }
				*/
        Base &operator+=(const Base &rhsbase) {
          try {
            auto &rhs = dynamic_cast<const MatrixVector &>(rhsbase);
            std::transform(this->storage.cbegin(), this->storage.cend(),
                           rhs.storage.cbegin(), this->storage.begin(),
                           [](T l, T r) -> T { return l + r; });
            return *this;
          } catch (std::bad_cast &ex) {
            return Base::operator+=(rhsbase);
          }
        }

        Base &operator*=(T rhs) {
          std::transform(this->storage.cbegin(), this->storage.cend(),
                         this->storage.begin(),
                         [rhs](T l) -> T { return l * rhs; });
          return *this;
        }
        std::unique_ptr<MatrixBase<T>> operator*(T rhs) {
          auto new_ptr = std::move(this->copy());
          auto &new_matrix = dynamic_cast<decltype(*this) &>(*new_ptr);
          new_matrix *= rhs;
          return std::move(new_ptr);
        }
				/* FIXME
        MatrixVector<T> operator*(const MatrixVector<T> &rhs) const {
          MatrixVector<T> buf(this->num_rows, rhs.num_columns);
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
				*/

        std::unique_ptr<MatrixBase<T>> copy() const {
          std::unique_ptr<MatrixBase<T>> copy;
          copy.reset(new MatrixVector(*this));
          return std::move(copy);
        }

        ~MatrixVector() {}
      };
    }
  }
}
