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
#include "libint2.hpp"
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
  Impl(const std::string &xyz_file_name, const std::string &basisset_name) {
    resource = Libint2Resource::acquire_resource();
    std::ifstream xyz_file(xyz_file_name);
    std::vector<libint2::Atom> atoms = libint2::read_dotxyz(xyz_file);
    basis = std::make_unique<libint2::BasisSet>(basisset_name, atoms);
  }
};

Libint2Basis::Libint2Basis(const std::string &xyz_file,
                           const std::string &basisset_name)
    : impl(new Impl(xyz_file, basisset_name)) {}
Libint2Basis::~Libint2Basis() {}

Libint2Basis::matrix_t Libint2Basis::get_overlap() {
  return wrapper::matrix::make_matrix({{42.0}}); // FIXME
}

#endif
