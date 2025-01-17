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

#include <iostream>
#include <loki/Visitor.h>

class Base : public Loki::BaseVisitable<> {
public:
  LOKI_DEFINE_VISITABLE()
};

class Type1 : public Base {
public:
  LOKI_DEFINE_VISITABLE()
};

class VariableVisitor : public Loki::BaseVisitor,
// public Loki::Visitor<Base>,
// public Loki::Visitor<Type1>
                        public Loki::Visitor<Loki::Seq<Base, Type1>::Type>
{
public:
  void Visit(Base &) { std::cout << "void Visit(Base&)\n"; }
  void Visit(Type1 &) { std::cout << "void Visit(Type1&)\n"; }
};

class CBase : public Loki::BaseVisitable<void, Loki::DefaultCatchAll, true> {
public:
  LOKI_DEFINE_CONST_VISITABLE()
};

class CType1 : public CBase {
public:
  LOKI_DEFINE_CONST_VISITABLE()
};

class CVariableVisitor
    : public Loki::BaseVisitor,
// public Loki::Visitor<CBase,void,true>,
// public Loki::Visitor<CType1,void,true>
      public Loki::Visitor<Loki::Seq<CBase, CType1>::Type, void, true>
{
public:
  void Visit(const CBase &) { std::cout << "void Visit(CBase&)\n"; }
  void Visit(const CType1 &) { std::cout << "void Visit(CType1&)\n"; }
};

int main() {
  VariableVisitor visitor;
  Type1 type1;
  Base *dyn = &type1;
  dyn->Accept(visitor);

  CVariableVisitor cvisitor;
  CType1 ctype1;
  CBase *cdyn = &ctype1;
  cdyn->Accept(cvisitor);
}
