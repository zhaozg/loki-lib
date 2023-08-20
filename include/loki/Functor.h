////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
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
#ifndef LOKI_FUNCTOR_INC_
#define LOKI_FUNCTOR_INC_

// $Id$

#include "loki/NullType.h"
#include <loki/EmptyType.h>
#include <loki/Sequence.h>
#include <loki/SmallObj.h>
#include <loki/TypeTraits.h>
#include <loki/Typelist.h>
#include <memory>
#include <typeinfo>

///  \defgroup FunctorGroup Function objects

#ifndef LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT
// #define LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT
#endif

#ifndef LOKI_FUNCTORS_ARE_COMPARABLE
// #define LOKI_FUNCTORS_ARE_COMPARABLE
#endif

/// \namespace Loki
/// All classes of Loki are in the Loki namespace
namespace Loki {
////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl (internal)
////////////////////////////////////////////////////////////////////////////////

namespace Private {
template <typename R, template <class, class> class ThreadingModel, typename... Parms>
struct FunctorImplBase
#ifdef LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT
{
#else
    : public SmallValueObject<ThreadingModel> {
  inline FunctorImplBase() : SmallValueObject<ThreadingModel>() {}
  inline FunctorImplBase(const FunctorImplBase &)
      : SmallValueObject<ThreadingModel>() {}
#endif

  typedef R ResultType;
  typedef FunctorImplBase<R, ThreadingModel, Parms...> FunctorImplBaseType;

  virtual ~FunctorImplBase() {}

  virtual FunctorImplBase *DoClone() const = 0;

  template <class U> static U *Clone(U *pObj) {
    if (!pObj)
      return 0;
    U *pClone = static_cast<U *>(pObj->DoClone());
    assert(typeid(*pClone) == typeid(*pObj));
    return pClone;
  }

#ifdef LOKI_FUNCTORS_ARE_COMPARABLE

  virtual bool operator==(const FunctorImplBase &) const = 0;

#endif
};
} // namespace Private

////////////////////////////////////////////////////////////////////////////////
// macro LOKI_DEFINE_CLONE_FUNCTORIMPL
// Implements the DoClone function for a functor implementation
////////////////////////////////////////////////////////////////////////////////

#define LOKI_DEFINE_CLONE_FUNCTORIMPL(Cls)                                     \
  virtual Cls *DoClone() const { return new Cls(*this); }

////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl
// The base class for a hierarchy of functors. The FunctorImpl class is not used
//     directly; rather, the Functor class manages and forwards to a pointer to
//     FunctorImpl
// You may want to derive your own functors from FunctorImpl.
// Specializations of FunctorImpl for up to 15 parameters follow
////////////////////////////////////////////////////////////////////////////////

template <typename R, template <class, class> class ThreadingModel, typename... Parms>
class FunctorImpl;

////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl
////////////////////////////////////////////////////////////////////////////////

template <typename R, template <class, class> class ThreadingModel, typename... Parms>
class FunctorImpl
    : public Private::FunctorImplBase<R, ThreadingModel, Parms...> {
public:
  typedef R ResultType;
  virtual R operator()(Parms...parms) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// class template FunctorHandler
// Wraps functors and pointers to functions
////////////////////////////////////////////////////////////////////////////////

template <class ParentFunctor, typename Fun, typename...Parms>
class FunctorHandler : public ParentFunctor::Impl {
  typedef typename ParentFunctor::Impl Base;

public:
  typedef typename Base::ResultType ResultType;

  FunctorHandler(const Fun &fun) : f_(fun) {}

  LOKI_DEFINE_CLONE_FUNCTORIMPL(FunctorHandler)

#ifdef LOKI_FUNCTORS_ARE_COMPARABLE

  bool operator==(const typename Base::FunctorImplBaseType &rhs) const {
    // there is no static information if Functor holds a member function
    // or a free function; this is the main difference to tr1::function
    if (typeid(*this) != typeid(rhs))
      return false; // cannot be equal

    const FunctorHandler &fh = static_cast<const FunctorHandler &>(rhs);
    // if this line gives a compiler error, you are using a function object.
    // you need to implement bool MyFnObj::operator == (const MyFnObj&) const;
    return f_ == fh.f_;
  }
#endif
  // operator() implementations for up to 15 arguments

  ResultType operator()(Parms...parms) { return f_(parms...); }

private:
  Fun f_;
};

////////////////////////////////////////////////////////////////////////////////
// class template FunctorHandler
// Wraps pointers to member functions
////////////////////////////////////////////////////////////////////////////////

template <class ParentFunctor,
          typename PointerToObj,
          typename PointerToMemFn,
          typename... Parms>
class MemFunHandler : public ParentFunctor::Impl {
  typedef typename ParentFunctor::Impl Base;

public:
  typedef typename Base::ResultType ResultType;

  MemFunHandler(const PointerToObj &pObj, PointerToMemFn pMemFn)
      : pObj_(pObj), pMemFn_(pMemFn) {}

  LOKI_DEFINE_CLONE_FUNCTORIMPL(MemFunHandler)

#ifdef LOKI_FUNCTORS_ARE_COMPARABLE

  bool operator==(const typename Base::FunctorImplBaseType &rhs) const {
    if (typeid(*this) != typeid(rhs))
      return false; // cannot be equal

    const MemFunHandler &mfh = static_cast<const MemFunHandler &>(rhs);
    // if this line gives a compiler error, you are using a function object.
    // you need to implement bool MyFnObj::operator == (const MyFnObj&) const;
    return pObj_ == mfh.pObj_ && pMemFn_ == mfh.pMemFn_;
  }
#endif

  ResultType operator()(Parms...parms) { return ((*pObj_).*pMemFn_)(parms...); }

private:
  PointerToObj pObj_;
  PointerToMemFn pMemFn_;
};

////////////////////////////////////////////////////////////////////////////////
// TR1 exception
//////////////////////////////////////////////////////////////////////////////////

#ifdef LOKI_ENABLE_FUNCTION

#define LOKI_FUNCTION_THROW_BAD_FUNCTION_CALL                                  \
  if (empty())                                                                 \
    throw std::bad_function_call();

#else

#define LOKI_FUNCTION_THROW_BAD_FUNCTION_CALL

#endif

////////////////////////////////////////////////////////////////////////////////
///  \class Functor
///
///  \ingroup FunctorGroup
///  A generalized functor implementation with value semantics
///
/// \par Macro: LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT
/// Define
/// \code LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT \endcode
/// to avoid static instantiation/delete
/// order problems.
/// It often helps against crashes when using static Functors and multi
/// threading. Defining also removes problems when unloading Dlls which hosts
/// static Functor objects.
///
/// \par Macro: LOKI_FUNCTORS_ARE_COMPARABLE
/// To enable the operator== define the macro
/// \code LOKI_FUNCTORS_ARE_COMPARABLE \endcode
/// The macro is disabled by default, because it breaks compiling functor
/// objects  which have no operator== implemented, keep in mind when you enable
/// operator==.
////////////////////////////////////////////////////////////////////////////////
template <typename R,
          template <class, class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
          typename... Parms>
class Functor {
public:
  // Handy type definitions for the body type
  typedef FunctorImpl<R, ThreadingModel, Parms...> Impl;
  typedef R ResultType;

  // Member functions

  Functor() {}

  Functor(const Functor &rhs) : spImpl_(Impl::Clone(rhs.spImpl_.get())) {}

  Functor(Impl *spImpl) { spImpl_ = spImpl; }

  Functor(std::unique_ptr<Impl> &spImpl) { spImpl_.swap(spImpl); }

  template <typename Fun>
  Functor(Fun fun) : spImpl_(new FunctorHandler<Functor, Fun, Parms...>(fun)) {}

  template <class PtrObj, typename MemFn>
  Functor(const PtrObj &p, MemFn memFn)
      : spImpl_(new MemFunHandler<Functor, PtrObj, MemFn, Parms...>(p, memFn)) {}

  typedef Impl *(std::unique_ptr<Impl>::*unspecified_bool_type)() const;

  operator unspecified_bool_type() const {
    return spImpl_.get() ? &std::unique_ptr<Impl>::get : 0;
  }

  Functor &operator=(const Functor &rhs) {
    this->spImpl_.reset(Impl::Clone(rhs.spImpl_.get()));
    return *this;
  }

#ifdef LOKI_ENABLE_FUNCTION

  bool empty() const { return spImpl_.get() == 0; }

  void clear() { spImpl_.reset(0); }
#endif

#ifdef LOKI_FUNCTORS_ARE_COMPARABLE

  bool operator==(const Functor &rhs) const {
    if (spImpl_.get() == 0 && rhs.spImpl_.get() == 0)
      return true;
    if (spImpl_.get() != 0 && rhs.spImpl_.get() != 0)
      return *spImpl_.get() == *rhs.spImpl_.get();
    else
      return false;
  }

  bool operator!=(const Functor &rhs) const { return !(*this == rhs); }
#endif

  // operator() implementations for up to 15 arguments

  ResultType operator()(Parms...parms) const {
    LOKI_FUNCTION_THROW_BAD_FUNCTION_CALL
    return (*spImpl_)(parms...);
  }

private:
  std::unique_ptr<Impl> spImpl_;
};

////////////////////////////////////////////////////////////////////////////////
//
//  BindersFirst and Chainer
//
////////////////////////////////////////////////////////////////////////////////

namespace Private {
template <class Fctor> struct BinderFirstTraits;

template <typename R, template <class, class> class ThreadingModel, typename... Parms>
struct BinderFirstTraits<Functor<R, ThreadingModel, Parms...>> {

  using OriginalFunctor =  Functor<R, ThreadingModel, Parms...> ;
  using ArgsList = Typelist<Parms...>;
  using ParmList = Typelist<Parms...>;
  using BoundFunctorType = Functor<R, ThreadingModel, Parms...>;
  using Impl = typename BoundFunctorType::Impl;
};

template <class T> struct BinderFirstBoundTypeStorage;

template <class T> struct BinderFirstBoundTypeStorage {
  typedef typename TypeTraits<T>::ParameterType RefOrValue;
};

template <typename R, template <class, class> class ThreadingModel, typename... Parms>
struct BinderFirstBoundTypeStorage<Functor<R, ThreadingModel, Parms...>> {
  typedef Functor<R, ThreadingModel, Parms...> OriginalFunctor;
  typedef const typename TypeTraits<OriginalFunctor>::ReferredType RefOrValue;
};

} // namespace Private

////////////////////////////////////////////////////////////////////////////////
///  \class BinderFirst
///
///  \ingroup FunctorGroup
///  Binds the first parameter of a Functor object to a specific value
////////////////////////////////////////////////////////////////////////////////

template <class OriginalFunctor, typename... Parms>
class BinderFirst : public Private::BinderFirstTraits<OriginalFunctor>::Impl {
  typedef typename Private::BinderFirstTraits<OriginalFunctor>::Impl Base;
  typedef typename OriginalFunctor::ResultType ResultType;

  typedef typename OriginalFunctor::Parm1 BoundType;

  typedef typename Private::BinderFirstBoundTypeStorage<
      typename Private::BinderFirstTraits<OriginalFunctor>::OriginalParm1>::
      RefOrValue BoundTypeStorage;

public:
  BinderFirst(const OriginalFunctor &fun, BoundType bound)
      : f_(fun), b_(bound) {}

  LOKI_DEFINE_CLONE_FUNCTORIMPL(BinderFirst)

#ifdef LOKI_FUNCTORS_ARE_COMPARABLE

  bool operator==(const typename Base::FunctorImplBaseType &rhs) const {
    if (typeid(*this) != typeid(rhs))
      return false; // cannot be equal
    // if this line gives a compiler error, you are using a function object.
    // you need to implement bool MyFnObj::operator == (const MyFnObj&) const;
    return f_ == ((static_cast<const BinderFirst &>(rhs)).f_) &&
           b_ == ((static_cast<const BinderFirst &>(rhs)).b_);
  }
#endif

  // operator() implementations for up to 15 arguments

  ResultType operator()(Parms... parms) { return f_(b_, parms...); }

private:
  OriginalFunctor f_;
  BoundTypeStorage b_;
};

////////////////////////////////////////////////////////////////////////////////
///  Binds the first parameter of a Functor object to a specific value
///  \ingroup FunctorGroup
////////////////////////////////////////////////////////////////////////////////

template <class Fctor>
typename Private::BinderFirstTraits<Fctor>::BoundFunctorType
BindFirst(const Fctor &fun, typename Fctor::Parm1 bound) {
  typedef typename Private::BinderFirstTraits<Fctor>::BoundFunctorType Outgoing;
  return Outgoing(BinderFirst<Fctor>(fun, bound));
}

////////////////////////////////////////////////////////////////////////////////
///  \class Chainer
///
///  \ingroup FunctorGroup
///   Chains two functor calls one after another
////////////////////////////////////////////////////////////////////////////////

template <typename Fun1,
          typename Fun2,
          typename... Parms> class Chainer : public Fun2::Impl {
  typedef Fun2 Base;

public:
  typedef typename Base::ResultType ResultType;

  Chainer(const Fun1 &fun1, const Fun2 &fun2) : f1_(fun1), f2_(fun2) {}

  LOKI_DEFINE_CLONE_FUNCTORIMPL(Chainer)

#ifdef LOKI_FUNCTORS_ARE_COMPARABLE

  bool operator==(const typename Base::Impl::FunctorImplBaseType &rhs) const {
    if (typeid(*this) != typeid(rhs))
      return false; // cannot be equal
    // if this line gives a compiler error, you are using a function object.
    // you need to implement bool MyFnObj::operator == (const MyFnObj&) const;
    return f1_ == ((static_cast<const Chainer &>(rhs)).f2_) &&
           f2_ == ((static_cast<const Chainer &>(rhs)).f1_);
  }
#endif

  // operator() implementations for up to 15 arguments

  ResultType operator()(Parms... parms) { return f1_(parms...), f2_(parms...); }

private:
  Fun1 f1_;
  Fun2 f2_;
};

////////////////////////////////////////////////////////////////////////////////
///  Chains two functor calls one after another
///  \ingroup FunctorGroup
////////////////////////////////////////////////////////////////////////////////

template <class Fun1, class Fun2>
Fun2 Chain(const Fun1 &fun1, const Fun2 &fun2) {
  return Fun2(Chainer<Fun1, Fun2>(fun1, fun2));
}

} // namespace Loki

#endif // end file guardian
