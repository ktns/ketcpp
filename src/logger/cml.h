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
#include <type_traits>

#include "logger/base.h"

namespace ketcpp::test {
  struct CMLLoggerElementTester;
}

namespace ketcpp::logger {
  //! Output a log of a job in CML CompChem convention
  class CMLLogger : public Logger {
  private:
    friend ketcpp::test::CMLLoggerElementTester;
    struct element_t {
      std::string name;
      std::map<std::string, std::string> attributes;
      bool operator==(const element_t &other) const {
        return name == other.name && attributes == other.attributes;
      }
    };
    friend std::ostream &operator<<(std::ostream &os, const element_t &e) {
      os << '<' << e.name;
      for (const auto a : e.attributes) {
        os << ' ' << a.first << "=\"" << a.second << '"';
      }
      return os << "/>";
    }
#ifdef __cpp_lib_is_aggregate
    static_assert(std::is_aggregate_v<element_t>);
#endif

    static const element_t compchem_root;
    static const element_t joblist;

    std::ostream &ostr;
    std::stack<element_t> stack;

    CMLLogger &operator<<(const element_t &element);
    CMLLogger &push(const element_t &element);
    CMLLogger &pop();

  public:
    CMLLogger(std::ostream &ostr);
    CMLLogger(const CMLLogger &) = delete;
    CMLLogger(CMLLogger &&) = default;
    ~CMLLogger();

    void initialize_scf(const mol_t &, const basisset_t &, const scf_conf_t &) {
      throw std::logic_error("Not yet implemented");
    }
  };
}
