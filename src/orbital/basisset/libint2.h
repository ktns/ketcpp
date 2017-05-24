
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

#include "config/ketcpp_config.h"

#ifdef LIBINT2_FOUND

#include <string>

#include "orbital/basis/libint.h"
#include "orbital/basisset/base.h"

namespace ketcpp::orbital::basisset {
  //! Create Libint2Basis
  class Libint2BasisSet : public Base {
  private:
    const std::string name;

  public:
    //! Load a basisset from its name using libint2
    Libint2BasisSet(const std::string &basisset_name) : name(basisset_name) {}
    //! Factory method for Libint2Basis
    std::unique_ptr<orbital::basis::Base>
    get_basis(const wrapper::molecule::Base &mol) const override {
      return std::make_unique<orbital::basis::Libint2Basis>(mol, name);
    }
  };
}

#endif
