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

#include <sstream>

#include <bandit/bandit.h>

#include "jobs/scf.h"
#include "logger/cml.h"
#include "orbital/basisset/fixture.h"
#include "wrapper/molecule/fixture.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::logger;
using namespace ketcpp::test;

namespace ketcpp::test {
  struct CMLLoggerElementTester {
    static auto should_be_comparable() {
      return [] {
        CMLLogger::element_t a = {"test",
                                  {{"test1", "test1"}, {"test2", "test2"}}},
                             b = {"test",
                                  {{"test2", "test2"}, {"test1", "test1"}}},
                             c = {"tset",
                                  {{"test1", "test1"}, {"test2", "test2"}}},
                             d = {"tset",
                                  {{"test1", "test1"}, {"test2", "test2"}}},
                             e = {"test", {{"test1", "test1"}}};
        a must equal(b);
        a must_not equal(c);
        a must_not equal(d);
        a must_not equal(e);
      };
    }
  };
}

go_bandit([] {
  describe("CMLLogger::element_t", [] {
    it("should be comparable", CMLLoggerElementTester::should_be_comparable());
  });
});
