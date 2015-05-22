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

go_bandit([]() {
  describe("MatrixArray", []() {
    it("should not be abstract class", []() {
      std::is_abstract<MatrixArray<float, 3, 3>>::value must_not be_truthy;
    });

    it("should be initialized with list", []() {
      []() {
        MatrixArray<float, 2, 2> array = {1.f, 2.f, 3.f, 4.f};
        MatrixArray<float, 2, 2> array2 = {{1.f, 2.f}, {3.f, 4.f}};
      } must_not throw_exception;
    });
    MatrixArray<float, 3, 2> array = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
    describe(".rows", [&array]() {

      it("should iterate as many times as number of rows", [&array]() {
        size_t n = 0;
        for (auto i : array.rows()) {
          ++n must be_lte(3);
        }
        n must equal(3);
      });

      it("should all row vectors", [&array]() {
        float v[3][2] = {{1.f, 2.f}, {3.f, 4.f}, {5.f, 6.f}};
        size_t k = 0;
        for (auto i : array.rows()) {
          std::equal(i.begin(), i.end(), &v[k++][0]) must be_truthy;
        }
      });
    });

    describe(".columns", [&array]() {
      it("should iterate as many times as number of columns", [&array]() {
        size_t n = 0;
        for (auto i : array.columns()) {
          ++n must be_lte(2);
        }
        n must equal(2);
      });

      it("should all row vectors", [&array]() {
        float v[2][3] = {{1.f, 3.f, 5.f}, {2.f, 4.f, 6.f}};
        size_t k = 0;
        for (auto i : array.columns()) {
          std::equal(i.begin(), i.end(), &v[k++][0]) must be_truthy;
        }
      });
    });
  });
});
