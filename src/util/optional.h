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

#if __has_include(<optional>)
#include <optional>
#else
#include <experimental/optional>
#endif

//! Utility classes
namespace ketcpp::util {
#if __has_include(<optional>)
  //! tmplate alias for std::optional
  template <typename T> using optional = std::optional<T>;
#else
  //! Override std::experimental::optional and add has_value method
  template <typename T>
  struct optional : public std::experimental::optional<T> {
    using std::experimental::optional<T>::optional;
    bool has_value() { return static_cast<bool>(*this); }
  };
#endif
}
