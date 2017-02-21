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

go_bandit([] {
  describe("MatrixArray", [] {
    it("should not be abstract class", [] {
      std::is_abstract<MatrixArray<float, 3, 3>>::value must_not be_truthy;
    });

    it("should be initialized with list", [] {
      [] {
        MatrixArray<float, 2, 2> array = {1.f, 2.f, 3.f, 4.f};
        MatrixArray<float, 2, 2> array2 = {{1.f, 2.f}, {3.f, 4.f}};
      } must_not throw_exception;
    });

    it("should be correctly initialized with nested list", [] {
      MatrixArray<float, 2, 2> array = {1.f, 2.f, 3.f, 4.f};
      MatrixArray<float, 2, 2> array2 = {{1.f, 2.f}, {3.f, 4.f}};
      array must equal(array2);
    });

    MatrixArray<float, 3, 2> array = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
    auto array2 = array;
    array2 *= 2;
    auto array3 = array;
    array3 *= 3;

    describe(".get_num_rows", [&array] {
      it("should return the correct number of rows",
         [&array] { array.get_num_rows() must equal(3u); });
    });
    describe(".get_num_columns", [&array] {
      it("should return the correct number of columns",
         [&array] { array.get_num_columns() must equal(2u); });
    });
    describe(".get_row_size", [&array] {
      it("should return the correct size of rows",
         [&array] { array.get_row_size() must equal(2u); });
    });
    describe(".get_column_size", [&array] {
      it("should return the correct size of columns",
         [&array] { array.get_column_size() must equal(3u); });
    });

    describe("::operator==", [&array, &array2] {
      it("should return true for same matrix", [&array] {
        auto array2 = array;
        (array == array2) must be_truthy;
      });

      it("should false true for different matrix",
         [&array, &array2] { (array == array2) must be_falsy; });
    });

    describe("::operator+=", [&array, &array2, &array3] {
      it("should change elements", [&array, &array2, &array3] {
        auto array4 = array;

        array must equal(array4);
        array4 += array2;
        array4 must equal(array3);
      });
    });

    describe("::operator*=", [&array, &array2] {
      describe("(float)", [&array, &array2] {
        it("should change elements", [&array, &array2] {
          auto array3 = array;
          array3 must equal(array);
          array3 *= 2.0f;
          array3 must equal(array2);
        });
      });
      describe("(unsinged int)", [&array, &array2] {
        it("should change elements", [&array, &array2] {
          auto array3 = array;
          array3 must equal(array);
          array3 *= 2u;
          array3 must equal(array2);
        });
      });
    });
    describe("::operator*", [&array, &array2] {
      describe("(float)", [&array, &array2] {
        it("should return a multiplied matrix", [&array, &array2] {
          auto array3 = array * 2.f;
          array3 must equal(array2);
        });
      });
      describe("(unsinged int)", [&array, &array2] {
        it("should return a multiplied matrix", [&array, &array2] {
          auto array3 = array * 2u;
          array3 must equal(array2);
        });
      });
    });
  });
});
