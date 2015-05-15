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
#include "wrapper/matrix/base.h"

namespace ketcpp {
  namespace wrapper {
    namespace matrix {
      template <typename T, int m, int n = m>
      class MatrixArray : public MatrixBase<T> {
        using array = std::array<T, m * n>;
        array storage;

      public:
        template <typename... E>
        MatrixArray(E &&... e)
            : storage{{std::forward<E>(e)...}} {}
        ~MatrixArray() {}
      };
    }
  }
}
