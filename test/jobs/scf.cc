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

#include <type_traits>

#include <bandit/bandit.h>

#include "jobs/scf.h"

using namespace ketcpp;
using namespace ketcpp::jobs;
using namespace bandit;
using namespace bandit::Matchers;

go_bandit([] {
  describe("SCF", [] {
    it("should not be default constructible",
       [] { std::is_default_constructible_v<SCF> must be_falsy; });

    it("should not be copy constructible",
       [] { std::is_copy_constructible_v<SCF> must be_falsy; });

    it("should not be move constructible",
       [] { std::is_move_constructible_v<SCF> must be_falsy; });
  });

  describe("SCF::Configuration", [] {
    it("should not be default constructible", [] {
      std::is_default_constructible_v<SCF::Configuration> must be_falsy;
    });
  });
});
