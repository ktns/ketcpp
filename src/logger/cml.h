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

#include <map>
#include <ostream>
#include <stack>

namespace ketcpp::logger {
  //! Output a log of a job in CML CompChem convention
  class CMLLogger {
  private:
    struct element {
      std::string name;
      std::map<std::string, std::string> attributes;
    };

    static const element compchem_root;
    static const element joblist;

    std::ostream &ostr;
    std::stack<element> stack;

    CMLLogger &operator<<(const element &element);
    CMLLogger &push(const element &element);
    CMLLogger &pop();

  public:
    CMLLogger(std::ostream &ostr);
    ~CMLLogger();
  };
}
