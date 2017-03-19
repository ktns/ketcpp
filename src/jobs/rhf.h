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

#include <memory>
#include <type_traits>

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

  //! Administrates a job that solves a Restricted Hartree Forck problem.
  class RHF {
    //! Whether the job is prepared to run
    bool prepared;
    //! Which type of initialguess have made
    util::optional<InitialGuessType> initial_guess_type;
    //! Molecule to solve
    std::unique_ptr<const molecule_t> molecule;
    //! BasisSet to use
    std::unique_ptr<const basisset_t> basisset;
    //! LCAO basis function system
    std::unique_ptr<basis_t> basis;
    //! Overlap matrix of basis functions
    std::unique_ptr<const matrix_t> overlap;
    //! Core Hamiltonian matrix
    std::unique_ptr<matrix_t> core_hamiltonian;
    //! Fock matrix
    std::unique_ptr<matrix_t> fock;

  public:
    //! Default constructor
    RHF() : prepared(false), initial_guess_type({}) {}
    //! Empty destructor
    ~RHF() {}
    //! Prepare an RHF job and lock a molecule and a basisset to use.
    void prepare(std::unique_ptr<const molecule_t> &&,
                 std::unique_ptr<const basisset_t> &&);
    //! Release resources that job has locked
    void release(std::unique_ptr<molecule_t> &, std::unique_ptr<basisset_t> &);
    //! Release resources that job has locked
    //! @return std::pair that holds unique_ptr to a molecule and a basisset.
    auto release() {
      std::unique_ptr<molecule_t> mol;
      std::unique_ptr<basisset_t> set;
      release(mol, set);
      return make_tuple(std::move(mol), std::move(set));
    }
    //! Accessor to the basis
    const auto &get_basis() { return basis; }
    //! Accessor to the overlap matrix
    const auto &get_overlap() { return overlap; }
    //! Accessor to the core hamiltonian matrix
    const auto &get_core_hamiltonian() { return core_hamiltonian; }

    //! Make initial guess for the RHF problem
    InitialGuessType make_initial_guess(InitialGuessMethod);
    //! Accessor to initial guess type
    auto get_initial_guess_type() { return initial_guess_type; }
  };
}
