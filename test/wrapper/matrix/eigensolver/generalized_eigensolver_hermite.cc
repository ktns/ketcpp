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
#include "wrapper/matrix/dummy.h"
#include "wrapper/matrix/eigensolver/generalized_eigensolver_hermite.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;
using namespace ketcpp::wrapper::matrix::eigensolver;

typedef GeneralizedEigensolverHermite<float> GEH;
go_bandit([] {
  describe("GeneralizedEigensolverHermite<float>", [] {
    auto a = make_dummy_matrix<float>(), b = a;

    before_each([&] {
      a = make_symmetric_matrix<float>({{2}, {-2, -4}});
      b = make_symmetric_matrix<float>({{2}, {2, 4}});
    });

    describe("ctor", [&] {
#ifdef __cpp_deduction_guides
      it("should deduce type properly", [&] {
        GeneralizedEigensolverHermite solver(a, b);
        std::is_same<decltype(solver), GEH>::value must be_truthy;
      });
#else
      xit("should deduce scalar type properly", [] {});
#endif
    });
    describe("solve()", [&] {
      it("should return correct eigenvalues", [&] {
        GEH solver(a, b);
        auto values = solver.solve().first,
             correct_values = make_matrix<float, 1, 2>({{-1}, {3}});
        auto within_delta = [](float a, float b) {
          return std::abs(a - b) < 1e-5;
        };

        AssertThat(values, EqualsContainer(correct_values, within_delta));
      });

      it("should return correct eigenvectors", [&] {
        GEH solver(a, b);
        auto vectors = solver.solve().second,
             correct_vectors = make_matrix<float, 2, 2>({{0, 1}, {0.5, -0.5}});
        auto within_delta = [](float a, float b) {
          return std::abs(a - b) < 1e-5;
        };

        AssertThat(vectors, EqualsContainer(correct_vectors, within_delta));
      });

      it("should return solution for the generalized eigenproblem", [&a, &b] {
        GEH solver(a, b);
#ifdef __cpp_structured_bindings
        auto[values, vectors] = solver.solve();
#else
        auto pair = solver.solve();
        auto values = pair.first;
        auto vectors = pair.last;
#endif
        auto diag = make_diagonal_matrix(values.cbegin(), values.cend());
        auto with_delta = [](float a, float b) {
          return std::abs(a - b) < 1e-6;
        };
        AssertThat(a * vectors,
                   EqualsContainer(b * vectors * diag, with_delta));
      });
    });
  });
});
