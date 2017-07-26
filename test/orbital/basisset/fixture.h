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

#include "orbital/basisset/base.h"

namespace ketcpp::orbital::basisset {
  class FixtureSTO3G : public Base {
  public:
    FixtureSTO3G() {}
    ~FixtureSTO3G() {}
    std::unique_ptr<orbital::basis::Base>
    get_basis(const wrapper::molecule::Base &) const override;
    const std::string &get_label() const override {
      static const std::string label = "STO-3G";
      return label;
    }
  };
}
