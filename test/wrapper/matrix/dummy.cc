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

#include "wrapper/matrix/dummy.h"
#include "wrapper/matrix/default.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

go_bandit([] {
  describe("MatrixDummy", [] {
    it("should be default constructible",
       [] { [] { MatrixDummy<float> dummy; } must_not throw_exception; });

    describe(".get_num_rows", [] {
      it("should throw logic_error", [] {
        MatrixDummy<float> dummy;
        AssertThrows(std::logic_error, [&] {
          volatile auto v[[gnu::unused]] = dummy.get_num_rows();
        }());
      });
    });
    describe(".get_num_columns", [] {
      it("should throw logic_error", [] {
        MatrixDummy<float> dummy;
        AssertThrows(std::logic_error, [&] {
          volatile auto v[[gnu::unused]] = dummy.get_num_columns();
        }());
      });
    });
    describe(".get_row_size", [] {
      it("should throw logic_error", [] {
        MatrixDummy<float> dummy;
        AssertThrows(std::logic_error, [&] {
          volatile auto v[[gnu::unused]] = dummy.get_row_size();
        }());
      });
    });
    describe(".get_column_size", [] {
      it("should throw logic_error", [] {
        MatrixDummy<float> dummy;
        AssertThrows(std::logic_error, [&] {
          volatile auto v[[gnu::unused]] = dummy.get_column_size();
        }());
      });
    });

    describe("::operator+=", [] {
      it("should throw logic_error", [] {
        MatrixDummy<float> dummy1, dummy2;
        AssertThrows(std::logic_error, [&] { dummy1 += dummy2; }());
      });
    });

    describe("::operator+", [] {
      it("should throw logic_error", [] {
        MatrixDummy<float> dummy1, dummy2;
        AssertThrows(std::logic_error, [&] {
          volatile auto v[[gnu::unused]] = dummy1 + dummy2;
        }());
      });
    });

    describe("::operator*=", [] {
      describe("(float)", [] {
        it("should throw logic_error", [] {
          MatrixDummy<float> dummy;
          AssertThrows(std::logic_error, [&] { dummy *= 2.0f; }());
        });
      });
    });
    describe("::operator*", [] {
      describe("(float)", [] {
        it("should throw logic_error", [] {
          MatrixDummy<float> dummy;
          AssertThrows(std::logic_error,
                       [&] { volatile auto v = dummy * 2.0f; }());
        });
      });
    });
  });
  describe("make_dummy_matrix", [] {
    it("should return a dummy matrix", [] {
      auto dummy = make_dummy_matrix<float>();
      AssertThrows(std::logic_error, [&] { dummy *= 0.0; }());
    });
    it("should return an assignable matrix", [] {
      auto dummy = make_dummy_matrix<float>();
      dummy = make_matrix<float, 1>({42});
      dummy->at(0, 0) must equal(42);
    });
  });
});
