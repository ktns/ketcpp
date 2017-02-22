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
#include "wrapper/matrix/dummy.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

go_bandit([] {
  describe("Matrix", [] {
    Matrix<float> matrix1 = make_dummy_matrix<float>(), matrix2 = matrix1,
                  matrix3 = matrix1;
    before_each([&matrix1, &matrix2, &matrix3] {
      matrix1 = make_matrix<float, 3, 2>({{1, 2}, {3, 4}, {5, 6}});
      matrix2 = make_matrix<float>({{2, 4}, {6, 8}, {10, 12}});
      matrix3 = matrix1 * 3;
    });

    it("Should not be abstract class",
       [] { std::is_abstract<Matrix<float>>::value must be_falsy; });

    describe(".get_num_rows", [&matrix1] {
      it("should return the correct number of rows",
         [&matrix1] { matrix1->get_num_rows() must equal(3u); });
    });
    describe(".get_num_columns", [&matrix1] {
      it("should return the correct number of columns",
         [&matrix1] { matrix1->get_num_columns() must equal(2u); });
    });
    describe(".get_row_size", [&matrix1] {
      it("should return the correct size of rows",
         [&matrix1] { matrix1->get_row_size() must equal(2u); });
    });
    describe(".get_column_size", [&matrix1] {
      it("should return the correct size of columns",
         [&matrix1] { matrix1->get_column_size() must equal(3u); });
    });

    describe("::operator==", [&matrix1, &matrix2] {
      it("should return true for same matrix", [&matrix1] {
        auto matrix2 = matrix1;
        (matrix1 == matrix2) must be_truthy;
      });

      it("should false true for different matrix", [&matrix1, &matrix2] {
        matrix1 must_not equal(matrix2);
        (matrix1 == matrix2) must be_falsy;
      });
    });

    describe("::operator+=", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix1;

        matrix4 must equal(matrix1);
        matrix4 must_not equal(matrix3);
        matrix4 += matrix2;
        matrix4 must_not equal(matrix1);
        matrix4 must equal(matrix3);
      });
    });

    describe("::operator-=", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix3;

        matrix4 must_not equal(matrix1);
        matrix4 must equal(matrix3);
        matrix4 -= matrix2;
        matrix4 must equal(matrix1);
        matrix4 must_not equal(matrix3);
      });
    });

    describe("::operator*=", [&matrix1, &matrix2] {
      describe("(float)", [&matrix1, &matrix2] {
        it("should change elements", [&matrix1, &matrix2] {
          auto matrix3 = matrix1;
          matrix3 must equal(matrix1);
          matrix3 must_not equal(matrix2);
          matrix3 *= 2.0f;
          matrix3 must_not equal(matrix1);
          matrix3 must equal(matrix2);
        });
      });
      describe("(unsinged int)", [&matrix1, &matrix2] {
        it("should change elements", [&matrix1, &matrix2] {
          auto matrix3 = matrix1;
          matrix3 must equal(matrix1);
          matrix3 must_not equal(matrix2);
          matrix3 *= 2u;
          matrix3 must_not equal(matrix1);
          matrix3 must equal(matrix2);
        });
      });
    });

    describe("::operator+", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix1 + matrix2;
        matrix4 must equal(matrix3);
        matrix1 must_not equal(matrix3);
      });
    });

    describe("::operator-", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix3 - matrix1;
        matrix4 must equal(matrix2);
        matrix3 must_not equal(matrix2);
      });
    });

    describe("::operator*", [&matrix1, &matrix2] {
      describe("(float)", [&matrix1, &matrix2] {
        it("should return a multiplied matrix", [&matrix1, &matrix2] {
          auto matrix3 = matrix1 * 2.f;
          matrix3 must equal(matrix2);
        });
      });
      describe("(unsinged int)", [&matrix1, &matrix2] {
        it("should return a multiplied matrix", [&matrix1, &matrix2] {
          auto matrix3 = matrix1 * 2u;
          matrix3 must equal(matrix2);
        });
      });
    });
    it("should be able to be compared as matrix", [&matrix1, &matrix2] {
      AssertThat(matrix1, Is().EqualToContainer(matrix1));
      AssertThat(matrix1, Is().Not().EqualToContainer(matrix2));
    });
  });
});
