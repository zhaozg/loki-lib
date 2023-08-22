////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2005 Peter K黰mel
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
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_FUNCTION_INC_
#define LOKI_FUNCTION_INC_

// $Id$

#define LOKI_ENABLE_FUNCTION

#include <stdexcept>

#include <loki/Functor.h>

namespace Loki {

////////////////////////////////////////////////////////////////////////////////
///  \struct Function
///
///  \ingroup FunctorGroup
///  Allows a boost/TR1 like usage of Functor.
///
///  \par Usage
///
///      - free functions: e.g.  \code Function<int(int,int)> f(&freeFunction);
///                              \endcode
///      - member functions: e.g \code Function<int()>
///      f(&object,&ObjectType::memberFunction);
///                              \endcode
///
///  see also test/Function/FunctionTest.cpp (the modified test program from
///  boost)
////////////////////////////////////////////////////////////////////////////////

template <typename R = void(),
          template <class, class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL>
struct Function;

template <class R,
          template <class, class> class ThreadingModel,
          typename... Parms>
struct Function<R(Parms...), ThreadingModel> : public Functor<R, ThreadingModel, Parms...> {
  typedef Functor<R, ThreadingModel, Parms...> FBase;

  Function() : FBase() {}

  Function(const Function &func) : FBase() {
    if (!func.empty())
      FBase::operator=(func);
  }

  // test on emptiness
  template <class R2> Function(Function<R2()> func) : FBase() {
    if (!func.empty())
      FBase::operator=(func);
  }

  // clear  by '= 0'
  Function(const int i) : FBase() {
    if (i == 0)
      FBase::clear();
    else
      throw std::runtime_error("Loki::Function(const int i): i!=0");
  }

  template <class Func> Function(Func func) : FBase(func) {}

  template <class Host, class Func>
  Function(const Host &host, const Func &func) : FBase(host, func) {}
};

////////////////////////////////////////////////////////////////////////////////
// macros for the repetitions
////////////////////////////////////////////////////////////////////////////////

#define LOKI_FUNCTION_BODY                                                     \
                                                                               \
  Function() : FBase() {}                                                      \
                                                                               \
  Function(const Function &func) : FBase() {                                   \
    if (!func.empty())                                                         \
      FBase::operator=(func);                                                  \
  }                                                                            \
                                                                               \
  Function(const int i) : FBase() {                                            \
    if (i == 0)                                                                \
      FBase::clear();                                                          \
    else                                                                       \
      throw std::runtime_error("Loki::Function(const int i): i!=0");           \
  }                                                                            \
                                                                               \
  template <class Func> Function(Func func) : FBase(func) {}                   \
                                                                               \
  template <class Host, class Func>                                            \
  Function(const Host &host, const Func &func) : FBase(host, func) {}

#define LOKI_FUNCTION_R2_CTOR_BODY                                             \
                                                                               \
        : FBase()                           \
        {                                                                      \
    if (!func.empty())                                                         \
      FBase::operator=(func);                                                  \
  }

////////////////////////////////////////////////////////////////////////////////
// repetitions
////////////////////////////////////////////////////////////////////////////////

template <class R,
          typename... Parms>
struct Function<R(Parms...), LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL> : public Loki::Functor<R, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, Parms...> {
  typedef Functor<R, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, Parms...> FBase;

  template <class R2, class... Args2>
  Function(Function<R2(Args2...)> func) LOKI_FUNCTION_R2_CTOR_BODY

      LOKI_FUNCTION_BODY // if compilation breaks here then
                         // Function.h was not included before
                         // Functor.h, check your include order
                         // or define LOKI_ENABLE_FUNCTION
};

} // namespace Loki

#endif // end file guardian
