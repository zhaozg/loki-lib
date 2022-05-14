///////////////////////////////////////////////////////////////////////////////
// Unit Test for Loki
//
// Copyright Terje Sletteb? and Pavel Vozenilek 2002.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef ABSTRACTFACTORYTEST_H
#define ABSTRACTFACTORYTEST_H

// $Id$


#include <memory>
#include <typeinfo>
#include <loki/AbstractFactory.h>

///////////////////////////////////////////////////////////////////////////////
// AbstractFactoryTest
///////////////////////////////////////////////////////////////////////////////

class Soldier { public: virtual ~Soldier() {} };
class Monster { public: virtual ~Monster() {} };
class SuperMonster { public: virtual ~SuperMonster() {} };

class SillySoldier : public Soldier {};
class SillyMonster : public Monster {};
class SillySuperMonster : public SuperMonster {};

class BadSoldier : public Soldier {};
class BadMonster : public Monster {};
class BadSuperMonster : public SuperMonster {};

#ifndef LOKI_DISABLE_TYPELIST_MACROS

typedef Loki::AbstractFactory<LOKI_TYPELIST_3(Soldier, Monster, SuperMonster)> AbstractEnemyFactory;

typedef Loki::ConcreteFactory<AbstractEnemyFactory, Loki::OpNewFactoryUnit,
  LOKI_TYPELIST_3(SillySoldier, SillyMonster, SillySuperMonster)> EasyLevelEnemyFactory;

typedef Loki::ConcreteFactory<AbstractEnemyFactory, Loki::OpNewFactoryUnit,
  LOKI_TYPELIST_3(BadSoldier, BadMonster, BadSuperMonster)> HardLevelEnemyFactory;

#else // LOKI_DISABLE_TYPELIST_MACROS

typedef Loki::AbstractFactory<Seq<Soldier, Monster, SuperMonster>::Type > AbstractEnemyFactory;

typedef Loki::ConcreteFactory<AbstractEnemyFactory, Loki::OpNewFactoryUnit,
Seq<SillySoldier, SillyMonster, SillySuperMonster>::Type > EasyLevelEnemyFactory;

typedef Loki::ConcreteFactory<AbstractEnemyFactory, Loki::OpNewFactoryUnit,
Seq<BadSoldier, BadMonster, BadSuperMonster>::Type > HardLevelEnemyFactory;

#endif

class AbstractFactoryTest : public Test
{
public:
  AbstractFactoryTest() : Test("AbstractFactory.h") {}

  virtual void execute(TestResult &result)
    {
    printName(result);

    using namespace Loki;

    bool r;

    std::shared_ptr<AbstractEnemyFactory> easyFactory(new EasyLevelEnemyFactory);
    std::shared_ptr<AbstractEnemyFactory> hardFactory(new HardLevelEnemyFactory);

    Soldier *s;

    s = easyFactory->Create<Soldier>();

    r= !!(typeid(*s)==typeid(SillySoldier)); //SGB !! eliminates bool-to-int performance warning

    delete s;

    testAssert("AbstractFactory",r,result);

    std::cout << '\n';
    }
} abstractFactoryTest;

#endif
