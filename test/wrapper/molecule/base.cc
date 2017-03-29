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

#include <tuple>

namespace bandit::Matchers {
  template <typename C, typename T>
  std::basic_ostream<C, T> &
  operator<<(std::basic_ostream<C, T> &s,
             const std::tuple<double, double, double, unsigned int> &t) {
#ifdef __cpp_structured_bindings
    auto[x, y, z, Z] = t;
#else
    auto x = std::get<0>(t);
    auto y = std::get<1>(t);
    auto z = std::get<2>(t);
    auto Z = std::get<3>(t);
#endif
    s << x << ',' << y << ',' << z << ',' << Z;
    return s;
  }
}

#include <bandit/bandit.h>

#include "wrapper/molecule/base.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::molecule;

go_bandit([] {
  describe("atom_t", [] {
    describe("::ctor(unit, x, y, z, Z)", [] {
      it("should properly scale coordinates", [] {
        atom_t(LengthUnit::AtomicUnit, 1, 2, 3, 42)
            must equal(std::make_tuple(1.0, 2.0, 3.0, 42u));
        atom_t(LengthUnit::Angstrom, BOHR_RADIUS_IN_ANGSTROM * 1,
               BOHR_RADIUS_IN_ANGSTROM * 2, BOHR_RADIUS_IN_ANGSTROM * 3, 42)
            must equal(std::make_tuple(1.0, 2.0, 3.0, 42u));
      });
    });

    describe("accessors", [] {
      it("should retrieve correct values", [] {
        atom_t atom(1, 2, 3, 42);
        atom.x() must equal(1);
        atom.y() must equal(2);
        atom.z() must equal(3);
        atom.Z() must equal(42);
      });

      it("should be assignable accessors", [] {
        atom_t atom(0, 0, 0, 0);
        atom must equal(std::make_tuple(0.0, 0.0, 0.0, 0u));
        atom.x() = 1;
        atom.y() = 2;
        atom.z() = 3;
        atom.Z() = 42;
        atom.atomic_number() = 42;
        atom must equal(std::make_tuple(1.0, 2.0, 3.0, 42u));
      });
    });
  });

  describe("pointcharge_t", [] {
    describe("::ctor(unit, x, y, z, Z)", [] {
      it("should properly scale coordinates", [] {
        pointcharge_t(LengthUnit::AtomicUnit, 1, 2, 3, 42)
            must equal(std::make_tuple(1.0, 2.0, 3.0, 42.0));
        pointcharge_t(LengthUnit::Angstrom, BOHR_RADIUS_IN_ANGSTROM * 1,
                      BOHR_RADIUS_IN_ANGSTROM * 2, BOHR_RADIUS_IN_ANGSTROM * 3,
                      42) must equal(std::make_tuple(1.0, 2.0, 3.0, 42u));
      });
    });

    describe("::ctor(const atom_t&)", [] {
      it("should properly copy", [] {
        atom_t atom(1, 2, 3, 42);
        pointcharge_t pointcharge(atom);
        pointcharge must equal(std::make_tuple(1.0, 2.0, 3.0, 42.0));
      });
    });

    describe("accessors", [] {
      it("should retrieve correct values", [] {
        pointcharge_t pointcharge(1, 2, 3, 42);
        pointcharge.x() must equal(1);
        pointcharge.y() must equal(2);
        pointcharge.z() must equal(3);
        pointcharge.q() must equal(42);
        pointcharge.charge() must equal(42);
      });

      it("should be assignable accessors", [] {
        pointcharge_t pointcharge(0, 0, 0, 0);
        pointcharge must equal(std::make_tuple(0.0, 0.0, 0.0, 0u));
        pointcharge.x() = 1;
        pointcharge.y() = 2;
        pointcharge.z() = 3;
        pointcharge.q() = 42;
        pointcharge must equal(std::make_tuple(1.0, 2.0, 3.0, 42u));
      });
    });
  });
});
