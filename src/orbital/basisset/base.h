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

#include <string>

#include "orbital/basis/base.h"
#include "wrapper/molecule/base.h"

namespace ketcpp::orbital::basisset {
  class Base {
  public:
    virtual ~Base() {}
    virtual std::unique_ptr<orbital::basis::Base>
    get_basis(const wrapper::molecule::Base &) const = 0;
    std::unique_ptr<orbital::basis::Base>
    get_basis(std::unique_ptr<const wrapper::molecule::Base> &pmol) const {
      return get_basis(*pmol);
    }
    //! @return The label of basisset (e.g. STO-3G, 6-31G, etc.)
    virtual const std::string &get_label() const {
      static const std::string empty_string = "";
      return empty_string;
    }
  };
}
