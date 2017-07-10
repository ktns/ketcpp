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

#include "logger/cml.h"

using namespace ketcpp::logger;

const CMLLogger::element_t CMLLogger::compchem_root =
    {"module",
     {{"convention", "convention:compchem"},
      {"xmlns", "http://www.xml-cml.org/schema"},
      {"xmlns:conventions", "http://www.xml-cml.org/convention/"},
      {"xmlns:compchem", "http://www.xml-cml.org/dictionary/compchem/"},
      {"xmlns:cc", "http://www.xml-cml.org/dictionary/compchem/"},
      {"xmlns:cml", "http://www.xml-cml.org/schema"},
      {"xmlns:convention", "http://www.xml-cml.org/convention/"},
      {"xmlns:nonsi", "http://www.xml-cml.org/unit/nonSi/"},
      {"xmlns:si", "http://www.xml-cml.org/unit/si/"}}},
                           CMLLogger::joblist = {
                               "module",
                               {{"dictRef", "compchem:jobList"},
                                {"id", "jobList1"}}};

CMLLogger &CMLLogger::operator<<(const element_t &element) {
  ostr << '<' << element.name;
  for (const auto[k, v] : element.attributes) {
    ostr << ' ' << k << '=' << '"' << v << '"';
  }
  ostr << "/>";
  return *this;
}

CMLLogger &CMLLogger::push(const element_t &element) {
  stack.push(element);
  ostr << '<' << element.name;
  for (auto const[k, v] : element.attributes) {
    ostr << ' ' << k << '=' << '"' << v << '"';
  }
  ostr << '>';
  return *this;
}
CMLLogger &CMLLogger::pop() {
  const auto element = stack.top();
  ostr << "</" << element.name << '>' << std::flush;
  stack.pop();
  return *this;
}

CMLLogger::CMLLogger(std::ostream &ostr) : ostr(ostr) {
  push(compchem_root);
  push(joblist);
}

CMLLogger::~CMLLogger() {
  while (!stack.empty()) {
    pop();
  }
}
