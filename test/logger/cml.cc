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

#include "logger/cml.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::logger;

go_bandit([] {
  describe("CMLLogger", [] {
    it("should be constructed and destructed without error", [] {
      std::stringstream ss;
      auto logger = new CMLLogger(ss);
      delete logger;
    });
  });
});
