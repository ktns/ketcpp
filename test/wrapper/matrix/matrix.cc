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
#include "wrapper/matrix/matrix.h"
#include "wrapper/matrix/default.h"
using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

go_bandit([] {
  describe("Matrix", [] {
    it("Should not be abstract class",
       [] { std::is_abstract<Matrix<float>>::value must be_falsy; });

    Matrix<float> matrix = make_matrix<float, 3, 2>({{1, 2}, {3, 4}, {5, 6}});
    Matrix<float> matrix2 = make_matrix<float>({{2, 4}, {6, 8}, {10, 12}});
    Matrix<float> matrix3 = matrix * 3;
    Matrix<float> matrix4 = matrix2 * 2;

    describe(".get_num_rows", [&matrix] {
      it("should return the correct number of rows",
         [&matrix] { matrix->get_num_rows() must equal(3); });
    });
    describe(".get_num_columns", [&matrix] {
      it("should return the correct number of columns",
         [&matrix] { matrix->get_num_columns() must equal(2); });
    });
    describe(".get_row_size", [&matrix] {
      it("should return the correct size of rows",
         [&matrix] { matrix->get_row_size() must equal(2); });
    });
    describe(".get_column_size", [&matrix] {
      it("should return the correct size of columns",
         [&matrix] { matrix->get_column_size() must equal(3); });
    });

    describe("::operator==", [&matrix, &matrix2] {
      it("should return true for same matrix", [&matrix] {
        auto matrix2 = matrix;
        (matrix == matrix2) must be_truthy;
      });

      it("should false true for different matrix", [&matrix, &matrix2] {
        matrix must_not equal(matrix2);
        (matrix == matrix2) must be_falsy;
      });
    });

    describe("::operator+=", [&matrix, &matrix2, &matrix3] {
      it("should change elements", [&matrix, &matrix2, &matrix3] {
        auto matrix4 = matrix;

        matrix4 must equal(matrix);
        matrix4 must_not equal(matrix3);
        matrix4 += matrix2;
        matrix4 must_not equal(matrix);
        matrix4 must equal(matrix3);
      });
    });

    describe("::operator*=", [&matrix, &matrix2] {
      describe("(float)", [&matrix, &matrix2] {
        it("should change elements", [&matrix, &matrix2] {
          auto matrix3 = matrix;
          matrix3 must equal(matrix);
          matrix3 must_not equal(matrix2);
          matrix3 *= 2.0f;
          matrix3 must_not equal(matrix);
          matrix3 must equal(matrix2);
        });
      });
      describe("(unsinged int)", [&matrix, &matrix2] {
        it("should change elements", [&matrix, &matrix2] {
          auto matrix3 = matrix;
          matrix3 must equal(matrix);
          matrix3 must_not equal(matrix2);
          matrix3 *= 2u;
          matrix3 must_not equal(matrix);
          matrix3 must equal(matrix2);
        });
      });
    });
    describe("::operator*", [&matrix, &matrix2] {
      describe("(float)", [&matrix, &matrix2] {
        it("should return a multiplied matrix", [&matrix, &matrix2] {
          auto matrix3 = matrix * 2.f;
          matrix3 must equal(matrix2);
        });
      });
      describe("(unsinged int)", [&matrix, &matrix2] {
        it("should return a multiplied matrix", [&matrix, &matrix2] {
          auto matrix3 = matrix * 2u;
          matrix3 must equal(matrix2);
        });
      });
    });
  });
});
