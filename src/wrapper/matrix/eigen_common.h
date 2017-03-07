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

#if __has_include("Eigen/Dense")

#include <type_traits>

#include <Eigen/Dense>

#include "wrapper/matrix/matrix.h"

namespace ketcpp::wrapper::matrix {
  template <typename T> class MatrixEigenCommon : public MatrixBase<T> {
  public:
    typedef Eigen::Ref<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                                     Eigen::StorageOptions::RowMajor>>
        common_matrix_ref;
    typedef Eigen::Ref<const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic,
                                           Eigen::StorageOptions::RowMajor>>
        const_common_matrix_ref;
    virtual common_matrix_ref matrix_instance() = 0;
    virtual const_common_matrix_ref matrix_instance() const = 0;

    using Base = MatrixBase<T>;
    using Common = MatrixEigenCommon<T>;

    MatrixEigenCommon &operator=(const MatrixEigenCommon &other) {
      this->matrix_instance() = other.matrix_instance();
      return *this;
    }
  };
}
#endif
