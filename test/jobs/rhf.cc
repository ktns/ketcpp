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

go_bandit([] {
  describe("RHF", [] {
    it("should destruct its resources", [] {
      bool mol_alive = true, set_alive = true;
      auto mol = std::make_unique<TestMolecule>(mol_alive);
      auto set = std::make_unique<TestBasisSet>(set_alive);
      [&] {
        RHF job;
        job.prepare(std::move(mol), std::move(set));
      }();
      mol_alive must be_falsy;
      set_alive must be_falsy;
    });

    describe(".release(mol, set)", [] {
      it("should release resources", [] {
        bool mol_alive = true, set_alive = true;
        std::unique_ptr<wrapper::molecule::Base> mol =
            std::make_unique<TestMolecule>(mol_alive);
        std::unique_ptr<orbital::basisset::Base> set =
            std::make_unique<TestBasisSet>(set_alive);
        void *mol_ptr = mol.get(), *set_ptr = set.get();
        RHF job;
        job.prepare(std::move(mol), std::move(set));
        job.release(mol, set);
        mol.get() must equal(mol_ptr);
        set.get() must equal(set_ptr);
      });
    });
  });
});
