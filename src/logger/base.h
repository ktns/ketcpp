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

namespace ketcpp::jobs {
  struct SCFConfiguration;
}

namespace ketcpp::wrapper::molecule {
  class Base;
}

namespace ketcpp::logger {
  //! Base class for all loggers
  class Logger {
  protected:
    typedef jobs::SCFConfiguration scf_conf_t;
    typedef wrapper::molecule::Base mol_t;

    Logger(){};

  public:
    virtual ~Logger() {}

    //! Logs initialization of a SCF job
    virtual void initialize_scf(const mol_t &, const scf_conf_t &) = 0;
  };
}
