////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2006 Peter K黰mel
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author makes no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// $Id$

#include <loki/Pimpl.h>

// #define TEST_WITH_BOOST
#ifdef TEST_WITH_BOOST
#include <boost/shared_ptr.hpp>
#endif

#include <loki/SmallObj.h>

using namespace Loki;

/////////////////////////////////////////
// class A declaration
/////////////////////////////////////////

class A {
public:
  A();
  void foo();

private:
  PimplOf<A>::Type d;
};

/////////////////////////////////////////
// class B declaration
/////////////////////////////////////////

class B : private PimplOf<B>::Owner {
public:
  B();
  void foo();
};

/////////////////////////////////////////
// class C declaration
/////////////////////////////////////////

class C {
public:
  C();
  void foo();

private:
  PimplOf<C>::Type p;
  RimplOf<C>::Type d;
};

/////////////////////////////////////////
// class D declaration
/////////////////////////////////////////

class D : private RimplOf<D>::Owner {
public:
  D();
  void foo();
};

////////////////////
// more test code
////////////////////

struct E;

typedef ConstPropPtr<ImplOf<E>> CPropPtr;
typedef std::shared_ptr<ImplOf<E>> StdAutoPtr;

// Pimpl

typedef Pimpl<ImplOf<E>> Pimpl1;
typedef Pimpl<ImplOf<E>, CPropPtr> Pimpl2;
typedef Pimpl<ImplOf<E>, StdAutoPtr> Pimpl5;

struct P1 {
  Pimpl1 d;
  P1();
  void f();
  void f() const;
};
struct P2 {
  Pimpl2 d;
  P2();
  void f();
  void f() const;
};
struct P5 {
  Pimpl5 d;
  P5();
  void f();
  void f() const;
};

// PimplOwner

typedef PimplOwner<ImplOf<E>> PimplOwner1;
typedef PimplOwner<ImplOf<E>, CPropPtr> PimplOwner2;
typedef PimplOwner<ImplOf<E>, StdAutoPtr> PimplOwner5;

struct PO1 : private PimplOwner1 {
  PO1();
  void f();
  void f() const;
};
struct PO2 : private PimplOwner2 {
  PO2();
  void f();
  void f() const;
};
struct PO5 : private PimplOwner5 {
  PO5();
  void f();
  void f() const;
};

// Rimpl

typedef RimplOf<E, Pimpl1> Rimpl1;
typedef RimplOf<E, Pimpl2> Rimpl2;
typedef RimplOf<E, Pimpl5> Rimpl5;

struct R1 {
  Pimpl1 p;
  Rimpl1::Type d;
  R1();
  void f();
  void f() const;
};
struct R2 {
  Pimpl2 p;
  Rimpl2::Type d;
  R2();
  void f();
  void f() const;
};
struct R5 {
  Pimpl5 p;
  Rimpl5::Type d;
  R5();
  void f();
  void f() const;
};

// RimplOwner

typedef RimplOf<E, Pimpl1>::Owner RimplO1;
typedef RimplOf<E, Pimpl2>::Owner RimplO2;
typedef RimplOf<E, Pimpl5>::Owner RimplO5;

struct RO1 : private RimplO1 {
  RO1();
  void f();
  void f() const;
};
struct RO2 : private RimplO2 {
  RO2();
  void f();
  void f() const;
};
struct RO5 : private RimplO5 {
  RO5();
  void f();
  void f() const;
};
