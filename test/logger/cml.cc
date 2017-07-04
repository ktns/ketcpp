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
    std::stringstream ss;
    std::unique_ptr<CMLLogger> logger;

    before_each([&] { logger = std::make_unique<CMLLogger>(ss); });

    after_each([&] {
      logger.reset();
      ss.str("");
    });

    it("should not be copy constructible",
       [] { std::is_copy_constructible_v<CMLLogger> must be_falsy; });

    it("should be move constructible",
       [] { std::is_move_constructible_v<CMLLogger> must be_truthy; });

    it("should emit closing tags while destruction", [&] {
      const std::string closing_tag = "</module>";
      const auto n = closing_tag.length();
      ss.seekg(-n, std::ios_base::end);
      std::string end1;
      end1.resize(n);
      ss.read(&end1[0], n);
      AssertThat(end1, Is().Not().EqualTo(closing_tag));

      logger.reset();
      ss.seekg(-n, std::ios_base::end);
      std::string end2;
      end2.resize(n);
      ss.read(&end2[0], n);
      AssertThat(end2, Is().EqualTo(closing_tag));
    });

    describe("::initialize_scf()", [&] {
      it("should output compchem initialize module", [&] {
        ss.str("");
        jobs::SCF::Configuration config = {};
        logger->initialize_scf(config);
        ss.flush();
        const auto log = ss.str();

      });
    });
  });
});
