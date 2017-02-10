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

#include "config/ketcpp_config.h"

#ifdef LIBINT2_FOUND

#include <memory>

#include <libint2.hpp>

#include "orbital/basis/libint.h"
#include "wrapper/matrix/default.h"

using namespace ketcpp;
using namespace ketcpp::orbital::basis;

namespace {
  class Libint2Resource {
  private:
    Libint2Resource() {
      libint2::initialize();
      assert(libint2::initialized());
    }
    static std::weak_ptr<const Libint2Resource> resource;

  public:
    ~Libint2Resource() {
      libint2::finalize();
      assert(!libint2::initialized());
    }
    static std::shared_ptr<const Libint2Resource> acquire_resource() {
      auto ptr = resource.lock();
      if (!ptr) {
        ptr.reset(new Libint2Resource);
        resource = ptr;
      }
      assert(ptr);
      return ptr;
    }
  };
  std::weak_ptr<const Libint2Resource> Libint2Resource::resource;
}

class Libint2Basis::Impl {
  friend Libint2Basis;

private:
  std::shared_ptr<const Libint2Resource> resource;
  std::unique_ptr<libint2::BasisSet> basis;
  size_t basis_size;
  std::vector<size_t> shell2bf;
  Impl(const std::string &xyz_file_name, const std::string &basisset_name) {
    resource = Libint2Resource::acquire_resource();
    std::ifstream xyz_file(xyz_file_name);
    std::vector<libint2::Atom> atoms = libint2::read_dotxyz(xyz_file);
    basis = std::make_unique<libint2::BasisSet>(basisset_name, atoms);
    basis_size = basis->nbf();
    shell2bf = basis->shell2bf();
  }
  Libint2Basis::matrix_t get_1el_matrix(libint2::Operator opr);
};

Libint2Basis::Libint2Basis(const std::string &xyz_file,
                           const std::string &basisset_name)
    : impl(new Impl(xyz_file, basisset_name)) {}
Libint2Basis::~Libint2Basis() {}

Libint2Basis::matrix_t
Libint2Basis::Impl::get_1el_matrix(libint2::Operator opr) {
  matrix_t mat = wrapper::matrix::make_zero_matrix<double>(basis_size);
  const auto &shells = *basis.get();
  const auto &shell2bf = this->shell2bf;
  libint2::Engine engine(opr, shells.max_nprim(), shells.max_l(), 0);
  for (size_t s1 = 0; s1 < shell2bf.size(); s1++) {
    for (size_t s2 = s1; s2 < shell2bf.size(); s2++) {
      engine.compute(shells[s1], shells[s2]);
      auto &result = engine.results()[0];
      auto ns2 = shells[s2].size();
      auto offset1 = shell2bf[s1], offset2 = shell2bf[s2];
      for (size_t i1 = 0; i1 < shells[s1].size(); i1++) {
        for (size_t i2 = 0; i2 < shells[s2].size(); i2++) {
          mat->at(offset1 + i1, offset2 + i2) =
              mat->at(offset2 + i2, offset1 + i1) = result[i1 * ns2 + i2];
        }
      }
    }
  }
  return mat;
}
Libint2Basis::matrix_t Libint2Basis::get_overlap() {
  return impl->get_1el_matrix(libint2::Operator::overlap);
}

#endif
