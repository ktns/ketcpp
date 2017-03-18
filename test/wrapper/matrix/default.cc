/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2015 Katsuhiko Nishimra
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

#include <iterator>
#include <sstream>

#include <bandit/bandit.h>

#include "wrapper/matrix/default.h"
#include "wrapper/matrix/dummy.h"

using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

go_bandit([] {
  describe("make_matrix", [] {
    describe("<float, m,n>", [] {
      describe("(initializer_list)", [] {
        it("should return a matrix with correct size", [] {
          auto matrix = make_matrix<float, 3, 2>({1, 2, 3, 4, 5, 6});
          matrix->get_num_rows() must equal(3);
          matrix->get_num_columns() must equal(2);
        });

        it("should return a matrix with correct contents", [] {
          auto matrix = make_matrix<float, 3, 2>({1, 2, 3, 4, 5, 6});
          std::array<float, 6> array = {{1, 2, 3, 4, 5, 6}};
          AssertThat(matrix, Is().EqualToContainer(array));
        });
        it("should accept too long initializer_list", [] {
          auto matrix = make_matrix<float, 3, 2>({{1, 2, 3, 4, 5, 6, 7}});
          matrix->get_num_rows() must equal(3);
          matrix->get_num_columns() must equal(2);
          std::array<float, 6> array = {{1, 2, 3, 4, 5, 6}};
          AssertThat(matrix, Is().EqualToContainer(array));
        });
      });

      describe("(nested initializer_list)", [] {
        it("should return a matrix with correct size", [] {
          auto matrix = make_matrix<float, 3, 2>({{1, 2}, {3, 4}, {5, 6}});
          matrix->get_num_rows() must equal(3);
          matrix->get_num_columns() must equal(2);
        });
      });
    });

    describe("<float>", [] {
      describe("(nested initializer_list)", [] {
        it("should return a matrix with correct size", [] {
          auto matrix = make_matrix<float>({{1, 2}, {3, 4}, {5, 6}});
          matrix->get_num_rows() must equal(3);
          matrix->get_num_columns() must equal(2);
        });
        it("should return a matrix with correct contents", [] {
          auto matrix = make_matrix<float>({{1, 2}, {3, 4}, {5, 6}});
          std::array<float, 6> array = {{1, 2, 3, 4, 5, 6}};
          AssertThat(matrix, Is().EqualToContainer(array));
        });
      });
    });
  });

  describe("make_zero_matrix", [] {
    describe("(2, 3)", [] {
      it("should return a matrix whose elements are all zero", [] {
        auto mat = make_zero_matrix<float>(2, 3);
        mat->for_each(
            [&mat](size_t i, size_t j) { mat->at(i, j) must equal(0); });
      });
    });
    describe("(2)", [] {
      it("should return a matrix whose elements are all zero", [] {
        auto mat = make_zero_matrix<float>(2);
        mat->for_each(
            [&mat](size_t i, size_t j) { mat->at(i, j) must equal(0); });
      });
    });
  });

  describe("make_symmetric_matrix", [] {
    it("should return a symmetric matrix", [] {
      auto mat = make_symmetric_matrix<float>({{0}, {1, 2}, {3, 4, 5}});
      mat->for_each([&mat](size_t i, size_t j) {
        mat->at(i, j) must equal(mat->at(j, i));
      });
    });
  });

  describe("make_diagonal_matrix", [] {
    auto correct = make_dummy_matrix<float>();
    before_each([&correct] {
      correct = make_matrix<float, 3>({{1, 0, 0}, {0, 2, 0}, {0, 0, 3}});
    });

    it("should return a diagonal matrix", [&correct] {
      auto diag = make_diagonal_matrix<float>({1, 2, 3});
      diag must equal(correct);
    });

    it("should accept forward iterators", [&correct] {
      std::initializer_list<float> list = {1, 2, 3};
      auto diag = make_diagonal_matrix(list.begin(), list.end());
      diag must equal(correct);
    });

    it("should accept input iterators", [&correct] {
      std::stringstream ss("1 2 3");
      std::istream_iterator<float> begin(ss), end;
      auto diag = make_diagonal_matrix(begin, end);
      diag must equal(correct);
    });

    it("should accept iterators from MatrixBase", [&correct] {
      auto vec = make_matrix<float, 1, 3>({1, 2, 3});
      auto diag = make_diagonal_matrix(vec.cbegin(), vec.cend());
      diag must equal(correct);
    });
  });
});
