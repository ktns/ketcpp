/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2017 Katsuhiko Nishimra
 *
 * This file is part of ketcpp.
 *
 * ketcpp is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3.0 of the License, or any later version.
 *
 * ketcpp is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ketcpp.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>

#include <bandit/bandit.h>

#include "wrapper/matrix.h"
#include "wrapper/matrix/inverse.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;
using namespace ketcpp::wrapper::matrix::inverse;

go_bandit([] {
  describe("InverseMatrixFactory", [] {
    it("should be able to invert a matrix", [] {
      auto within_delta = [](float delta) {
        return [delta](float a, float b) { return std::abs(a - b) < delta; };
      };
      auto matrix =
          make_matrix<float, 3, 3>({{2, 1, 5}, {3, -4, 3}, {-2, -3, -5}});
      InverseMatrixFactory<float> inverter(matrix);
      AssertThat(
          inverter.inverse() * matrix,
          EqualsContainer(make_unit_matrix<float>(3), within_delta(1e-6f)));
    });
  });
});
