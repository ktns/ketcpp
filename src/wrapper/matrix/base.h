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
#include <functional>
#include <memory>
#if __has_include(<optional>)
#include <optional>
template <typename T> namespace { using optional = std::optional<T>; };
#else
#include <experimental/optional>
namespace {
  template <typename T>
  struct optional : public std::experimental::optional<T> {
    using std::experimental::optional<T>::optional;
    bool has_value() { return static_cast<bool>(*this); }
  };
};
#endif
#include <sstream>

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T> class MatrixBase {
      public:
        virtual size_t get_num_rows() const = 0;
        virtual size_t get_num_columns() const = 0;
        virtual size_t get_row_size() const = 0;
        virtual size_t get_column_size() const = 0;
        virtual T &at(size_t irow, size_t icol) = 0;
        virtual T at(size_t irow, size_t icol) const = 0;

        void for_each(std::function<void(size_t, size_t)> lmd, //
                      size_t nr = 0, size_t nc = 0) const {
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
        template <typename R>
        optional<R> for_each(std::function<optional<R>(size_t, size_t)> lmd,
                             size_t nr = 0, size_t nc = 0) const {
          if (nr == 0)
            nr = this->get_num_rows();
          if (nc == 0)
            nc = this->get_num_columns();
          for (size_t i = 0; i < nr; i++) {
            for (size_t j = 0; j < nc; j++) {
              auto ret = lmd(i, j);
              if (ret.has_value())
                return ret;
            }
          }
          return {};
        }

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
          for_each([this, &rhs](size_t i, size_t j) {
            this->at(i, j) += rhs.at(i, j);
          });
          return *this;
        }
        MatrixBase &operator+=(const std::unique_ptr<MatrixBase> &rhs) {
          return *this += *rhs;
        }
        virtual MatrixBase &operator*=(T rhs) = 0;

        virtual bool operator!=(const MatrixBase &rhs) const {
          optional<bool> ret = for_each(
              static_cast<std::function<optional<bool>(size_t, size_t)>>(
                  [this, &rhs](size_t i, size_t j) -> optional<bool> {
                    if (this->at(i, j) != rhs.at(i, j))
                      return true; // abort loop and return true
                    return {};     // resume loop
                  }));

          return ret.value_or(
              false); // ret has value when the loop has finished
        }
        virtual bool operator==(const MatrixBase &rhs) const {
          return !((*this) != rhs);
        }

        virtual std::unique_ptr<MatrixBase> copy() const = 0;

        virtual ~MatrixBase(){};
      };

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
  }
}
