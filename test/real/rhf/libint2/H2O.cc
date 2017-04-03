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

#include <bandit/bandit.h>

#include "jobs/rhf.h"
#include "orbital/basisset/libint2.h"
#include "wrapper/matrix.h"
#include "wrapper/matrix/dummy.h"
#include "wrapper/molecule/fixture.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp;
using namespace ketcpp::jobs;
using namespace ketcpp::orbital::basisset;
using namespace ketcpp::wrapper::matrix;
using namespace ketcpp::wrapper::molecule;

go_bandit([] {
  describe("RHF using libint2", [] {
    std::unique_ptr<Libint2BasisSet> basisset;
    std::unique_ptr<FixtureH2O> mol;
    Matrix<double> correct_overlap = make_dummy_matrix<double>(),
                   correct_coreh = correct_overlap,
                   correct_fock = correct_overlap,
                   correct_density = correct_overlap;

    before_each([&] {
      basisset = std::make_unique<Libint2BasisSet>("STO-3G");
      correct_overlap = wrapper::matrix::make_symmetric_matrix<double>(
          {{+0.10000000e+01},
           {+0.23670392e+00, +0.10000000e+01},
           {+0.00000000e+00, +0.00000000e+00, +0.10000000e+01},
           {+0.00000000e+00, +0.00000000e+00, +0.00000000e+00, +0.10000000e+01},
           {+0.00000000e+00, +0.00000000e+00, +0.00000000e+00, +0.00000000e+00,
            +0.10000000e+01},
           {+0.52485534e-01, +0.46709281e+00, +0.38887100e+00, +0.14311115e-01,
            -0.11964767e-01, +0.10000000e+01},
           {+0.52485684e-01, +0.46709359e+00, -0.11203435e+00, +0.28606226e+00,
            -0.23913519e+00, +0.24703413e+00, +0.10000000e+01}});
      correct_coreh = wrapper::matrix::make_symmetric_matrix<double>(
          {{-3.2707466e+01},
           {-7.6096206e+00, -9.3225767e+00},
           {-1.0771234e-02, -1.2723748e-01, -7.5790415e+00},
           {-1.1687029e-02, -1.3805543e-01, +2.1247898e-02, -7.5134646e+00},
           {+9.7698825e-03, +1.1540874e-01, -1.7761876e-02, +5.5020976e-02,
            -7.4936416e+00},
           {-1.7005997e+00, -3.6664246e+00, -2.5689634e+00, -1.3765816e-01,
            +1.1508480e-01, -5.0290866e+00},
           {-1.7006045e+00, -3.6664318e+00, +6.8713967e-01, -1.9041595e+00,
            +1.5917920e+00, -1.5659711e+00, -5.0290911e+00}});
      correct_density =
          wrapper::matrix::make_symmetric_matrix<double>(
              {{+2.1066504e+00},
               {-4.4842475e-01, +1.9764733e+00},
               {+6.2699979e-02, -3.5473274e-01, +8.9938392e-01},
               {+6.8030674e-02, -3.8489207e-01, +1.7678412e-01, +1.4480347e+00},
               {-5.6870884e-02, +3.2175416e-01, -1.4778013e-01, +4.6141830e-01,
                +1.6142749e+00},
               {-2.6944853e-02, -3.8620675e-02, +7.1749737e-01, +5.9084858e-02,
                -4.9394761e-02, +6.0813286e-01},
               {-2.6944918e-02, -3.8620935e-02, -1.6655332e-01, +5.3869946e-01,
                -4.5032869e-01, -1.8779198e-01, +6.0813231e-01}}) /
          2;
      correct_fock = wrapper::matrix::make_symmetric_matrix<double>(
          {{-2.0235145e+01},
           {-5.1631145e+00, -2.4426736e+00},
           {-1.6805364e-02, -7.0696673e-02, -3.2179111e-01},
           {-1.8234171e-02, -7.6707645e-02, -1.2598994e-02, -3.5705840e-01},
           {+1.5243027e-02, +6.4124480e-02, +1.0531978e-02, -2.8297299e-02,
            -3.6725339e-01},
           {-1.1626680e+00, -1.0043084e+00, -5.3130902e-01, -5.5699306e-02,
            +4.6563965e-02, -5.6350596e-01},
           {-1.1626713e+00, -1.0043104e+00, +1.0865265e-01, -4.0289126e-01,
            +3.3679915e-01, -3.8176459e-01, -5.6350754e-01}});
      mol.reset(new FixtureH2O);
    });

    it("should be able to solve RHF problem about H2O molecule", [&] {
      auto within = [](double delta) {
        return [delta](double a, double b) { return std::abs(a - b) < delta; };
      };
      RHF job({InitialGuessMethod::CoreHamiltonian, 1e-5, 100});
      job.prepare(std::move(mol), basisset);
      job.make_initial_guess(InitialGuessMethod::CoreHamiltonian);
      {
        const auto &F = *job.get_fock();
        AssertThat(F, EqualsContainer(correct_coreh, within(1e-6)));
      }
      job.update_orbital();
      job.update_density();
      auto e = job.calc_energy();
      e must be_less_than(-74.9644492715);
      job.update_fock();
      e = job.calc_energy();
      e must be_greater_than(-74.9644492715);
      for (int i = 0; i < 30; i++) {
        job.update_orbital();
        job.update_density();
        job.update_fock();
        auto e2 = job.calc_energy();
        e2 must be_less_than(e + 1e-5);
        e = e2;
      }
      {
        const auto &F = *job.get_fock(), &P = *job.get_density();
        AssertThat(F, EqualsContainer(correct_fock, within(1e-6)));
        AssertThat(P, EqualsContainer(correct_density, within(1e-6)));
      }

      e must be_close_to(-74.9644492715).within(1e-4);
    });
  });
});

#endif
