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

#pragma once

#include "logger/base.h"

namespace ketcpp::logger {

  Logger &get_null_logger();

  //! Null logger discarding all logs
  class NullLogger : public Logger {
  private:
    NullLogger(){};
    NullLogger(const NullLogger &) = delete;
    NullLogger(NullLogger &&) = delete;
    virtual ~NullLogger() {}

    void initialize_scf(const jobs::SCF::Configuration &) override {}

    static NullLogger the_null_logger;
    friend Logger & ::ketcpp::logger::get_null_logger();
  };

  inline Logger &get_null_logger() { return NullLogger::the_null_logger; }
}
