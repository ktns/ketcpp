/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2017 Katsuhiko Nishimra
 *
 * This file is part of ketcpp.
 *
 * ketcpp is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3.0 of the License, or any later version.
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

#include <type_traits>

#include "wrapper/matrix/matrix.h"

namespace ketcpp::wrapper::matrix::eigensolver {
  //! @brief Generalized Eigenproblem solver for Hermitian matrices.
  //! @details Implements a solver instance for the generalized eigenproblem,
  //! \f$ A\boldsymbol v =  \lambda B \boldsymbol v,\f$ where both \f$ A\f$ and
  //! \f$ B\f$ are Hermitian matrices.
  //! @tparam Numeric type of elements of matrices.
  template <class T> class GeneralizedEigensolverHermite {
    static_assert(std::is_floating_point_v<T>);
    typedef MatrixBase<T> M;

    const M &a;
    const M &b;

    template <typename U> using optional = ketcpp::wrapper::matrix::optional<U>;

  public:
    //! @brief Initialize the solver with two references to matrices.
    //! @param[in] a A reference to the hermitian matrix \f$A\f$.
    //! @param[in] b A reference to the positive definite hermitian matrix
    //! \f$B\f$.
    //! @note Solvers do not hold copies of the matrices.
    GeneralizedEigensolverHermite(const M &a, const M &b) : a(a), b(b) {
      assert(a.for_each([&a](size_t i, size_t j) -> optional<bool> {
                if (a.at(i, j) != a.at(j, i))
                  return false;
                else
                  return {};
              }).value_or(true));

      assert(b.for_each([&b](size_t i, size_t j) -> optional<bool> {
                if (b.at(i, j) != b.at(j, i))
                  return false;
                else
                  return {};
              }).value_or(true));
      assert(a.dimension() == b.dimension());
    }
    //! @brief Solve the generalized eigenproblem.
    void solve() {}
  };
#ifdef __cpp_deduction_guides
  template <class M>
  GeneralizedEigensolverHermite(const M &a, const M &b)
      ->GeneralizedEigensolverHermite<typename M::scalar_t>;
#endif
}
