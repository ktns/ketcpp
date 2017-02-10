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

#include "wrapper/matrix/default.h"
using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

go_bandit([] {
  describe("make_zero_matrix", [] {
    it("should return a matrix whose elements are all zero", [] {
      auto mat = make_zero_matrix<double>(2, 3);
      mat->for_each(
          [&mat](size_t i, size_t j) { mat->at(i, j) must equal(0); });
    });
  });

  describe("make_symmetric_matrix", [] {
    it("should return a symmetric matrix", [] {
      auto mat = make_symmetric_matrix<double>({{0}, {1, 2}, {3, 4, 5}});
      mat->for_each([&mat](size_t i, size_t j) {
        mat->at(i, j) must equal(mat->at(j, i));
      });
    });
  });
});
