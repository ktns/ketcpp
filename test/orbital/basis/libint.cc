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
#include "config/fixture.h"
#include "orbital/basis/libint.h"
#include "wrapper/matrix/default.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::orbital::basis;

go_bandit([] {
  describe("orbital::basisset::Libint2Basis", [] {
    std::unique_ptr<Libint2Basis> basis;
    before_each([&] {
      basis = std::make_unique<Libint2Basis>(FIXTURE_PATH_H2O_XYZ, "STO-3G");
    });

    after_each([&] { basis.reset(); });

    it("Should be constructed by a molecule file name and a basisset name",
       [&] { basis must be_truthy; });

    it("ctor/dtor should initialize/finalize libint2", [&] {
      libint2::initialized() must be_truthy;
      basis.reset();
      libint2::initialized() must be_falsy;
      basis = std::make_unique<Libint2Basis>(FIXTURE_PATH_H2O_XYZ, "STO-3G");
      libint2::initialized() must be_truthy;
    });

    describe("::get_overlap", [&] {
      it("Should return correct overlap matrix", [&] {
        auto overlap = basis->get_overlap();
        decltype(overlap) correct =
            wrapper::matrix::make_matrix<double>({{0.0}});
        overlap must equal(correct);
      });

    });
  });
});

#endif
