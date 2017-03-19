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

#include <ios>

#include "wrapper/matrix/default.h"
#include "wrapper/molecule/base.h"

#include "jobs/rhf.h"

using namespace ketcpp;
using namespace ketcpp::jobs;
using orbital::basis::matrix_t;

namespace bandit::Matchers {
  template <typename C, typename T>
  std::basic_ostream<C, T> &operator<<(std::basic_ostream<C, T> &out,
                                       const InitialGuessType type) {
    return out << [type] {
      switch (type) {
      case InitialGuessType::Density:
        return "InitialGuessType::Density";
      case InitialGuessType::Fock:
        return "InitialGuessType::Fock";
      }
    }();
  }

  template <typename C, typename T>
  std::basic_ostream<C, T> &operator<<(std::basic_ostream<C, T> &out,
                                       const InitialGuessMethod method) {
    return out << [method] {
      switch (method) {
      case InitialGuessMethod::CoreHamiltonian:
        return "InitialGuessMethod::CoreHamiltonian";
      }
    }();
  }

  template <typename C, typename T>
  std::basic_ostream<C, T> &
  operator<<(std::basic_ostream<C, T> &out,
             const util::optional<InitialGuessType> type) {
    if (type)
      return out << type.value();
    else
      return out << "{}";
  }

  template <typename C, typename T>
  std::basic_ostream<C, T> &
  operator<<(std::basic_ostream<C, T> &out,
             const util::optional<InitialGuessMethod> method) {
    if (method)
      return out << method.value();
    else
      return out << "{}";
  }
}

#include <bandit/bandit.h>

using namespace bandit;
using namespace bandit::Matchers;

class TestMolecule : public wrapper::molecule::Base {
  bool &alive;

public:
  TestMolecule(bool &alive) : alive(alive) { alive = true; }
  ~TestMolecule() { alive = false; }

  const std::vector<wrapper::molecule::atom_t> &atoms() const override {
    static const std::vector<wrapper::molecule::atom_t> empty;
    return empty;
  };
  size_t total_nuclear_charge() const override { return 1; }
  int formal_charge() const override { return 0; }
};

static const matrix_t overlap_matrix = [] {
  auto x = wrapper::matrix::make_matrix<double, 2>(
      {rand() * 1., rand() * 1., rand() * 1., rand() * 1.});
  return x * x->transpose();
}();

static const matrix_t kinetic_matrix = [] {
  auto x = wrapper::matrix::make_matrix<double, 2>(
      {rand() * 1., rand() * 1., rand() * 1., rand() * 1.});
  return x + x->transpose();
}();

static const matrix_t nuclear_attraction_matrix = [] {
  auto x = wrapper::matrix::make_matrix<double, 2>(
      {rand() * 1., rand() * 1., rand() * 1., rand() * 1.});
  return x + x->transpose();
}();

static const matrix_t electron_repulsion_matrix = [] {
  auto x = wrapper::matrix::make_matrix<double, 2>(
      {rand() * 1., rand() * 1., rand() * 1., rand() * 1.});
  return x + x->transpose();
}();

class TestBasis : public orbital::basis::Base {
  matrix_t get_overlap() { return overlap_matrix; }
  matrix_t get_kinetic() { return kinetic_matrix; }
  matrix_t get_nuclear(const std::vector<wrapper::molecule::pointcharge_t> &) {
    return nuclear_attraction_matrix;
  }
  matrix_t &add_rhf_electron_repulsion(matrix_t &fock,
                                       const matrix_t &density) {
    return fock += electron_repulsion_matrix;
  }
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
    mol_alive = set_alive = false;
    mol = std::make_unique<TestMolecule>(mol_alive);
    set = std::make_unique<TestBasisSet>(set_alive);
  }
  ~TestSuite() {}
};

go_bandit([] {
  describe("RHF", [] {
    it("should destruct its resources", [] {
      TestSuite t;
      std::unique_ptr<RHF> pjob(new RHF);
      pjob->prepare(std::move(t.mol), std::move(t.set));
      t.mol_alive must be_truthy;
      t.set_alive must be_truthy;
      pjob.reset();
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
        *job.get_core_hamiltonian()
             must equal(kinetic_matrix + nuclear_attraction_matrix);
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

    describe(".make_initial_guess()", [] {
      it("should set the type of initial guess made", [] {
        TestSuite t;
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
        job.get_initial_guess_type() must be_falsy;
        auto ret = job.make_initial_guess(InitialGuessMethod::CoreHamiltonian);
        ret must equal(InitialGuessType::Fock);
        job.get_initial_guess_type() must be_truthy;
        job.get_initial_guess_type().value() must equal(InitialGuessType::Fock);
      });
    });

    describe(".update_orbital()", [] {
      it("should set orbital energies", [] {
        TestSuite t;
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
        job.get_initial_guess_type() must be_falsy;
        job.make_initial_guess(InitialGuessMethod::CoreHamiltonian);
        job.update_orbital();
        job.get_energies() must be_truthy;
      });

      it("should set orbital coefficients", [] {
        TestSuite t;
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
        job.get_initial_guess_type() must be_falsy;
        job.make_initial_guess(InitialGuessMethod::CoreHamiltonian);
        job.update_orbital();
        job.get_coefficients() must be_truthy;
      });
    });

    describe(".update_density()", [] {
      it("should set density matrix", [] {
        TestSuite t;
        RHF job;
        job.prepare(std::move(t.mol), std::move(t.set));
        job.get_initial_guess_type() must be_falsy;
        job.make_initial_guess(InitialGuessMethod::CoreHamiltonian);
        job.update_orbital();
        job.get_density() must be_falsy;
        job.update_density();
        job.get_density() must be_truthy;
      });
    });
  });
});
