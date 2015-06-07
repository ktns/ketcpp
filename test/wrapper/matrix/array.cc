/*
 * ketcpp: Quantum chemical toolset made of C++
 * Copyright (C) 2015 Katsuhiko Nishimra
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

#include <bandit/bandit.h>
#include "wrapper/matrix/array.h"
using namespace bandit;
using namespace bandit::Matchers;
using namespace ketcpp::wrapper::matrix;

go_bandit([] {
  describe("MatrixArray", [] {
    it("should not be abstract class", [] {
      std::is_abstract<MatrixArray<float, 3, 3>>::value must_not be_truthy;
    });

    it("should be initialized with list", [] {
      [] {
        MatrixArray<float, 2, 2> array = {1.f, 2.f, 3.f, 4.f};
        MatrixArray<float, 2, 2> array2 = {{1.f, 2.f}, {3.f, 4.f}};
      } must_not throw_exception;
    });
    MatrixArray<float, 3, 2> array = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f};
    auto array2 = array * 2;
    auto array3 = array * 3;
    describe(".rows", [&array] {
      it("should iterate as many times as number of rows", [&array] {
        size_t n = 0;
        for (auto i : array.rows()) {
          ++n must be_lte(3);
        }
        n must equal(3);
      });

      describe(".begin", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().begin()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      describe(".cbegin", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().cbegin()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      describe(".end", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().end()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      describe(".cend", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().cend()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      it("should all row vectors", [&array] {
        float v[3][2] = {{1.f, 2.f}, {3.f, 4.f}, {5.f, 6.f}};
        size_t k = 0;
        for (auto i : array.rows()) {
          std::equal(i.begin(), i.end(), &v[k++][0]) must be_truthy;
        }
      });
    });

    describe(".rows const", [&array2] {
      const auto &array = array2;
      it("should iterate as many times as number of rows", [&array] {
        size_t n = 0;
        for (auto i : array.rows()) {
          ++n must be_lte(3);
        }
        n must equal(3);
      });

      describe(".begin", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().begin()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      describe(".cbegin", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().cbegin()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      describe(".end", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().end()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      describe(".cend", [&array] {
        it("should iterate as many times as number of columns", [&array] {
          size_t n = 0;
          for (float i : array.rows().cend()) {
            ++n must be_lte(2);
          }
          n must equal(2);
        });
      });

      it("should all row vectors", [&array] {
        float v[3][2] = {{2.f, 4.f}, {6.f, 8.f}, {10.f, 12.f}};
        size_t k = 0;
        for (auto i : array.rows()) {
          std::equal(i.begin(), i.end(), &v[k++][0]) must be_truthy;
        }
      });
    });

    describe(".columns", [&array] {
      it("should iterate as many times as number of columns", [&array] {
        size_t n = 0;
        for (auto i : array.columns()) {
          ++n must be_lte(2);
        }
        n must equal(2);
      });

      describe(".begin", [&array] {
        it("should iterate as many times as number of rows", [&array] {
          size_t n = 0;
          for (float i : array.columns().begin()) {
            ++n must be_lte(3);
          }
          n must equal(3);
        });
      });

      describe(".cbegin", [&array] {
        it("should iterate as many times as number of rows", [&array] {
          size_t n = 0;
          for (float i : array.columns().cbegin()) {
            ++n must be_lte(3);
          }
          n must equal(3);
        });
      });

      describe(".end", [&array] {
        it("should iterate as many times as number of rows", [&array] {
          size_t n = 0;
          for (float i : array.columns().end()) {
            ++n must be_lte(3);
          }
          n must equal(3);
        });
      });

      describe(".cend", [&array] {
        it("should iterate as many times as number of rows", [&array] {
          size_t n = 0;
          for (float i : array.columns().cend()) {
            ++n must be_lte(3);
          }
          n must equal(3);
        });
      });

      it("should all column vectors", [&array] {
        float v[2][3] = {{1.f, 3.f, 5.f}, {2.f, 4.f, 6.f}};
        size_t k = 0;
        for (auto i : array.columns()) {
          std::equal(i.begin(), i.end(), &v[k++][0]) must be_truthy;
        }
      });
    });

    describe(".columns const", [&array2] {
      const auto &array = array2;
      it("should iterate as many times as number of columns", [&array] {
        size_t n = 0;
        for (auto i : array.columns()) {
          ++n must be_lte(2);
        }
        n must equal(2);
      });

      describe(".begin", [&array] {
        it("should iterate as many times as number of rows", [&array] {
          size_t n = 0;
          for (float i : array.columns().begin()) {
            ++n must be_lte(3);
          }
          n must equal(3);
        });
      });

      describe(".end", [&array] {
        it("should iterate as many times as number of rows", [&array] {
          size_t n = 0;
          for (float i : array.columns().end()) {
            ++n must be_lte(3);
          }
          n must equal(3);
        });
      });

      it("should all column vectors", [&array] {
        float v[2][3] = {{2.f, 6.f, 10.f}, {4.f, 8.f, 12.f}};
        size_t k = 0;
        for (auto i : array.columns()) {
          std::equal(i.begin(), i.end(), &v[k++][0]) must be_truthy;
        }
      });
    });

    describe("::operator==", [&array, &array2] {
      it("should return true for same matrix", [&array] {
        auto array2 = array;
        (array == array2) must be_truthy;
      });

      it("should false true for different matrix",
         [&array, &array2] { (array == array2) must be_falsy; });
    });

    describe("::operator+=", [&array, &array2, &array3] {
      it("should change elements", [&array, &array2, &array3] {
        auto array4 = array;

        array must equal(array4);
        array4 += array2;
        array4 must equal(array3);
      });
    });

    describe("::operator*=", [&array, &array2] {
      describe("(float)", [&array, &array2] {
        it("should change elements", [&array, &array2] {
          auto array3 = array;
          array3 must equal(array);
          array3 *= 2.0f;
          array3 must equal(array2);
        });
      });
      describe("(unsinged int)", [&array, &array2] {
        it("should change elements", [&array, &array2] {
          auto array3 = array;
          array3 must equal(array);
          array3 *= 2u;
          array3 must equal(array2);
        });
      });
    });
    describe("::operator*", [&array, &array2] {
      describe("(float)", [&array, &array2] {
        it("should return a multiplied matrix", [&array, &array2] {
          auto array3 = array * 2.f;
          array3 must equal(array2);
        });
      });
      describe("(unsinged int)", [&array, &array2] {
        it("should return a multiplied matrix", [&array, &array2] {
          auto array3 = array * 2u;
          array3 must equal(array2);
        });
      });
      describe("(MatrixArray)", [] {
        it("should return a multiplied matrix", [] {
          MatrixArray<float, 3, 2> array1 = {1, 2, 3, 4, 5, 6};
          MatrixArray<float, 2, 3> array2 = {6, 5, 4, 3, 2, 1};
          MatrixArray<float, 3, 3> array3 = {12, 9, 6, 30, 23, 16, 48, 37, 26};
          array1 *array2 must equal(array3);
        });
      });
    });
  });
});
