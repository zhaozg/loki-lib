////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
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
////////////////////////////////////////////////////////////////////////////////

// $Id$

#include <loki/Singleton.h>

Loki::Private::TrackerArray Loki::Private::pTrackerArray = 0;
unsigned int Loki::Private::elements = 0;

////////////////////////////////////////////////////////////////////////////////
// function AtExitFn
// Ensures proper destruction of objects with longevity
////////////////////////////////////////////////////////////////////////////////

void LOKI_C_CALLING_CONVENTION_QUALIFIER Loki::Private::AtExitFn() {
  assert(elements > 0 && pTrackerArray != 0);
  // Pick the element at the top of the stack
  LifetimeTracker *pTop = pTrackerArray[elements - 1];
  // Remove that object off the stack
  // Don't check errors - realloc with less memory
  //     can't fail
  pTrackerArray = static_cast<TrackerArray>(
      std::realloc(pTrackerArray, sizeof(*pTrackerArray) * --elements));
  // Destroy the element
  delete pTop;
}
