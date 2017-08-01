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

#include <cassert>

#include "jobs/scf.h"
#include "logger/cml.h"
#include "util/element.h"

using namespace std::literals::string_literals;

using namespace ketcpp::logger;

const CMLLogger::element_t
    CMLLogger::compchem_root =
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
    CMLLogger::joblist = {"module",
                          {{"id", "jobList1"},
                           {"dictRef", "compchem:jobList"}}},
    CMLLogger::job = {"module", {{"dictRef", "compchem:job"}, {"id", "job"}}},
    CMLLogger::jobinit = {"module",
                          {{"id", "jobInitialization"},
                           {"dictRef", "compchem:initialization"}}},
    CMLLogger::paramlist = {"parameterList", {}},
    CMLLogger::molecule = {"molecule", {{"id", "molgeom"}}},
    CMLLogger::atomarray = {"atomArray", {}};

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

template <typename DictRef, typename Parameter>
void CMLLogger::emit_parameter(DictRef dictref, Parameter parameter,
                               const char *unit) {
  assert(stack.top().name == "parameterList");
  using namespace std::literals::string_literals;
  const element_t tag = {"parameter", {{"dictRef", "compchem:"s + dictref}}};
  push(tag);
  emit_scalar(parameter, unit);
  assert(stack.top().name == "parameter" &&
         stack.top().attributes.at("dictRef") == tag.attributes.at("dictRef"));
  pop();
}

template <typename...> inline constexpr bool false_v[[maybe_unused]] = false;

template <typename Scalar>
void CMLLogger::emit_scalar(Scalar scalar, const char *unit) {
  element_t tag;

  if
#ifdef __cpp_if_constexpr
      constexpr
#endif
      (std::is_convertible_v<Scalar, const std::string &>) {
    tag = {"scalar", {{"dataType", "xsd:string"}}};
  } else if
#ifdef __cpp_if_constexpr
      constexpr
#endif
      (std::is_integral_v<Scalar>) {
    tag = {"scalar", {{"dataType", "xsd:integer"}}};
  } else if
#ifdef __cpp_if_constexpr
      constexpr
#endif
      (std::is_floating_point_v<Scalar>) {
    tag = {"scalar", {{"dataType", "xsd:double"}}};
  } else {
#ifdef __cpp_if_constexpr
    static_assert(false_v<Scalar>, "Unknown type of scalar!");
#endif
  }

  if (unit != nullptr) {
    tag.attributes["units"] = unit;
  }

  push(tag);
  ostr << scalar;
  pop();
}

void CMLLogger::initialize_scf(const mol_t &mol, const basisset_t &set,
                               const scf_conf_t &conf) {
  static const element_t task = {"parameter", {{"dictRef", "compchem:task"}}},
                         num_atoms = {"paramlist",
                                      {{"dictRef", "compchem:numAtoms"}}};
  push(job);
  push(jobinit);
  emit_scf_parameters(conf, mol);
  assert(stack.top() == jobinit);
  emit_molecule(mol);
  assert(stack.top() == jobinit);
  pop();
  assert(stack.top() == job);
}

void CMLLogger::emit_scf_parameters(const scf_conf_t &conf, const mol_t &mol) {
  push(paramlist);
  emit_parameter("method", "scf");
  emit_parameter("task", "energy");
  emit_parameter("numAtoms", mol.get_num_atoms());
  emit_parameter("numElectrons", mol.get_num_electrons());
  emit_parameter("numAlphaElectrons", mol.get_num_alpha_electrons());
  emit_parameter("numBetaElectrons", mol.get_num_beta_electrons());
  emit_parameter("charge", static_cast<double>(mol.formal_charge()),
                 "nonsi:elementary_charge");
  emit_parameter("spinMultiplicty", mol.get_multiplicity());
  assert(stack.top() == paramlist);
  pop();
}

void CMLLogger::emit_molecule(const mol_t &mol) {
  push(molecule);
  push(atomarray);
  size_t i = 0;
  for (const wrapper::molecule::atom_t a : mol.atoms()) {
    element_t e = {"atom",
                   {
                       {"id", "a"s + std::to_string(++i)},
                       {"elementType", util::Z_to_element_symbol(a.Z())},
                       {"x3", std::to_string(a.x())},
                       {"y3", std::to_string(a.y())},
                       {"z3", std::to_string(a.z())},
                   }};
    assert(stack.top() == atomarray);
    *this << e;
    assert(stack.top() == atomarray);
  }
  assert(stack.top() == atomarray);
  pop();
  assert(stack.top() == molecule);
  pop();
  assert(stack.top() == jobinit);
}
