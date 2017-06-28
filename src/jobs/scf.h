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

#include "orbital/basisset/base.h"
#include "wrapper/matrix.h"
#include "wrapper/molecule/base.h"

//! Job administrators
namespace ketcpp::jobs {
  //! Type of initial guess.
  enum class InitialGuessType {
    //! Guess density matrix.
    Density,
    //! Guess Fock matrix.
    Fock
  };

  //! Enumerates implemented initial guess method.
  enum class InitialGuessMethod {
    //! Use core hamiltonian as initial guess of Fock matrix.
    CoreHamiltonian
  };

  typedef wrapper::molecule::Base molecule_t;
  typedef orbital::basisset::Base basisset_t;
  typedef orbital::basis::Base basis_t;
  typedef wrapper::matrix::Matrix<double> matrix_t;

  //! Base class for various type of SCF jobs
  class SCF {
    SCF() = delete;
    SCF(const SCF &) = delete;
    SCF(SCF &&) = delete;

  public:
    //! Configuration values for an SCF job.
    struct Configuration {
      Configuration() = delete;
      //! Method to make an initial guess
      InitialGuessMethod initial_guess_method;
      //! Convergence criterion for Frobenius norm of density matrix change
      //! @f$ ||P_n - P_{n-1}|| @f$
      matrix_t::value_type convergence_criterion;
      //! Max number of iterations
      size_t max_iterations;
    };
  };
}
