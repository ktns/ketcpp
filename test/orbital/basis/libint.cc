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
        auto correct = wrapper::matrix::make_symmetric_matrix<double>(
            {{0.10000000e+01},
             {0.23670392e+00, 0.10000000e+01},
             {0.00000000e+00, 0.00000000e+00, 0.10000000e+01},
             {0.00000000e+00, 0.00000000e+00, 0.00000000e+00, 0.10000000e+01},
             {0.00000000e+00, 0.00000000e+00, 0.00000000e+00, 0.00000000e+00,
              0.10000000e+01},
             {0.52485534e-01, 0.46709281e+00, 0.38887100e+00, 0.14311115e-01,
              -0.11964767e-01, 0.10000000e+01},
             {0.52485684e-01, 0.46709359e+00, -0.11203435e+00, 0.28606226e+00,
              -0.23913519e+00, 0.24703413e+00, 0.10000000e+01}});
        auto within_delta = [](double a, double b) -> bool {
          return std::abs(a - b) < 1e-7;
        };
        AssertThat(overlap, EqualsContainer(correct, within_delta));
      });
    });

    describe("::get_kinetic", [&] {
      it("Should return correct kinetic matrix", [&] {
        auto kinetic = basis->get_kinetic();
        auto correct = wrapper::matrix::make_symmetric_matrix<double>(
            {{+2.9003204e+01},
             {-1.6801096e-01, +8.0812790e-01},
             {+0.0000000e+00, +0.0000000e+00, +2.5287312e+00},
             {+0.0000000e+00, +0.0000000e+00, +0.0000000e+00, +2.5287312e+00},
             {+0.0000000e+00, +0.0000000e+00, +0.0000000e+00, +0.0000000e+00,
              +2.5287312e+00},
             {-3.3098365e-03, +1.2301215e-01, +2.7495169e-01, +1.0118690e-02,
              -8.4597024e-03, +7.6003188e-01},
             {-3.3097585e-03, +1.2301271e-01, -7.9214194e-02, +2.0226109e-01,
              -1.6908118e-01, +7.4497840e-03, +7.6003188e-01}});
        auto within_delta = [](double a, double b) -> bool {
          return std::abs(a - b) < 1e-5;
        };
        AssertThat(kinetic, EqualsContainer(correct, within_delta));
      });
    });
  });
});

#endif
