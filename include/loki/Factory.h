////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// Copyright (c) 2005 by Peter Kuemmel
// This code DOES NOT accompany the book:
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
#ifndef LOKI_FACTORYPARM_INC_
#define LOKI_FACTORYPARM_INC_

// $Id$

#include <loki/Functor.h>
#include <loki/LokiTypeInfo.h>
#include <loki/Sequence.h>
#include <loki/SmallObj.h>

#include <map>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
// unreachable code if OnUnknownType throws an exception
#endif

/**
 * \defgroup	FactoriesGroup Factories
 * \defgroup	FactoryGroup Factory
 * \ingroup		FactoriesGroup
 * \brief		Implements a generic object factory.
 *
 * <i>The Factory Method pattern is an object-oriented design pattern.
 * Like other creational patterns, it deals with the problem of creating objects
 * (products) without specifying the exact class of object that will be created.
 * Factory Method, one of the patterns from the Design Patterns book, handles
 * this problem by defining a separate method for creating the objects, which
 * subclasses can then override to specify the derived type of product that will
 * be created.
 * <br>
 * More generally, the term Factory Method is often used to refer to any method
 * whose main purpose is creation of objects.</i>
 * <div ALIGN="RIGHT"><a
 * href="http://en.wikipedia.org/wiki/Factory_method_pattern">
 * Wikipedia</a></div>
 *
 * Loki proposes a generic version of the Factory. Here is a typical use.<br>
 * <code><br>
 * 1. Factory< AbstractProduct, int > aFactory;<br>
 * 2. aFactory.Register( 1, createProductNull );<br>
 * 3. aFactory.CreateObject( 1 ); <br>
 * </code><br>
 * <br>
 * - 1. The declaration<br>
 * You want a Factory that produces AbstractProduct.<br>
 * The client will refer to a creation method through an int.<br>
 * - 2.The registration<br>
 * The code that will contribute to the Factory will now need to declare its
 * ProductCreator by registering them into the Factory.<br>
 * A ProductCreator is a just a function that will return the right object. ie
 * <br> <code> Product* createProductNull()<br>
 * {<br>
 *     return new Product<br>
 * }<br>
 * </code><br>
 * - 3. The use<br>
 * Now the client can create object by calling the Factory's CreateObject method
 * with the right identifier. If the ProductCreator were to have arguments
 * (<i>ie :Product* createProductParm( int a, int b )</i>)
 */

namespace Loki {

/**
 * \defgroup	FactoryErrorPoliciesGroup Factory Error Policies
 * \ingroup		FactoryGroup
 * \brief		Manages the "Unknown Type" error in an object factory
 *
 * \class DefaultFactoryError
 * \ingroup		FactoryErrorPoliciesGroup
 * \brief		Default policy that throws an exception
 *
 */

template <typename IdentifierType, class AbstractProduct>
struct DefaultFactoryError {
  struct Exception : public std::exception {
    const char *what() const throw() { return "Unknown Type"; }
  };

  static AbstractProduct *OnUnknownType(IdentifierType) { throw Exception(); }
};

////////////////////////////////////////////////////////////////////////////////
// class template FunctorImpl
////////////////////////////////////////////////////////////////////////////////
template <typename AP, class Id, typename... Parms>
struct FactoryImpl  {

  virtual ~FactoryImpl() {}
  virtual AP *CreateObject(const Id &id, Parms...parms) = 0;
};

////////////////////////////////////////////////////////////////////////////////
///  \class Factory
///
///  \ingroup FactoryGroup
///  Implements a generic object factory.
///
///  Create functions can have up to 15 parameters.
///
///  \par Singleton lifetime when used with Loki::SingletonHolder
///  Because Factory uses internally Functors which inherits from
///  SmallObject you must use the singleton lifetime
///  \code Loki::LongevityLifetime::DieAsSmallObjectChild \endcode
///  Alternatively you could suppress for Functor the inheritance
///  from SmallObject by defining the macro:
/// \code LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT \endcode
////////////////////////////////////////////////////////////////////////////////
template <class AbstractProduct,
          typename IdentifierType,
          template <typename, class> class FactoryErrorPolicy = DefaultFactoryError,
          typename... Parms>
class Factory : public FactoryErrorPolicy<IdentifierType, AbstractProduct> {
protected:
  typedef FactoryImpl<AbstractProduct, IdentifierType, Parms...> Impl;

  typedef Functor<AbstractProduct *, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, Parms...> ProductCreator;

private:
  typedef std::map<IdentifierType, ProductCreator> IdToProductMap;

  IdToProductMap associations_;

public:
  Factory() : associations_() {}

  ~Factory() {
    associations_.erase(associations_.begin(), associations_.end());
  }

  bool Register(const IdentifierType &id, ProductCreator creator) {
    return associations_
               .insert(typename IdToProductMap::value_type(id, creator))
               .second != 0;
  }

  template <class PtrObj, typename CreaFn>
  bool Register(const IdentifierType &id, const PtrObj &p, CreaFn fn) {
    ProductCreator creator(p, fn);
    return associations_
               .insert(typename IdToProductMap::value_type(id, creator))
               .second != 0;
  }

  bool Unregister(const IdentifierType &id) {
    return associations_.erase(id) != 0;
  }

  bool IsRegistered(const IdentifierType &id) {
    return associations_.find(id) != associations_.end();
  }

  std::vector<IdentifierType> RegisteredIds() {
    std::vector<IdentifierType> ids;
    for (typename IdToProductMap::iterator it = associations_.begin();
         it != associations_.end(); ++it) {
      ids.push_back(it->first);
    }
    return ids;
  }

  AbstractProduct *CreateObject(const IdentifierType &id, Parms... parms) {
    typename IdToProductMap::iterator i = associations_.find(id);
    if (i != associations_.end())
      return (i->second)(parms...);
    return this->OnUnknownType(id);
  }

};

/**
 *   \defgroup	CloneFactoryGroup Clone Factory
 *   \ingroup	FactoriesGroup
 *   \brief		Creates a copy from a polymorphic object.
 *
 *   \class		CloneFactory
 *   \ingroup	CloneFactoryGroup
 *   \brief		Creates a copy from a polymorphic object.
 */

template <class AbstractProduct,
          class ProductCreator = AbstractProduct *(*)(const AbstractProduct *),
          template <typename, class> class FactoryErrorPolicy = DefaultFactoryError>
class CloneFactory : public FactoryErrorPolicy<TypeInfo, AbstractProduct> {
public:
  bool Register(const TypeInfo &ti, ProductCreator creator) {
    return associations_
               .insert(typename IdToProductMap::value_type(ti, creator))
               .second != 0;
  }

  bool Unregister(const TypeInfo &id) { return associations_.erase(id) != 0; }

  AbstractProduct *CreateObject(const AbstractProduct *model) {
    if (model == NULL) {
      return NULL;
    }

    typename IdToProductMap::iterator i = associations_.find(typeid(*model));

    if (i != associations_.end()) {
      return (i->second)(model);
    }
    return this->OnUnknownType(typeid(*model));
  }

private:
  typedef std::map<TypeInfo, ProductCreator> IdToProductMap;
  IdToProductMap associations_;
};

} // namespace Loki

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // end file guardian
