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

using matrix_t = Libint2Basis::matrix_t;

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

  template <typename T> void set_param(libint2::Engine &engine, T param) {
    engine.set_params(param);
  }
  template <> void set_param(libint2::Engine &, std::nullptr_t) {}
}

//! @cond PRIVATE
class Libint2Basis::Impl {
  friend Libint2Basis;

private:
  std::shared_ptr<const Libint2Resource> resource;
  std::vector<libint2::Atom> atoms;
  std::unique_ptr<libint2::BasisSet> basis;
  size_t basis_size;
  std::vector<size_t> shell2bf;
  //// for test
  // Impl(const std::string &xyz_file_name, const std::string &basisset_name) {
  //  resource = Libint2Resource::acquire_resource();
  //  std::ifstream xyz_file(xyz_file_name);
  //  atoms = libint2::read_dotxyz(xyz_file);
  //  auto coutbuf = std::cout.rdbuf();
  //  struct nullbuf_t : public std::streambuf {
  //    int overflow(int c) override { return c; }
  //  } nullbuf;
  //  std::cout.rdbuf(&nullbuf);
  //  basis = std::make_unique<libint2::BasisSet>(basisset_name, atoms);
  //  std::cout.rdbuf(coutbuf);
  //  basis_size = basis->nbf();
  //  shell2bf = basis->shell2bf();
  //}
  template <typename Iter>
  Impl(Iter begin, Iter end,
       std::enable_if_t<
           std::is_convertible_v<
               std::decay_t<typename std::iterator_traits<Iter>::value_type>,
               atom_t>,
           const std::string &>
           basisset_name) {
    std::transform(begin, end, std::back_inserter(atoms),
                   [](const atom_t &a) -> libint2::Atom {
                     libint2::Atom o;
                     std::tie(o.x, o.y, o.z, o.atomic_number) = a;
                     return o;
                   });
    auto coutbuf = std::cout.rdbuf();
    struct nullbuf_t : public std::streambuf {
      int overflow(int c) override { return c; }
    } nullbuf;
    std::cout.rdbuf(&nullbuf);
    resource = Libint2Resource::acquire_resource();
    basis = std::make_unique<libint2::BasisSet>(basisset_name, atoms);
    std::cout.rdbuf(coutbuf);
    basis_size = basis->nbf();
    shell2bf = basis->shell2bf();
  }
  template <typename T = std::nullptr_t>
  matrix_t get_1el_matrix(libint2::Operator opr, T param = nullptr) {
    matrix_t mat = wrapper::matrix::make_zero_matrix<double>(basis_size);
    const auto &shells = *basis.get();
    const auto &shell2bf = this->shell2bf;
    libint2::Engine engine(opr, shells.max_nprim(), shells.max_l(), 0);
    set_param(engine, param);
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
  matrix_t &add_rhf_electron_repulsion(matrix_t &fock,
                                       const matrix_t &density) {
    const auto &shells = *basis.get();
    const auto &shell2bf = this->shell2bf;
    libint2::Engine engine(libint2::Operator::coulomb, shells.max_nprim(),
                           shells.max_l(), 0);
    const auto n = shells.size();
    for (size_t i = 0; i < n; i++) {
      for (size_t j = i; j < n; j++) {
        for (size_t k = 0; k < n; k++) {
          for (size_t l = k; l < n; l++) {
            const auto &si = shells[i], &sj = shells[j], &sk = shells[k],
                       &sl = shells[l];
            const size_t offset_i = shell2bf[i], offset_j = shell2bf[j],
                         offset_k = shell2bf[k], offset_l = shell2bf[l],
                         ni = si.size(), nj = sj.size(), nk = sk.size(),
                         nl = sl.size();
            engine.compute(si, sj, sk, sl);
            const auto &results = engine.results()[0];
            if (results == nullptr)
              continue;
            for (size_t bi = 0; bi < ni; bi++) {
              for (size_t bj = 0; bj < nj; bj++) {
                for (size_t bk = 0; bk < nk; bk++) {
                  for (size_t bl = 0; bl < nl; bl++) {
                    size_t br = nl * nk * nj * bi + nl * nk * bj + nl * bk + bl,
                           boi = bi + offset_i, boj = bj + offset_j,
                           bok = bk + offset_k, bol = bl + offset_l;
                    auto r = results[br];
                    if (i != j)
                      r *= 2;
                    if (k != l)
                      r *= 2;
                    fock->at(boi, boj) += density->at(bok, bol) * r / 2.0;
                    fock->at(boj, boi) += density->at(bol, bok) * r / 2.0;
                    fock->at(bok, bol) += density->at(boi, boj) * r / 2.0;
                    fock->at(bol, bok) += density->at(boj, boi) * r / 2.0;
                    fock->at(boi, bok) -= density->at(boj, bol) * r / 8.0;
                    fock->at(boj, bol) -= density->at(boi, bok) * r / 8.0;
                    fock->at(boi, bol) -= density->at(boj, bok) * r / 8.0;
                    fock->at(boj, bok) -= density->at(boi, bol) * r / 8.0;
                    fock->at(bok, boi) -= density->at(bol, boj) * r / 8.0;
                    fock->at(bol, boj) -= density->at(bok, boi) * r / 8.0;
                    fock->at(bol, boi) -= density->at(bok, boj) * r / 8.0;
                    fock->at(bok, boj) -= density->at(bol, boi) * r / 8.0;
                  }
                }
              }
            }
          }
        }
      }
    }
    return fock;
  }
};
//! @endcond

// Libint2Basis::Libint2Basis(const std::string &xyz_file,
//                           const std::string &basisset_name)
//    : impl(new Impl(xyz_file, basisset_name)) {}
// Libint2Basis::~Libint2Basis() {}

Libint2Basis::Libint2Basis(const wrapper::molecule::Base &mol,
                           const std::string &basisset_name)
    : impl(new Impl(mol.atoms().cbegin(), mol.atoms().cend(), basisset_name)) {}

Libint2Basis::~Libint2Basis() {}

matrix_t Libint2Basis::get_overlap() {
  return impl->get_1el_matrix(libint2::Operator::overlap);
}
matrix_t Libint2Basis::get_kinetic() {
  return impl->get_1el_matrix(libint2::Operator::kinetic);
}
matrix_t Libint2Basis::get_nuclear() {
  return impl->get_1el_matrix(libint2::Operator::nuclear,
                              make_point_charges(impl->atoms));
}
matrix_t Libint2Basis::get_nuclear(const std::vector<pointcharge_t> &charges) {
  decltype(libint2::make_point_charges(impl->atoms)) libint2_charges;
  libint2_charges.reserve(charges.size());
  std::transform(charges.cbegin(), charges.cend(),
                 std::back_inserter(libint2_charges),
                 [](const pointcharge_t c) {
                   std::array<double, 3> coord = {{c.x(), c.y(), c.z()}};
                   return std::make_pair(c.q(), coord);
                 });
  assert(libint2_charges.size() == charges.size());
  return impl->get_1el_matrix(libint2::Operator::nuclear, libint2_charges);
}

matrix_t &Libint2Basis::add_rhf_electron_repulsion(matrix_t &fock,
                                                   const matrix_t &density) {
  return impl->add_rhf_electron_repulsion(fock, density);
}
#endif
