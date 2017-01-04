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

#include <memory>
#include <type_traits>

#include "orbital/basisset/base.h"
#include "wrapper/matrix/matrix.h"
#include "wrapper/molecule/base.h"

namespace ketcpp {
  namespace jobs {
    class RHF {
      bool prepared;
      std::unique_ptr<const wrapper::molecule::Base> molecule;
      std::unique_ptr<const orbital::basisset::Base> basisset;
      std::unique_ptr<orbital::basis::Base> basis;
      std::unique_ptr<const wrapper::matrix::Matrix<double>> overlap;

    public:
      RHF() : prepared(false) {}
      ~RHF() {}
      void prepare(std::unique_ptr<const wrapper::molecule::Base> &&,
                   std::unique_ptr<const orbital::basisset::Base> &&);
      void release(std::unique_ptr<wrapper::molecule::Base> &,
                   std::unique_ptr<orbital::basisset::Base> &);
      auto release() {
        std::unique_ptr<wrapper::molecule::Base> mol;
        std::unique_ptr<orbital::basisset::Base> set;
        release(mol, set);
        return make_tuple(std::move(mol), std::move(set));
      }
      const auto &get_basis() { return basis; }
      const auto &get_overlap() { return overlap; }
    };
  }
}
