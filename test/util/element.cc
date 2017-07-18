/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2017 Katsuhiko Nishimra
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

#include "util/element.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::util;

go_bandit([] {
  describe("Z_to_element_symbol", [] {
    describe("(0)", [] {
      it("should throw an exception", [] {
        AssertThrows(std::out_of_range, [] { Z_to_element_symbol(0); }());
      });
    });
    // static_assert(Z_to_element_symbol(0), "This should fail");

    describe("(1)", [] {
      it("should be \"H\"", [] {
        const std::string e = Z_to_element_symbol(1);
        e must equal("H");
      });
    });
    static_assert(Z_to_element_symbol(1)[0] == 'H');

    describe("(6)", [] {
      it("should be \"C\"", [] {
        const std::string e = Z_to_element_symbol(6);
        e must equal("C");
      });
    });
    static_assert(Z_to_element_symbol(6)[0] == 'C');

    describe("(8)", [] {
      it("should be \"O\"", [] {
        const std::string e = Z_to_element_symbol(8);
        e must equal("O");
      });
    });
    static_assert(Z_to_element_symbol(8)[0] == 'O');

    describe("(44)", [] {
      it("should be \"Ru\"", [] {
        const std::string e = Z_to_element_symbol(44);
        e must equal("Ru");
      });
    });

    describe("(118)", [] {
      it("should be \"Og\"", [] {
        const std::string e = Z_to_element_symbol(118);
        e must equal("Og");
      });
    });

    describe("(119)", [] {
      it("should throw an exception", [] {
        AssertThrows(std::out_of_range, [] { Z_to_element_symbol(119); }());
      });
    });
  });
});
