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

#include "config/ketcpp_config.h"

#ifdef EIGEN3_FOUND

#include <bandit/bandit.h>

#include "wrapper/matrix/array.h"
#include "wrapper/matrix/dummy.h"
#include "wrapper/matrix/eigen.h"
#include "wrapper/matrix/vector.h"
using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

static MatrixEigen<float> eigen_stat = {{1.f, 2.f}, {3.f, 4.f}, {5.f, 6.f}};

go_bandit([] {
  describe("MatrixEigen(dynamically sized)", [] {
    MatrixEigen<float> matrix1(3, 2), matrix2(3, 2), matrix3(3, 2);

    before_each([&] {
      MatrixEigen<float> eigen = {{1.f, 2.f}, {3.f, 4.f}, {5.f, 6.f}};
      matrix1 = matrix2 = matrix3 = eigen;
      matrix2 *= 2;
      matrix3 *= 3;
    });

    it("should not be abstract class",
       [] { std::is_abstract<MatrixEigen<float>>::value must_not be_truthy; });

    it("should be initialized with nested list", [] {
      [] {
        MatrixEigen<float> matrix2 = {{1.f, 2.f}, {3.f, 4.f}};
      } must_not throw_exception;
    });

    it("should be correctly initialized with nested list", [] {
      MatrixEigen<float, 3, 2> matrix = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
      MatrixEigen<float> matrix2 = {{1.f, 2.f}, {3.f, 4.f}, {5.f, 6.f}};
      matrix must equal(matrix2);
    });

    describe("copy ctor", [&matrix1] {
      it("should copy a matrix", [&matrix1] {
        MatrixEigen<float, 3, 2> matrix2(matrix1);
        matrix2 must equal(matrix1);
      });
    });

    describe(".get_num_rows", [&matrix1] {
      it("should return the correct number of rows",
         [&matrix1] { matrix1.get_num_rows() must equal(3u); });
    });
    describe(".get_num_columns", [&matrix1] {
      it("should return the correct number of columns",
         [&matrix1] { matrix1.get_num_columns() must equal(2u); });
    });
    describe(".get_row_size", [&matrix1] {
      it("should return the correct size of rows",
         [&matrix1] { matrix1.get_row_size() must equal(2u); });
    });
    describe(".get_column_size", [&matrix1] {
      it("should return the correct size of columns",
         [&matrix1] { matrix1.get_column_size() must equal(3u); });
    });

    describe(".at", [&matrix1] {
      it("should return correct element", [&matrix1] {
        matrix1.at(0, 0) must equal(1);
        matrix1.at(0, 1) must equal(2);
        matrix1.at(1, 0) must equal(3);
        matrix1.at(1, 1) must equal(4);
        matrix1.at(2, 0) must equal(5);
        matrix1.at(2, 1) must equal(6);
      });

      it("should return assignable reference", [&matrix1] {
        matrix1.at(0, 0) = 0;
        matrix1.at(0, 1) = 0;
        matrix1.at(1, 0) = 0;
        matrix1.at(1, 1) = 0;
        matrix1.at(2, 0) = 0;
        matrix1.at(2, 1) = 0;
        matrix1.at(0, 0) must equal(0);
        matrix1.at(0, 1) must equal(0);
        matrix1.at(1, 0) must equal(0);
        matrix1.at(1, 1) must equal(0);
        matrix1.at(2, 0) must equal(0);
        matrix1.at(2, 1) must equal(0);
      });

      it("should be invoked from const reference", [&matrix1] {
        const auto &matrix2 = matrix1;
        matrix2.at(0, 0) must equal(1);
        matrix2.at(0, 1) must equal(2);
        matrix2.at(1, 0) must equal(3);
        matrix2.at(1, 1) must equal(4);
        matrix2.at(2, 0) must equal(5);
        matrix2.at(2, 1) must equal(6);
      });
    });

    describe(".cbegin()/.cend()", [&matrix1] {
      it("should iterate correct times", [&matrix1] {
        std::distance(matrix1.cbegin(), matrix1.cend())
            must equal(matrix1.size());
      });
      it("should return not assignable iterator", [] {
        std::is_assignable<decltype(*matrix1.cbegin()), float>::value must
            be_falsy;
      });
    });

    describe(".begin()/.end() const", [&matrix1] {
      it("should iterate correct times", [&matrix1] {
        const auto matrix2 = matrix1;
        std::distance(matrix2.begin(), matrix2.end())
            must equal(matrix2.size());
      });
      it("should return not assignable iterator", [&matrix1] {
        const auto matrix2 = matrix1;
        std::is_assignable<decltype(*matrix2.begin()), float>::value must
            be_falsy;
      });
    });

    describe(".begin()/.end()", [&matrix1] {
      it("should iterate correct times", [&matrix1] {
        std::distance(matrix1.begin(), matrix1.end())
            must equal(matrix1.size());
      });
      it("should return assignable iterator", [&matrix1] {
        std::is_assignable<decltype(*matrix1.begin()), float>::value must
            be_truthy;
        std::fill(matrix1.begin(), matrix1.end(), 0);
        std::count(matrix1.cbegin(), matrix1.cend(), 0)
            must equal(matrix1.size());
      });
    });

    describe("::operator==", [&matrix1, &matrix2] {
      it("should return true for same matrices", [&matrix1] {
        auto matrix2 = matrix1;
        (matrix1 == matrix2) must be_truthy;
      });
      it("should return false for different matrices", [&matrix1, &matrix2] {
        matrix1 must_not equal(matrix2);
        (matrix1 == matrix2) must be_falsy;
      });
      it("should be comparable with MatrixArrayCore", [&matrix1, &matrix2] {
        MatrixArrayCore<float, 3, 2> array = {1, 2, 3, 4, 5, 6};
        (matrix1 == array) must be_truthy;
        (array == matrix1) must be_truthy;
        (matrix2 == array) must be_falsy;
        (array == matrix2) must be_falsy;
      });
      it("should be comparable with MatrixVector", [&matrix1, &matrix2] {
        MatrixVector<float> vector = {{1, 2}, {3, 4}, {5, 6}};
        (matrix1 == vector) must be_truthy;
        (vector == matrix1) must be_truthy;
        (matrix2 == vector) must be_falsy;
        (vector == matrix2) must be_falsy;
      });
      it("should be comparable with MatrixEigen<fixed size>",
         [&matrix1, &matrix2] {
           MatrixEigen<float, 3, 2> eigen_c = {{1, 2}, {3, 4}, {5, 6}};
           (matrix1 == eigen_c) must be_truthy;
           (eigen_c == matrix1) must be_truthy;
           (matrix2 == eigen_c) must be_falsy;
           (eigen_c == matrix2) must be_falsy;
         });

      describe("(Matrix<float>)", [&matrix1, matrix2] {
        it("should work", [&matrix1, matrix2] {
          Matrix<float> matrix3 = matrix1;
          (matrix1 == matrix3) must be_truthy;
          (matrix2 == matrix3) must be_falsy;
        });
      });
    });

    describe("::operator!=", [&matrix1, &matrix2] {
      it("should return false for same matrices", [&matrix1] {
        auto matrix2 = matrix1;
        matrix1 must equal(matrix2);
        (matrix1 != matrix2) must be_falsy;
      });
      it("should return true for different matrices", [&matrix1, &matrix2] {
        matrix1 must_not equal(matrix2);
        (matrix1 != matrix2) must be_truthy;
      });
      it("should be comparable with MatrixArrayCore", [&matrix1, &matrix2] {
        MatrixArrayCore<float, 3, 2> array = {{1, 2, 3, 4, 5, 6}};
        (matrix1 != array) must be_falsy;
        (array != matrix1) must be_falsy;
        (matrix2 != array) must be_truthy;
        (array != matrix2) must be_truthy;
      });
      it("should be comparable with MatrixVector", [&matrix1, &matrix2] {
        MatrixVector<float> vector = {{1, 2}, {3, 4}, {5, 6}};
        (matrix1 != vector) must be_falsy;
        (vector != matrix1) must be_falsy;
        (matrix2 != vector) must be_truthy;
        (vector != matrix2) must be_truthy;
      });
      it("should be comparable with MatrixEigen<fixed size>",
         [&matrix1, &matrix2] {
           MatrixEigen<float, 3, 2> eigen_c = {{1, 2}, {3, 4}, {5, 6}};
           (matrix1 != eigen_c) must be_falsy;
           (eigen_c != matrix1) must be_falsy;
           (matrix2 != eigen_c) must be_truthy;
           (eigen_c != matrix2) must be_truthy;
         });

      describe("(Matrix<float>)", [&matrix1, matrix2] {
        it("should work", [&matrix1, matrix2] {
          Matrix<float> matrix3 = matrix1;
          (matrix1 != matrix3) must be_falsy;
          (matrix2 != matrix3) must be_truthy;
        });
      });
    });

    describe("::operator+=", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix1;
        matrix4 must equal(matrix1);
        matrix4 must_not equal(matrix3);
        matrix4 += matrix2;
        matrix4 must_not equal(matrix1);
        matrix4 must equal(matrix3);
      });
      it("should work with MatrixArrayCore", [&matrix1] {
        MatrixArrayCore<float, 3, 2> array = {6, 5, 4, 3, 2, 1},
                                     array2 = {7, 7, 7, 7, 7, 7};
        matrix1 must_not equal(array2);
        matrix1 += array;
        matrix1 must equal(array2);
      });
      it("should work with MatrixArray", [&matrix1] {
        MatrixArray<float, 3, 2> array = {6, 5, 4, 3, 2, 1},
                                 array2 = {7, 7, 7, 7, 7, 7};
        matrix1 must_not equal(array2);
        matrix1 += array;
        matrix1 must equal(array2);
      });
      it("should work with MatrixVector", [&matrix1] {
        MatrixVector<float> vector1 = {{6, 5}, {4, 3}, {2, 1}},
                            vector2 = {{7, 7}, {7, 7}, {7, 7}};
        matrix1 must_not equal(vector2);
        matrix1 += vector1;
        matrix1 must equal(vector2);
      });
      it("should work with MatrixEigen<fixed size>", [&matrix1] {
        MatrixEigen<float, 3, 2> eigen_c1 = {{6, 5}, {4, 3}, {2, 1}},
                                 eigen_c2 = {{7, 7}, {7, 7}, {7, 7}};
        matrix1 must_not equal(eigen_c2);
        matrix1 += eigen_c1;
        matrix1 must equal(eigen_c2);
      });
    });

    describe("::operator-=", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix3;

        matrix4 must equal(matrix3);
        matrix4 must_not equal(matrix2);
        matrix4 -= matrix1;
        matrix4 must_not equal(matrix3);
        matrix4 must equal(matrix2);
      });
      it("should work with MatrixArray", [&matrix1] {
        MatrixArray<float, 3, 2> array = {6, 5, 4, 3, 2, 1},
                                 array2 = {-5, -3, -1, 1, 3, 5};
        matrix1 must_not equal(array2);
        matrix1 -= array;
        matrix1 must equal(array2);
      });
      it("should work with MatrixVector", [&matrix1] {
        MatrixVector<float> vector1 = {{6, 5}, {4, 3}, {2, 1}},
                            vector2 = {{-5, -3}, {-1, 1}, {3, 5}};
        matrix1 must_not equal(vector2);
        matrix1 -= vector1;
        matrix1 must equal(vector2);
      });
      it("should work with MatrixEigen<fixed size>", [&matrix1] {
        MatrixEigen<float, 3, 2> eigen_c = {6, 5, 4, 3, 2, 1},
                                 eigen_c2 = {-5, -3, -1, 1, 3, 5};
        matrix1 must_not equal(eigen_c2);
        matrix1 -= eigen_c;
        matrix1 must equal(eigen_c2);
      });
    });

    describe("::operator*=", [&matrix1, &matrix2] {
      describe("(float)", [&matrix1, &matrix2] {
        it("should change elements", [&matrix1, &matrix2] {
          auto matrix3 = matrix1;
          matrix3 must equal(matrix1);
          matrix3 must_not equal(matrix2);
          matrix3 *= 2.0f;
          matrix3 must_not equal(matrix1);
          matrix3 must equal(matrix2);
        });
      });
      describe("(unsinged int)", [&matrix1, &matrix2] {
        it("should change elements", [&matrix1, &matrix2] {
          auto matrix3 = matrix1;
          matrix3 must equal(matrix1);
          matrix3 must_not equal(matrix2);
          matrix3 *= 2u;
          matrix3 must_not equal(matrix1);
          matrix3 must equal(matrix2);
        });
      });
    });

    describe("::operator/=", [&matrix1, &matrix2] {
      describe("(float)", [&matrix1, &matrix2] {
        it("should change elements", [&matrix1, &matrix2] {
          auto matrix3 = matrix2;
          matrix3 must_not equal(matrix1);
          matrix3 must equal(matrix2);
          matrix3 /= 2.0f;
          matrix3 must equal(matrix1);
          matrix3 must_not equal(matrix2);
        });
      });
      describe("(unsinged int)", [&matrix1, &matrix2] {
        it("should change elements", [&matrix1, &matrix2] {
          auto matrix3 = matrix2;
          matrix3 must_not equal(matrix1);
          matrix3 must equal(matrix2);
          matrix3 /= 2u;
          matrix3 must equal(matrix1);
          matrix3 must_not equal(matrix2);
        });
      });
    });

    describe("::operator+", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix1 + matrix2;
        matrix4 must equal(matrix3);
        matrix1 must_not equal(matrix3);
      });
    });

    describe("::operator-", [&matrix1, &matrix2, &matrix3] {
      it("should change elements", [&matrix1, &matrix2, &matrix3] {
        auto matrix4 = matrix3 - matrix1;
        matrix4 must equal(matrix2);
        matrix3 must_not equal(matrix2);
      });
    });

    describe("::operator*", [&matrix1, &matrix2] {
      describe("(float)", [&matrix1, &matrix2] {
        it("should return a matrix multiplied by the scalar",
           [&matrix1, &matrix2] {
             auto matrix3 = matrix1 * 2.f;
             matrix3 must equal(matrix2);
           });
        it("should not change elenments", [&matrix1] {
          auto matrix2 = matrix1;
          auto matrix3 = matrix1 * 2.f;
          matrix1 must equal(matrix2);
        });
      });
      describe("(unsinged int)", [&matrix1, &matrix2] {
        it("should return a matrix multiplied by the scalar",
           [&matrix1, &matrix2] {
             auto matrix3 = matrix1 * 2u;
             matrix3 must equal(matrix2);
           });
        it("should not change elenments", [&matrix1] {
          auto matrix2 = matrix1;
          auto matrix3 = matrix1 * 2u;
          matrix1 must equal(matrix2);
        });
      });

      describe("(Matrix)", [] {
        auto matrix_a = make_dummy_matrix<float>(), matrix_b = matrix_a,
             matrix_c = matrix_a, matrix_d = matrix_a;
        before_each([&matrix_a, &matrix_b, &matrix_c, &matrix_d] {
          matrix_a = MatrixEigen<float>({{-1, 2}, {3, -1}, {0, 3}});
          matrix_b = MatrixEigen<float>({{1, 1, 3}, {-1, 1, 0}});
          matrix_c = MatrixEigen<float>({{-3, 1, -3}, {4, 2, 9}, {-3, 3, 0}});
          matrix_d = MatrixEigen<float>({{2, 10}, {4, -3}});
        });
        it("should return a correct matrix product",
           [&matrix_a, &matrix_b, &matrix_c, &matrix_d] {
             (matrix_a * matrix_b) must equal(matrix_c);
             (matrix_b * matrix_a) must equal(matrix_d);
           });
        it("should return a correct matrix product for transposed matrices",
           [&matrix_a, &matrix_b, &matrix_c, &matrix_d] {
             (matrix_a->transpose() * matrix_b->transpose())
                 must equal(matrix_d->transpose());
             (matrix_b->transpose() * matrix_a->transpose())
                 must equal(matrix_c->transpose());
           });
        it("should not change elements", [&matrix_a, &matrix_b] {
          auto matrix_a2 = matrix_a;
          auto matrix_b2 = matrix_b;
          matrix_a must equal(matrix_a2);
          matrix_b must equal(matrix_b2);
          matrix_a *matrix_b;
          matrix_a must equal(matrix_a2);
          matrix_b must equal(matrix_b2);
          matrix_a *matrix_b;
          matrix_a must equal(matrix_a2);
          matrix_b must equal(matrix_b2);
        });
      });
    });

    describe("::operator/", [&matrix1, &matrix2] {
      describe("(float)", [&matrix1, &matrix2] {
        it("should return a matrix divided by the scalar",
           [&matrix1, &matrix2] {
             auto matrix3 = matrix2 / 2.f;
             matrix3 must equal(matrix1);
           });
        it("should not change elenments", [&matrix1] {
          auto matrix2 = matrix1;
          auto matrix3 = matrix1 / 2.f;
          matrix1 must equal(matrix2);
        });
      });

      describe("(unsinged int)", [&matrix1, &matrix2] {
        it("should return a matrix divided by the scalar",
           [&matrix1, &matrix2] {
             auto matrix3 = matrix2 / 2u;
             matrix3 must equal(matrix1);
           });
        it("should not change elenments", [&matrix1] {
          auto matrix2 = matrix1;
          auto matrix3 = matrix1 / 2.f;
          matrix1 must equal(matrix2);
        });
      });
    });

    describe("transpose()", [&matrix1] {
      auto matrix2 = make_dummy_matrix<float>();

      before_each([&matrix1, &matrix2] { matrix2 = matrix1.transpose(); });

      it("should correctly transpose elements of the matrix",
         [&matrix1, &matrix2] {
           matrix1.for_each([&matrix1, &matrix2](size_t i, size_t j) {
             matrix2->at(j, i) must equal(matrix1.at(i, j));
           });
           const auto matrix3 = matrix2;
           matrix1.for_each([&matrix1, &matrix3](size_t i, size_t j) {
             matrix3->at(j, i) must equal(matrix1.at(i, j));
           });
         });

      it("should change the dimension of the matrix correctly",
         [&matrix1, &matrix2] {
           matrix1.get_num_rows() must equal(matrix2->get_num_columns());
           matrix1.get_num_columns() must equal(matrix2->get_num_rows());
           matrix1.get_row_size() must equal(matrix2->get_column_size());
           matrix1.get_column_size() must equal(matrix2->get_row_size());
         });
    });

    it("should comparable as a container", [&] {
      std::array<float, 6> array1 = {{1, 2, 3, 4, 5, 6}},
                           array2 = {{2, 4, 6, 8, 10, 12}},
                           array3 = {{3, 6, 9, 12, 15, 18}};
      AssertThat(matrix1, Is().EqualToContainer(array1));
      AssertThat(matrix2, Is().Not().EqualToContainer(array1));
      AssertThat(matrix3, Is().Not().EqualToContainer(array1));
      AssertThat(matrix1, Is().Not().EqualToContainer(array2));
      AssertThat(matrix2, Is().EqualToContainer(array2));
      AssertThat(matrix3, Is().Not().EqualToContainer(array2));
      AssertThat(matrix1, Is().Not().EqualToContainer(array3));
      AssertThat(matrix2, Is().Not().EqualToContainer(array3));
      AssertThat(matrix3, Is().EqualToContainer(array3));
    });
  });
});
#endif
