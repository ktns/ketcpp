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

#include "jobs/rhf.h"
#include <bandit/bandit.h>
using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::jobs;

class TestMolecule : public wrapper::molecule::Base {
  bool &alive;

public:
  TestMolecule(bool &alive) : alive(alive) { alive = true; }
  ~TestMolecule() { alive = false; }
};

class TestBasisSet : public orbital::basisset::Base {
  bool &alive;

public:
  TestBasisSet(bool &alive) : alive(alive) { alive = true; }
  ~TestBasisSet() { alive = false; }
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

    describe(".release(mol, set)", [] {
      it("should release resources", [] {
        TestSuite t;
        void *mol_ptr = t.mol.get(), *set_ptr = t.set.get();
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
        t.mol.get() must be_null;
        t.set.get() must be_null;
        job.release(t.mol, t.set);
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
        t.mol.get() must be_null;
        t.set.get() must be_null;
        std::tie(t.mol, t.set) = job.release();
        t.mol.get() must equal(mol_ptr);
        t.set.get() must equal(set_ptr);
      });
    });
  });
});
