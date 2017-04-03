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
    //! LCAO basis function system
    std::unique_ptr<basis_t> basis;
    //! Overlap matrix of basis functions
    std::unique_ptr<const matrix_t> overlap;
    //! Core Hamiltonian matrix
    std::unique_ptr<matrix_t> core_hamiltonian;
    //! Fock matrix
    std::unique_ptr<matrix_t> fock;
    //! Orbital energies
    std::unique_ptr<matrix_t> energies;
    //! Orbital coefficients matrix
    std::unique_ptr<matrix_t> coefficients;
    //! Density matrix of alpha-spin electrons
    std::unique_ptr<matrix_t> density;
    //! Previousr density matrix;
    std::unique_ptr<matrix_t> previous_density;

  public:
    //! Default constructor
    RHF() : prepared(false), initial_guess_type({}) {}
    //! Empty destructor
    ~RHF() {}
    //! @brief Prepare an RHF job with a molecule and a basisset to use.
    //! @param mol A wrapper of molecule on which the job will be executed.
    //! The solver acquires ownership of the molecule to prevent any change
    //! the job.
    //! @param set A reference to a basis-set to be used.
    void prepare(std::unique_ptr<const molecule_t> &&mol,
                 const basisset_t &set);
    //! Prepare an RHF job with a molecule and a basisset to use.
    template <typename BS>
    std::enable_if_t<std::is_convertible_v<BS &, const basisset_t &>, void>
    prepare(std::unique_ptr<const molecule_t> &&mol,
            const std::unique_ptr<BS> &set) {
      return prepare(std::move(mol), *set);
    }
    //! Solve the problem
    void solve();
    //! Release resources that job has locked
    void release(std::unique_ptr<molecule_t> &);
    //! Release resources that job has locked
    //! @return std::pair that holds unique_ptr to a molecule and a basisset.
    auto release() {
      std::unique_ptr<molecule_t> mol;
      release(mol);
      return mol;
    }
    //! Accessor to @t prepared
    bool is_prepared() const { return prepared; }
    //! Check whether calculation has converged
    bool has_converged() const;
    //! Accessor to the basis @f$ {|\chi_\alpha\rangle} @f$
    const auto &get_basis() const { return basis; }
    //! Accessor to the overlap matrix @f$ S @f$
    const auto &get_overlap() const { return overlap; }
    //! Accessor to the core hamiltonian matrix @f$ H_\mathrm{core} @f$
    const auto &get_core_hamiltonian() const { return core_hamiltonian; }
    //! Accessor to the Fock matrix @f$ F @f$
    const auto &get_fock() const { return fock; }
    //! @brief Accessor to the orbital energies @f$ E_i @f$
    //! @note Not guaranteed to be sorted
    const auto &get_energies() const { return energies; }
    //! @brief Accessor to the orbital coefficients @f$ C_{i\alpha} @f$
    //! @details
    //! @f$ |\phi_i\rangle = \sum_\alpha C_{i\alpha} |\chi_\alpha\rangle @f$
    //! where @f$ |\phi_i\rangle @f$ is a MO and @f$ |\chi_\alpha\rangle @f$
    //! is a basis funciton.
    const auto &get_coefficients() const { return coefficients; }
    //! @brief Accessor to the density matrix @f$ P @f$ of alpha-spin electrons
    //! @note Due to non-orthogonality of the basis, idempotency of a density
    //! matrix is expressed as
    //! @f$ (S^{\frac 1 2} P S^{\frac 1 2})^n =S^{\frac 1 2} P S^{\frac 1 2}.@f$
    const auto &get_density() const { return density; }
    //! @brief Calculate the molecular energy
    //! @return @f$ \mathrm{Tr} P(F+H_\mathrm{core}) + E_\mathrm{nucl} @f$,
    //! where @f$ E_\mathrm{nucl}@f$ is the nuclear repulsion energy.
    //! @pre The solver should be prepared, and the Fock matrix and the
    //! Density matrix should have been calculated already.
    double calc_energy() const;

    //! Number of electrons
    size_t num_electrons() const {
      assert(molecule);
      return molecule->total_nuclear_charge() - molecule->formal_charge();
    }

    //! Make initial guess for the RHF problem
    InitialGuessType make_initial_guess(InitialGuessMethod);
    //! Accessor to initial guess type
    auto get_initial_guess_type() { return initial_guess_type; }

    //! @brief Update orbital energies and coefficients based on the current
    //! Fock matrix.
    void update_orbital();

    //! @brief Update the density matrix based on the current orbitals.
    void update_density();

    //! @brief Update the Fock matrix based on the current density matrix.
    void update_fock();
  };
}
