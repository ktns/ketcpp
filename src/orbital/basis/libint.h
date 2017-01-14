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

#include "orbital/basisset/gaussian.h"
#include "wrapper/molecule/base.h"
#include <memory>

namespace ketcpp {
  namespace orbital {
    namespace basis {
      class Libint2Basis {
      private:
        class Impl;
        std::unique_ptr<Impl> impl;
        typedef wrapper::molecule::Base mol_t;
        typedef basisset::Gaussian bset_t;
        friend Impl;

      public:
        Libint2Basis(const mol_t &, const bset_t &);
        ~Libint2Basis();
      };
    }
  }
}

#endif
