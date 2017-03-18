/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2015 Katsuhiko Nishimra
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

#include <bandit/bandit.h>

#include "wrapper/matrix/default.h"
#include "wrapper/molecule/base.h"

#include "jobs/rhf.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::jobs;

class TestMolecule : public wrapper::molecule::Base {
  bool &alive;

public:
  TestMolecule(bool &alive) : alive(alive) { alive = true; }
  ~TestMolecule() { alive = false; }

  const std::vector<wrapper::molecule::atom_t> &atoms() const override {
    static const std::vector<wrapper::molecule::atom_t> empty;
    return empty;
  };
};

static const wrapper::matrix::Matrix<double> overlap_matrix =
    wrapper::matrix::make_matrix<double, 2>(
        {rand() * 1., rand() * 1., rand() * 1., rand() * 1.});

static const wrapper::matrix::Matrix<double> kinetic_matrix =
    wrapper::matrix::make_matrix<double, 2>(
        {rand() * 1., rand() * 1., rand() * 1., rand() * 1.});

class TestBasis : public orbital::basis::Base {
  decltype(overlap_matrix) get_overlap() { return overlap_matrix; }
  decltype(kinetic_matrix) get_kinetic() { return kinetic_matrix; }
};

class TestBasisSet : public orbital::basisset::Base {
  bool &alive;

public:
  TestBasisSet(bool &alive) : alive(alive) { alive = true; }
  ~TestBasisSet() { alive = false; }
  std::unique_ptr<ketcpp::orbital::basis::Base>
  get_basis(const wrapper::molecule::Base &) const {
    return std::make_unique<TestBasis>();
  }
};

struct TestSuite {
  bool mol_alive, set_alive;
  std::unique_ptr<wrapper::molecule::Base> mol;
  std::unique_ptr<orbital::basisset::Base> set;
  TestSuite() {
    mol_alive = set_alive = true;
    mol = std::make_unique<TestMolecule>(mol_alive);
    set = std::make_unique<TestBasisSet>(set_alive);
  }
  ~TestSuite() {}
};

go_bandit([] {
  describe("RHF", [] {
    it("should destruct its resources", [] {
      TestSuite t;
      [&t] {
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
      }();
      t.mol_alive must be_falsy;
      t.set_alive must be_falsy;
    });

    describe(".prepare(mol, set)", [] {
      it("should prepare basis", [] {
        TestSuite t;
        RHF job;
        job.get_basis() must be_falsy;
        job.prepare(std::move(t.mol), std::move(t.set));
        job.get_basis() must be_truthy;
      });

      it("should prepare overlap matrix", [] {
        TestSuite t;
        RHF job;
        job.get_overlap() must be_falsy;
        job.prepare(std::move(t.mol), std::move(t.set));
        job.get_overlap() must be_truthy;
        *job.get_overlap() must equal(overlap_matrix);
      });

      it("should prepare core_hamiltonian matrix", [] {
        TestSuite t;
        RHF job;
        job.get_core_hamiltonian() must be_falsy;
        job.prepare(std::move(t.mol), std::move(t.set));
        job.get_core_hamiltonian() must be_truthy;
        //*job.get_overlap() must equal(overlap_matrix);
      });
    });

    describe(".release(mol, set)", [] {
      it("should release resources", [] {
        TestSuite t;
        void *mol_ptr = t.mol.get(), *set_ptr = t.set.get();
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
        t.mol must be_falsy;
        t.set must be_falsy;
        job.release(t.mol, t.set);
        t.mol must be_truthy;
        t.set must be_truthy;
        t.mol.get() must equal(mol_ptr);
        t.set.get() must equal(set_ptr);
      });
    });

    describe(".release()", [] {
      it("should release resources", [] {
        TestSuite t;
        void *mol_ptr = t.mol.get(), *set_ptr = t.set.get();
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
        t.mol must be_falsy;
        t.set must be_falsy;
        std::tie(t.mol, t.set) = job.release();
        t.mol must be_truthy;
        t.set must be_truthy;
        t.mol.get() must equal(mol_ptr);
        t.set.get() must equal(set_ptr);
      });
    });
  });
});
