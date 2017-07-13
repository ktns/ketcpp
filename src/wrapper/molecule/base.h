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

#include <cmath>
#include <tuple>

#include "wrapper/matrix/array.h"

//! molecule adaptors
namespace ketcpp::wrapper::molecule {
  //! The value of Bohr radius \f$a_\mathrm H\f$ in Angstrom.
  constexpr double BOHR_RADIUS_IN_ANGSTROM = 0.52917721067;
  //! Supported units of length
  enum struct LengthUnit { AtomicUnit, Angstrom };
  //! @return Scale factor from unit @p to an atomic unit
  constexpr double scale(LengthUnit u) {
    switch (u) {
    case LengthUnit::AtomicUnit:
      return 1;
    case LengthUnit::Angstrom:
      return 1.0 / BOHR_RADIUS_IN_ANGSTROM;
    default:
      throw std::logic_error("Unknown value of wrapper::molecule::LengthUnit");
    }
  }

  //! @brief Represents an atom.
  //! @details Holds coordinates in atomic units and an atomic number.
  struct atom_t : public std::tuple<double, double, double, unsigned int> {
  private:
    template <typename T, size_t m, size_t n>
    using MatrixArray = wrapper::matrix::MatrixArray<T, m, n>;
    typedef std::tuple<double, double, double, unsigned int> tuple;

  public:
    //! @details This class can be initialized as <tt> tuple<double,
    //! double, double,  unsigned int> </tt>
    using tuple::tuple;

    //! @brief Accessor to coordinates in atomic units.
    auto &x() { return std::get<0>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to coordinates in atomic units.
    auto &y() { return std::get<1>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to coordinates in atomic units.
    auto &z() { return std::get<2>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to atomic_number
    auto &Z() { return std::get<3>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to atomic_number
    auto &atomic_number() { return Z(); }
    //! @brief Accessor to coordinates in atomic units.
    auto x() const { return const_cast<atom_t *>(this)->x(); }
    //! @brief Accessor to coordinates in atomic units.
    auto y() const { return const_cast<atom_t *>(this)->y(); }
    //! @brief Accessor to coordinates in atomic units.
    auto z() const { return const_cast<atom_t *>(this)->z(); }
    //! @brief Accessor to atomic_number
    auto Z() const { return const_cast<atom_t *>(this)->Z(); }
    //! @brief Accessor to atomic_number
    auto atomic_number() const { return Z(); }

    //! @brief Returns coordinates of the atom
    MatrixArray<double, 1, 3> coordinates() const {
      MatrixArray<double, 1, 3> coordinates = {x(), y(), z()};
      return coordinates;
    }

    //! Initialize an atom with a coordinates with specified units
    atom_t(LengthUnit u, double x, double y, double z, unsigned int Z)
        : tuple(x * scale(u), y * scale(u), z * scale(u), Z) {}
  };
  static_assert(sizeof(std::tuple<double, double, double, unsigned int>) ==
                sizeof(atom_t));

  //!  Represent a point charge with an arbitrary charge.
  struct pointcharge_t : public std::tuple<double, double, double, double> {
  private:
    typedef std::tuple<double, double, double, double> tuple;

  public:
    //! @details This class can be initialized as <tt>tuple<double, double,
    //! double,  unsigned int></tt>.
    using tuple::tuple;

    //! @brief Accessor to coordinates in atomic units.
    auto &x() { return std::get<0>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to coordinates in atomic units.
    auto &y() { return std::get<1>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to coordinates in atomic units.
    auto &z() { return std::get<2>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to charge in elementary charge units.
    auto &q() { return std::get<3>(static_cast<tuple &>(*this)); }
    //! @brief Accessor to charge in elementary charge units.
    auto &charge() { return q(); }
    //! @brief Accessor to coordinates in atomic units.
    auto x() const { return const_cast<pointcharge_t *>(this)->x(); }
    //! @brief Accessor to coordinates in atomic units.
    auto y() const { return const_cast<pointcharge_t *>(this)->y(); }
    //! @brief Accessor to coordinates in atomic units.
    auto z() const { return const_cast<pointcharge_t *>(this)->z(); }
    //! @brief Accessor to charge in the elementary charge as a unit.
    auto q() const { return const_cast<pointcharge_t *>(this)->q(); }
    //! @brief Accessor to charge in the elementary charge as a unit.
    auto charge() const { return q(); }

    //! @brief Construct a point-charge that represents an atom passed as a
    //! parameter.
    pointcharge_t(const atom_t &atom)
        : tuple(atom.x(), atom.y(), atom.z(), atom.Z()) {}

    //! Construct a point-charge with coordinates in the specified unit.
    //! @param[in] u Unit of length in which coordinates are.
    //! @param[in] x,y,z Coordinates in unit @p u.
    //! @param[in] q The magnitude of a point-charge in elementary charge units.
    pointcharge_t(LengthUnit u, double x, double y, double z, double q)
        : tuple(x * scale(u), y * scale(u), z * scale(u), q) {}
  };
  static_assert(sizeof(std::tuple<double, double, double, unsigned int>) ==
                sizeof(atom_t));

  //! @brief Base class for classes which represent molecules
  class Base {
  public:
    //! @brief Returns all atoms in the molecule
    virtual const std::vector<atom_t> &atoms() const = 0;
    //! @return Number of atoms in the molecule
    virtual size_t get_num_atoms() const { return atoms().size(); }
    //! @brief Total nuclear charge
    virtual size_t total_nuclear_charge() const {
      const auto as = atoms();
      return std::accumulate(
          as.begin(), as.end(), 0u,
          [](size_t q, const atom_t &a) { return q + a.Z(); });
    }
    //! @brief Formal molecular charge
    virtual int formal_charge() const = 0;
    //! @brief Number of electrons
    virtual size_t get_num_electrons() const {
      const auto fc = formal_charge();
      const auto nc = total_nuclear_charge();
      assert(fc < 0 ||
             static_cast<std::make_unsigned_t<decltype(fc)>>(fc) < nc);
      return nc - fc;
    }
    //! @brief Returns nuclear repulsion energy
    virtual double nuclear_repulsion_energy() const {
      const auto &atoms = this->atoms();
      double nuclear_repulsion_energy = 0;
      for (auto ai = atoms.begin(); ai < atoms.end(); ai++) {
        for (auto aj = ai + 1; aj < atoms.end(); aj++) {
          auto coordinates = ai->coordinates() - aj->coordinates();
          const auto x = coordinates->at(0, 0), y = coordinates->at(0, 1),
                     z = coordinates->at(0, 2);
          const auto Zi = ai->atomic_number(), Zj = aj->atomic_number();
          const auto r = std::sqrt(x * x + y * y + z * z);
          nuclear_repulsion_energy += Zi * Zj / r;
        }
      }
      return nuclear_repulsion_energy;
    }

    //! @brief Empty virtual destructor
    virtual ~Base() {}
  };
}
