////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 by Guillaume Chatelet
//
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This code DOES NOT accompany the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_KEY_INC_
#define LOKI_KEY_INC_

// $Id$

#include <tuple>
#include <loki/Factory.h>

namespace Loki {

template <class Factory, typename IdentifierType, typename... Parms> class Key;

template <class F, typename I, typename... Parms>
bool operator==(const Key<F, I, Parms...> &k1, const Key<F, I, Parms...> &k2);

template <class F, typename I, typename... Parms>
bool operator<(const Key<F, I, Parms...> &k1, const Key<F, I, Parms...> &k2);

/**
 * A Key class
 */
template <class Factory, typename IdentifierType, typename... Parms>
class Key {
public:
  // member variables
  IdentifierType id;
  std::tuple<Parms...> values;

  // member functions
  Key() {}

  Key(const IdentifierType &id, Parms... parms) {
    this->id = id;
    values = std::make_tuple(parms...);
  }

  template <class F, typename I, typename... Args>
  friend bool operator==(const Key<F, I, Args...> &k1, const Key<F, I, Args...> &k2);

  template <class F, typename I, typename... Args>
  friend bool operator<(const Key<F, I, Args...> &k1, const Key<F, I, Args...> &k2);
};

template <class F, typename I, typename... Args>
bool operator==(const Key<F, I, Args...> &k1, const Key<F, I, Args...> &k2) {
  if (std::tuple_size<decltype(k1.values)>::value !=
      std::tuple_size<decltype(k2.values)>::value)
    return false;
  return k1.values == k2.values;
}

template <class F, typename I, typename... Args>
bool operator<(const Key<F, I, Args...> &k1, const Key<F, I, Args...> &k2) {
  if (std::tuple_size<decltype(k1.values)>::value <
      std::tuple_size<decltype(k2.values)>::value)
    return true;

  if (std::tuple_size<decltype(k1.values)>::value == 0)
    return true;

  if (k1.id < k2.id)
    return true;

  return k1.values < k2.values;
}

} // namespace Loki

#endif // end file guardian
