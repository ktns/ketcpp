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

#include <bandit/bandit.h>
#include "wrapper/matrix/vector.h"
using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

go_bandit([] {
  describe("MatrixVector", [] {
    it("should not be abstract class",
       [] { std::is_abstract<MatrixVector<float>>::value must_not be_truthy; });

    it("should be initialized with list", [] {
      [] {
        MatrixVector<float> vector = {{1.f, 2.f}, {3.f, 4.f}};
      } must_not throw_exception;
    });

    it("should be correctly initialized with nested list", [] {
      MatrixVector<float> vector = {{1.f}, {3.f, 4.f}};
      MatrixVector<float> vector2 = {{1.f, 0.f}, {3.f, 4.f}};
      vector must equal(vector2);
    });

    MatrixVector<float> vector = {{1.f, 2.f}, {3.f, 4.f}, {5.f, 6.f}};
    auto vector2 = vector;
    vector2 *= 2;
    auto vector3 = vector;
    vector3 *= 3;

    describe("::operator+=", [&vector, &vector2, &vector3] {
      it("should change elements", [&vector, &vector2, &vector3] {
        auto vector4 = vector;

        vector must equal(vector4);
        vector4 += vector2;
        vector4 must equal(vector3);
      });
    });

    describe("::operator*=", [&vector, &vector2] {
      describe("(float)", [&vector, &vector2] {
        it("should change elements", [&vector, &vector2] {
          auto vector3 = vector;
          vector3 must equal(vector);
          vector3 *= 2.0f;
          vector3 must equal(vector2);
        });
      });
      describe("(unsinged int)", [&vector, &vector2] {
        it("should change elements", [&vector, &vector2] {
          auto vector3 = vector;
          vector3 must equal(vector);
          vector3 *= 2u;
          vector3 must equal(vector2);
        });
      });
    });
    describe("::operator*", [&vector, &vector2] {
      describe("(float)", [&vector, &vector2] {
        it("should return a multiplied matrix", [&vector, &vector2] {
          auto vector3 = vector * 2.f;
          vector3 must equal(vector2);
        });
      });
      describe("(unsinged int)", [&vector, &vector2] {
        it("should return a multiplied matrix", [&vector, &vector2] {
          auto vector3 = vector * 2u;
          vector3 must equal(vector2);
        });
      });
    });
  });
});
