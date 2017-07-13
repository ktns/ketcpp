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

#include "wrapper/molecule/base.h"

namespace ketcpp {
  namespace wrapper {
    namespace molecule {
      class FixtureH2O : public Base {
      private:
        const std::vector<atom_t> h2o_atoms = {
            /*O*/ {LengthUnit::Angstrom, +0.93372, -0.03509, +0.09581, 8},
            /*H*/ {LengthUnit::Angstrom, +1.90161, +0.00053, +0.06603, 1},
            /*H*/ {LengthUnit::Angstrom, +0.65487, +0.67691, -0.49939, 1}};

      public:
        const std::vector<atom_t> &atoms() const override { return h2o_atoms; }
        int formal_charge() const override { return 0; }
      };

      class FixtureH3OP : public Base {
      private:
        const std::vector<atom_t> h3o_atoms = {
            /*H*/ {LengthUnit::Angstrom, +0.996267, +0.074625, +0.059771, 1},
            /*O*/ {LengthUnit::Angstrom, +1.986638, +0.106014, +0.043777, 8},
            /*H*/ {LengthUnit::Angstrom, +2.333373, +0.123485, -0.884432, 1},
            /*H*/ {LengthUnit::Angstrom, +2.333379, +0.867918, +0.574190, 1}};

      public:
        const std::vector<atom_t> &atoms() const override { return h3o_atoms; }
        int formal_charge() const override { return 1; }
      };
    }
  }
}
