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

#include "config/ketcpp_config.h"

#ifdef LIBINT2_FOUND

#include <memory>
#include <libint2.hpp>
#include <bandit/bandit.h>
#include "orbital/basisset/libint.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::orbital::basisset;

go_bandit([] {
  describe("orbital::basisset::Libint2BasisSet", [] {
    it("ctor must initialize libint2", [] {
      libint2::initialized() must be_falsy;
      auto basisset = std::make_unique<Libint2BasisSet>();
      libint2::initialized() must be_truthy;
      basisset.reset();
      libint2::initialized() must be_falsy;
    });
  });
});

#endif
