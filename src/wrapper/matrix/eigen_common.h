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

#ifdef EIGEN3_FOUND

#include <type_traits>

#include <Eigen/Dense>

#include "wrapper/matrix/matrix.h"

namespace ketcpp::wrapper::matrix {
  //! Whether MatrixEigen is implemented
  constexpr bool MatrixEigenImplemented = true;
  //! Common underlying class for matrix instances using Eigen3
  template <typename T> class MatrixEigenCommon : public MatrixBase<T> {
  public:
    //! Dynamic-size and row-major ordered Eigen::Matrix
    typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                          Eigen::StorageOptions::RowMajor>
        EigenBase;
    //! Reference to Eigen::Matrix
    typedef Eigen::Ref<EigenBase> common_matrix_ref;
    //! Constant reference to Eigen::Matrix
    typedef Eigen::Ref<const EigenBase> const_common_matrix_ref;
    //! Returns a reference to an underlying Eigen::Matrix
    virtual common_matrix_ref matrix_instance() = 0;
    //! Returns a constant reference to an underlying Eigen::Matrix
    virtual const_common_matrix_ref matrix_instance() const = 0;

    //! Base matrix class
    using Base = MatrixBase<T>;
    //! Base matrix class for matrices using Eigen
    using Common = MatrixEigenCommon<T>;

    //! Assignment operator
    MatrixEigenCommon &operator=(const MatrixEigenCommon &other) {
      this->matrix_instance() = other.matrix_instance();
      return *this;
    }
  };
}
#else
namespace ketcpp::wrapper::matrix {
  constexpr bool MatrixEigenImplemented = false;
}
#endif
