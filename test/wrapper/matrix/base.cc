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

#include <bandit/bandit.h>

#include "wrapper/matrix/array.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

namespace {
  template <typename T, size_t m, size_t n = m>
  class MatrixTestBase : public MatrixArray<T, m, n> {
  public:
    using MatrixArray<T, m, n>::MatrixArray;
    MatrixBase<T> &operator*=(T rhs) { return MatrixBase<T>::operator*=(rhs); }
  };
}

go_bandit([] {
  describe("MatrixBase", [] {
#ifdef _LIBCPP_VERSION
    xit
#else
    it
#endif
        ("Should be abstract class",
         [] { std::is_abstract<MatrixBase<float>>::value must be_truthy; });

    describe("::operator+=(matrix)", [] {
      it("should subtract matrix from matrix", [] {
        MatrixTestBase<int, 2> matrix1{{1, 2}, {3, 4}};
        MatrixTestBase<int, 2> const matrix2{{2, 4}, {6, 8}};
        MatrixTestBase<int, 2> const matrix3{{3, 6}, {9, 12}};
        (matrix1 += matrix2) must equal(matrix3);
        matrix1 must equal(matrix3);
      });
    });
    describe("::operator-=(matrix)", [] {
      it("should subtract matrix from matrix", [] {
        MatrixTestBase<int, 2> matrix1{{1, 2}, {3, 4}};
        MatrixTestBase<int, 2> const matrix2{{2, 4}, {6, 8}};
        MatrixTestBase<int, 2> const matrix3{{-1, -2}, {-3, -4}};
        (matrix1 -= matrix2) must equal(matrix3);
        matrix1 must equal(matrix3);
      });
    });

    describe("::operator*=(scalar)", [] {
      it("should multiply all elements uniformly", [] {
        MatrixTestBase<int, 2> matrix1{{1, 2}, {3, 4}};
        MatrixTestBase<int, 2> const matrix2{{2, 4}, {6, 8}};
        (matrix1 *= 2) must equal(matrix2);
        matrix1 must equal(matrix2);
      });
    });

    describe("::operator*(scalar)", [] {
      it("should multiply all elements uniformly", [] {
        MatrixTestBase<int, 2> matrix1{{1, 2}, {3, 4}};
        MatrixTestBase<int, 2> const matrix2{{2, 4}, {6, 8}};
        auto matrix3 = matrix1 * 2;
        matrix3 must equal(matrix2);
        matrix1 must_not equal(matrix2);
      });
    });

    describe("::begin/end()", [] {
      it("should enumerate all elements of a matrix", [] {
        const MatrixTestBase<int, 2> matrix{{1, 2}, {3, 4}};
        const std::array<int, 4> array{{1, 2, 3, 4}};
        std::equal(matrix.begin(), matrix.end(), array.begin()) must be_truthy;
      });
      it("should return assignable iterators", [] {
        MatrixTestBase<int, 2> matrix;
        const std::array<int, 4> array{{42, 42, 42, 42}};
        std::fill(matrix.begin(), matrix.end(), 42);
        std::equal(matrix.begin(), matrix.end(), array.begin()) must be_truthy;
      });
    });

    it("should be able to be compared as a container", [] {
      const MatrixTestBase<int, 2> matrix{{1, 2}, {3, 4}};
      const std::array<int, 4> array{{1, 2, 3, 4}};
      AssertThat(matrix, EqualsContainer(array));
    });
  });
});
