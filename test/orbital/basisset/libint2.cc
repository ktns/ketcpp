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

#include "orbital/basisset/libint2.h"
#include "wrapper/molecule/fixture.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::orbital::basis;
using namespace ketcpp::orbital::basisset;

go_bandit([] {
  describe("Libint2BasisSet", [] {
    std::unique_ptr<Libint2BasisSet> pbs;
    before_each([&pbs] { pbs.reset(new Libint2BasisSet("STO-3G")); });

    it("Should not default constructible",
       [] { std::is_default_constructible_v<Libint2BasisSet> must be_falsy; });

    describe(".get_basis", [&pbs] {
      it("should return an instance of Libint2Basis", [&pbs] {
        wrapper::molecule::FixtureH2O mol;
        auto pb = pbs->get_basis(mol);
        pb.get() must_not be_null;
        dynamic_cast<Libint2Basis *>(pb.get()) must_not be_null;
      });
    });

    describe(".get_label", [&pbs] {
      it("should return the correct label", [&pbs] {
        pbs->get_label() must equal("STO-3G"); //
      });
    });
  });
});

#endif
