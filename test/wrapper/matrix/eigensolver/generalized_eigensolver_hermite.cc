/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2015 Katsuhiko Nishimra
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

#include <type_traits>

#include <bandit/bandit.h>

#include "wrapper/matrix/default.h"
#include "wrapper/matrix/eigensolver/generalized_eigensolver_hermite.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;
using namespace ketcpp::wrapper::matrix::eigensolver;

go_bandit([] {
  describe("GeneralizedEigensolverHermite<float>", [] {
    describe("ctor", [] {
#ifdef __cpp_deduction_guides
      it("should deduce type properly", [] {
        auto a = make_symmetric_matrix<float>({{1}, {2, 3}}),
             b = make_symmetric_matrix<float>({{1}, {2, 1}});
        GeneralizedEigensolverHermite solver(
            static_cast<MatrixBase<float> &>(a),
            static_cast<MatrixBase<float> &>(b));
        std::is_same<decltype(solver),
                     GeneralizedEigensolverHermite<float>>::value must
            be_truthy;
      });
#else
      xit("should deduce scalar type properly", [] {  });
#endif
    });
  });
});
