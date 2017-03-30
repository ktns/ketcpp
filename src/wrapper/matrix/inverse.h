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

#include "config/ketcpp_config.h"

#ifdef HAVE_EIGEN3
#include <Eigen/Inverse.h>
#endif

#include "wrapper/matrix/base.h"
#include "wrapper/matrix/eigen_dynamic.h"
#include "wrapper/matrix/eigen_fixed.h"

//! Inverse matrix factories
namespace ketcpp::wrapper::matrix::inverse {
  using ketcpp::util::optional;

  //! Inverse matrix factory
  template <typename T> class InverseMatrixFactory {
    InverseMatrixFactory() = delete;
    InverseMatrixFactory(const InverseMatrixFactory &) = delete;
    //! Matrix to be inverted
    const MatrixBase<T> &base;

  public:
    InverseMatrixFactory(const MatrixBase<T> &base) : base(base) {}
    Matrix<T> inverse() {
      if
        constexpr(MatrixEigenImplemented) {
          std::unique_ptr<MatrixEigenCommon<T>> pcopy;
          const MatrixEigenCommon<T> *pbase =
              dynamic_cast<const MatrixEigenCommon<T> *>(&base);
          if (pbase == nullptr) {
            pcopy.reset(new MatrixEigen<T>(base));
            pbase = pcopy.get();
          }
          assert(pbase != nullptr);
          auto ref = pbase->matrix_instance();
          MatrixEigen<T> inverse(ref.inverse());
          return std::move(inverse);
        }
      else {
        throw std::logic_error("No implementation available");
      }
    }
  };

  static_assert(!std::is_default_constructible_v<InverseMatrixFactory<int>>);
  static_assert(!std::is_copy_constructible_v<InverseMatrixFactory<int>>);
}
