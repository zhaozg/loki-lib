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
#ifndef LOKI_CACHEDFACTORY_INC_
#define LOKI_CACHEDFACTORY_INC_

// $Id$

#include <algorithm>
#include <cassert>
#include <functional>
#include <sstream>
#include <iostream>
#include <loki/Key.h>
#include <map>
#include <time.h> ///< For clock_t definition.
#include <vector>

#ifdef DO_EXTRA_LOKI_TESTS
#define D(x) x
#else
#define D(x) ;
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__)
#include <time.h>
#endif

/**
 * \defgroup	FactoriesGroup Factories
 * \defgroup	CachedFactoryGroup Cached Factory
 * \ingroup		FactoriesGroup
 * \brief		CachedFactory provides an extension of a Factory with
 * caching support.
 *
 * Once used objects are returned to the CachedFactory that manages its
 * destruction.
 * If your code uses lots of "long to construct/destruct objects" using the
 * CachedFactory will surely speedup the execution.
 */
namespace Loki {
/**
 * \defgroup	EncapsulationPolicyCachedFactoryGroup	Encapsulation policies
 * \ingroup	CachedFactoryGroup
 * \brief	Defines how the object is returned to the client
 */
/**
 * \class	SimplePointer
 * \ingroup	EncapsulationPolicyCachedFactoryGroup
 * \brief	No encaspulation : returns the pointer
 *
 * This implementation does not make any encapsulation.
 * It simply returns the object's pointer.
 */
template <class AbstractProduct> class SimplePointer {
protected:
  typedef AbstractProduct *ProductReturn;
  ProductReturn encapsulate(AbstractProduct *pProduct) { return pProduct; }

  AbstractProduct *release(ProductReturn &pProduct) {
    AbstractProduct *pPointer(pProduct);
    pProduct = NULL;
    return pPointer;
  }
  const char *name() { return "pointer"; }
};

/**
 * \defgroup	CreationPolicyCachedFactoryGroup		Creation
 * policies \ingroup		CachedFactoryGroup
 * \brief		Defines a way to limit the creation operation.
 *
 * For instance one may want to be alerted (Exception) when
 * - Cache has created a more than X object within the last x seconds
 * - Cache creation rate has increased dramatically
 * .
 * which may result from bad caching strategy, or critical overload
 */
/**
 * \class	NeverCreate
 * \ingroup	CreationPolicyCachedFactoryGroup
 * \brief	Never allows creation. Testing purposes only.
 *
 * Using this policy will throw an exception.
 */
class NeverCreate {
protected:
  struct Exception : public std::exception {
    const char *what() const throw() {
      return "NeverFetch Policy : No Fetching allowed";
    }
  };

  bool canCreate() { throw Exception(); }

  void onCreate() {}
  void onDestroy() {}
  const char *name() { return "never"; }
};

/**
 * \class		AlwaysCreate
 * \ingroup	CreationPolicyCachedFactoryGroup
 * \brief		Always allows creation.
 *
 * Doesn't limit the creation in any way
 */
class AlwaysCreate {
protected:
  bool canCreate() { return true; }

  void onCreate() {}
  void onDestroy() {}
  const char *name() { return "always"; }
};

/**
 * \class	RateLimitedCreation
 * \ingroup	CreationPolicyCachedFactoryGroup
 * \brief	Limit in rate.
 *
 * This implementation will prevent from Creating more than maxCreation objects
 * within byTime ms by throwing an exception.
 * Could be usefull to detect prevent loads (http connection for instance).
 * Use the setRate method to set the rate parameters.
 * default is 10 objects in a second.
 */
// !! CAUTION !!
// The std::clock() function is not quite precise
// under linux this policy might not work.
// TODO : get a better implementation (platform dependant)
class RateLimitedCreation {
private:
  typedef std::vector<clock_t> Vector;
  Vector m_vTimes;
  unsigned maxCreation;
  clock_t timeValidity;
  clock_t lastUpdate;

  void cleanVector() {
    using namespace std;
    clock_t currentTime = clock();
    D(cout << "currentTime = " << currentTime << endl;)
    D(cout << "currentTime - lastUpdate = " << currentTime - lastUpdate
           << endl;)
    if (currentTime - lastUpdate > timeValidity) {
      m_vTimes.clear();
      D(cout << " is less than time validity " << timeValidity;)
      D(cout << " so clearing vector" << endl;)
    } else {
      D(cout << "Cleaning time less than " << currentTime - timeValidity
             << endl;)
      D(displayVector();)
      Vector::iterator newEnd = remove_if(
          m_vTimes.begin(), m_vTimes.end(),
          bind(less<clock_t>(), placeholders::_1, currentTime - timeValidity));
      // this rearrangement might be costly, consider optimization
      // by calling cleanVector in less used onCreate function
      // ... although it may not be correct
      m_vTimes.erase(newEnd, m_vTimes.end());
      D(displayVector();)
    }
    lastUpdate = currentTime;
  }
#ifdef DO_EXTRA_LOKI_TESTS
  void displayVector() {
    std::cout << "Vector : ";
    copy(m_vTimes.begin(), m_vTimes.end(),
         std::ostream_iterator<clock_t>(std::cout, " "));
    std::cout << std::endl;
  }
#endif
protected:
  RateLimitedCreation()
      : maxCreation(10), timeValidity(CLOCKS_PER_SEC), lastUpdate(clock()) {}

  struct Exception : public std::exception {
    const char *what() const throw() {
      return "RateLimitedCreation Policy : Exceeded the authorized creation "
             "rate";
    }
  };

  bool canCreate() {
    cleanVector();
    if (m_vTimes.size() > maxCreation)
      throw Exception();
    else
      return true;
  }

  void onCreate() { m_vTimes.push_back(clock()); }

  void onDestroy() {}
  const char *name() { return "rate limited"; }

public:
  // set the creation rate
  // No more than maxCreation within byTime milliseconds
  void setRate(unsigned maxCreation, unsigned byTime) {
    assert(byTime > 0);
    this->maxCreation = maxCreation;
    this->timeValidity = static_cast<clock_t>(byTime * CLOCKS_PER_SEC / 1000);
    D(std::cout << "Setting no more than " << maxCreation << " creation within "
                << this->timeValidity << " ms" << std::endl;)
  }
};

/**
 * \class	AmountLimitedCreation
 * \ingroup	CreationPolicyCachedFactoryGroup
 * \brief	Limit by number of objects
 *
 * This implementation will prevent from Creating more than maxCreation objects
 * within byTime ms by calling eviction policy.
 * Use the setRate method to set the rate parameters.
 * default is 10 objects.
 */
class AmountLimitedCreation {
private:
  unsigned maxCreation;
  unsigned created;

protected:
  AmountLimitedCreation() : maxCreation(10), created(0) {}

  bool canCreate() { return !(created >= maxCreation); }

  void onCreate() { ++created; }

  void onDestroy() { --created; }
  const char *name() { return "amount limited"; }

public:
  // set the creation max amount
  void setMaxCreation(unsigned maxCreation) {
    assert(maxCreation > 0);
    this->maxCreation = maxCreation;
    D(std::cout << "Setting no more than " << maxCreation << " creation"
                << std::endl;)
  }
};

/**
 * \defgroup	EvictionPolicyCachedFactoryGroup		Eviction
 * policies \ingroup	CachedFactoryGroup
 * \brief	Gathers informations about the stored objects and choose a
 * candidate for eviction.
 */

class EvictionException : public std::exception {
public:
  const char *what() const throw() {
    return "Eviction Policy : trying to make room but no objects are available";
  }
};

// The following class is intented to provide helpers to sort
// the container that will hold an eviction score
template <typename ST, // Score type
          typename DT  // Data type
          >
class EvictionHelper {
protected:
  typedef typename std::map<DT, ST> HitMap;
  typedef typename HitMap::iterator HitMapItr;

private:
  typedef std::pair<ST, DT> SwappedPair;
  typedef std::multimap<ST, DT> SwappedHitMap;
  typedef typename SwappedHitMap::iterator SwappedHitMapItr;

protected:
  HitMap m_mHitCount;

  // This function sorts the map according to the score
  // and returns the lower bound of the sorted container
  DT &getLowerBound() {
    assert(!m_mHitCount.empty());
    // inserting the swapped pair into a multimap
    SwappedHitMap copyMap;
    for (HitMapItr itr = m_mHitCount.begin(); itr != m_mHitCount.end(); ++itr)
      copyMap.insert(SwappedPair((*itr).second, (*itr).first));
    if ((*copyMap.rbegin()).first == 0) // the higher score is 0 ...
      throw EvictionException();        // there is no key evict
    return (*copyMap.begin()).second;
  }
};

/**
 * \class	EvictLRU
 * \ingroup	EvictionPolicyCachedFactoryGroup
 * \brief	Evicts least accessed objects first.
 *
 * Implementation of the Least recent used algorithm as
 * described in http://en.wikipedia.org/wiki/Page_replacement_algorithms .
 *
 * WARNING : If an object is heavily fetched
 * (more than ULONG_MAX = UINT_MAX = 4294967295U)
 * it could unfortunately be removed from the cache.
 */
template <typename DT,           // Data Type (AbstractProduct*)
          typename ST = unsigned // default data type to use as Score Type
          >
class EvictLRU : public EvictionHelper<ST, DT> {
private:
  typedef EvictionHelper<ST, DT> EH;

protected:
  virtual ~EvictLRU() {}

  // OnStore initialize the counter for the new key
  // If the key already exists, the counter is reseted
  void onCreate(const DT &key) { EH::m_mHitCount[key] = 0; }

  void onFetch(const DT &) {}

  // onRelease increments the hit counter associated with the object
  void onRelease(const DT &key) { ++(EH::m_mHitCount[key]); }

  void onDestroy(const DT &key) { EH::m_mHitCount.erase(key); }

  // this function is implemented in Cache and redirected
  // to the Storage Policy
  virtual void remove(DT const key) = 0;

  // LRU Eviction policy
  void evict() { remove(EH::getLowerBound()); }
  const char *name() { return "LRU"; }
};

/**
 * \class	EvictAging
 * \ingroup	EvictionPolicyCachedFactoryGroup
 * \brief	LRU aware of the time span of use
 *
 * Implementation of the Aging algorithm as
 * described in http://en.wikipedia.org/wiki/Page_replacement_algorithms .
 *
 * This method is much more costly than evict LRU so
 * if you need extreme performance consider switching to EvictLRU
 */
template <typename DT,           // Data Type (AbstractProduct*)
          typename ST = unsigned // default data type to use as Score Type
          >
class EvictAging : public EvictionHelper<ST, DT> {
private:
  EvictAging(const EvictAging &);
  EvictAging &operator=(const EvictAging &);
  typedef EvictionHelper<ST, DT> EH;
  typedef typename EH::HitMap HitMap;
  typedef typename EH::HitMapItr HitMapItr;

  // update the counter
  template <class T>
  struct updateCounter : public std::function<void(T)> {
    updateCounter(const DT &key) : key_(key) {}
    void operator()(T x) {
      x.second =
          (x.first == key_ ? (x.second >> 1) | (1 << ((sizeof(ST) - 1) * 8))
                           : x.second >> 1);
      D(std::cout << x.second << std::endl;)
    }
    const DT &key_;
    updateCounter(const updateCounter &rhs) : key_(rhs.key_) {}

  private:
    updateCounter &operator=(const updateCounter &rhs);
  };

protected:
  EvictAging() {}
  virtual ~EvictAging() {}

  // OnStore initialize the counter for the new key
  // If the key already exists, the counter is reseted
  void onCreate(const DT &key) { EH::m_mHitCount[key] = 0; }

  void onFetch(const DT &) {}

  // onRelease increments the hit counter associated with the object
  // Updating every counters by iterating over the map
  // If the key is the key of the fetched object :
  //  the counter is shifted to the right and it's MSB is set to 1
  // else
  //  the counter is shifted to the left
  void onRelease(const DT &key) {
    std::for_each(EH::m_mHitCount.begin(), EH::m_mHitCount.end(),
                  updateCounter<typename HitMap::value_type>(key));
  }

  void onDestroy(const DT &key) { EH::m_mHitCount.erase(key); }

  // this function is implemented in Cache and redirected
  // to the Storage Policy
  virtual void remove(DT const key) = 0;

  // LRU with Aging Eviction policy
  void evict() { remove(EH::getLowerBound()); }
  const char *name() { return "LRU with aging"; }
};

/**
 * \class	EvictRandom
 * \ingroup	EvictionPolicyCachedFactoryGroup
 * \brief	Evicts a random object
 *
 * Implementation of the Random algorithm as
 * described in http://en.wikipedia.org/wiki/Page_replacement_algorithms .
 */
template <typename DT,       // Data Type (AbstractProduct*)
          typename ST = void // Score Type not used by this policy
          >
class EvictRandom {
private:
  std::vector<DT> m_vKeys;
  typedef typename std::vector<DT>::size_type size_type;
  typedef typename std::vector<DT>::iterator iterator;

protected:
  virtual ~EvictRandom() {}

  void onCreate(const DT &) {}

  void onFetch(const DT &) {}

  void onRelease(const DT &key) { m_vKeys.push_back(key); }

  void onDestroy(const DT &key) {
    using namespace std;
    m_vKeys.erase(remove_if(m_vKeys.begin(), m_vKeys.end(),
                            bind(equal_to<DT>(), std::placeholders::_1, key)),
                  m_vKeys.end());
  }

  // Implemented in Cache and redirected to the Storage Policy
  virtual void remove(DT const key) = 0;

  // Random Eviction policy
  void evict() {
    if (m_vKeys.empty())
      throw EvictionException();
    size_type random = static_cast<size_type>(
        (m_vKeys.size() * rand()) / (static_cast<size_type>(RAND_MAX) + 1));
    remove(*(m_vKeys.begin() + random));
  }
  const char *name() { return "random"; }
};

/**
 * \defgroup	StatisticPolicyCachedFactoryGroup		Statistic
 * policies \ingroup	CachedFactoryGroup
 * \brief	Gathers information about the cache.
 *
 * For debugging purpose this policy proposes to gather informations
 * about the cache. This could be useful to determine whether the cache is
 * mandatory or if the policies are well suited to the application.
 */
/**
 * \class	NoStatisticPolicy
 * \ingroup	StatisticPolicyCachedFactoryGroup
 * \brief	Do nothing
 *
 * Should be used in release code for better performances
 */
class NoStatisticPolicy {
protected:
  void onDebug() {}
  void onFetch() {}
  void onRelease() {}
  void onCreate() {}
  void onDestroy() {}
  const char *name() { return "no"; }
};

/**
 * \class	SimpleStatisticPolicy
 * \ingroup	StatisticPolicyCachedFactoryGroup
 * \brief	Simple statistics
 *
 * Provides the following informations about the cache :
 *   - Created objects
 *   - Fetched objects
 *   - Destroyed objects
 *   - Cache hit
 *   - Cache miss
 *   - Currently allocated
 *   - Currently out
 *   - Cache overall efficiency
 */
class SimpleStatisticPolicy {
private:
  unsigned allocated, created, hit, out, fetched;

protected:
  SimpleStatisticPolicy()
      : allocated(0), created(0), hit(0), out(0), fetched(0) {}

  void onDebug(std::stringstream &ss) {
    using namespace std;
    ss << "############################" << endl;
    ss << "## About this cache " << this << endl;
    ss << "## + Created objects     : " << created << endl;
    ss << "## + Fetched objects     : " << fetched << endl;
    ss << "## + Destroyed objects   : " << created - allocated << endl;
    ss << "## + Cache hit           : " << hit << endl;
    ss << "## + Cache miss          : " << fetched - hit << endl;
    ss << "## + Currently allocated : " << allocated << endl;
    ss << "## + Currently out       : " << out << endl;
    ss << "############################" << endl;
    if (fetched != 0) {
      ss << "## Overall efficiency " << 100 * double(hit) / fetched << "%"
           << endl;
      ss << "############################" << endl;
    }
    ss << endl;
  }

  void onFetch() {
    ++fetched;
    ++out;
    ++hit;
  }
  void onRelease() { --out; }
  void onCreate() {
    ++created;
    ++allocated;
    --hit;
  }
  void onDestroy() { --allocated; }

  const char *name() { return "simple"; }

public:
  unsigned getCreated() { return created; }
  unsigned getFetched() { return fetched; }
  unsigned getHit() { return hit; }
  unsigned getMissed() { return fetched - hit; }
  unsigned getAllocated() { return allocated; }
  unsigned getOut() { return out; }
  unsigned getDestroyed() { return created - allocated; }
};

///////////////////////////////////////////////////////////////////////////
// Cache Factory definition
///////////////////////////////////////////////////////////////////////////
class CacheException : public std::exception {
public:
  const char *what() const throw() { return "Internal Cache Error"; }
};

/**
 * \class		CachedFactory
 * \ingroup		CachedFactoryGroup
 * \brief		Factory with caching support
 *
 * This class acts as a Factory (it creates objects)
 * but also keeps the already created objects to prevent
 * long constructions time.
 *
 * Note this implementation do not retain ownership.
 */
template <class AbstractProduct, typename IdentifierType,
          template <class> class EncapsulationPolicy = SimplePointer,
          class CreationPolicy = AlwaysCreate,
          template <typename, typename> class EvictionPolicy = EvictRandom,
          class StatisticPolicy = NoStatisticPolicy,
          template <typename, class> class FactoryErrorPolicy = DefaultFactoryError,
          class ObjVector = std::vector<AbstractProduct *>,
          typename... Parms>
class CachedFactory : protected EncapsulationPolicy<AbstractProduct>,
                      public CreationPolicy,
                      public StatisticPolicy,
                      EvictionPolicy<AbstractProduct *, unsigned> {
private:
  typedef Factory<AbstractProduct, IdentifierType, FactoryErrorPolicy, Parms...> MyFactory;
  typedef FactoryImpl<AbstractProduct, IdentifierType, Parms...> Impl;
  typedef Functor<AbstractProduct *, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, Parms...> ProductCreator;
  typedef EncapsulationPolicy<AbstractProduct> NP;
  typedef CreationPolicy CP;
  typedef StatisticPolicy SP;
  typedef EvictionPolicy<AbstractProduct *, unsigned> EP;

public:
  typedef typename NP::ProductReturn ProductReturn;

private:
  typedef Key<Impl, IdentifierType, Parms...> MyKey;
  typedef std::map<MyKey, ObjVector> KeyToObjVectorMap;
  typedef std::map<AbstractProduct *, MyKey> FetchedObjToKeyMap;

  MyFactory factory;
  KeyToObjVectorMap fromKeyToObjVector;
  FetchedObjToKeyMap providedObjects;
  unsigned outObjects;

  ObjVector &getContainerFromKey(MyKey key) { return fromKeyToObjVector[key]; }

  AbstractProduct *getPointerToObjectInContainer(ObjVector &entry) {
    if (entry.empty()) // No object available
    {                  // the object will be created in the calling function.
      // It has to be created in the calling function because of
      // the variable number of parameters for CreateObject(...) method
      return NULL;
    } else { // returning the found object
      AbstractProduct *pObject(entry.back());
      assert(pObject != NULL);
      entry.pop_back();
      return pObject;
    }
  }

  bool shouldCreateObject(AbstractProduct *const pProduct) {
    if (pProduct != NULL) // object already exists
      return false;
    if (CP::canCreate() == false) // Are we allowed to Create ?
      EP::evict();                // calling Eviction Policy to clean up
    return true;
  }

  void ReleaseObjectFromContainer(ObjVector &entry,
                                  AbstractProduct *const object) {
    entry.push_back(object);
  }

  void onFetch(AbstractProduct *const pProduct) {
    SP::onFetch();
    EP::onFetch(pProduct);
    ++outObjects;
  }

  void onRelease(AbstractProduct *const pProduct) {
    SP::onRelease();
    EP::onRelease(pProduct);
    --outObjects;
  }

  void onCreate(AbstractProduct *const pProduct) {
    CP::onCreate();
    SP::onCreate();
    EP::onCreate(pProduct);
  }

  void onDestroy(AbstractProduct *const pProduct) {
    CP::onDestroy();
    SP::onDestroy();
    EP::onDestroy(pProduct);
  }

  // delete the object
  template <class T> struct deleteObject : public std::function<void(T)> {
    void operator()(T x) { delete x; }
  };

  // delete the objects in the vector
  template <class T>
  struct deleteVectorObjects : public std::function<void(T)> {
    void operator()(T x) {
      ObjVector &vec(x.second);
      std::for_each(vec.begin(), vec.end(),
                    deleteObject<typename ObjVector::value_type>());
    }
  };

  // delete the keys of the map
  template <class T>
  struct deleteMapKeys : public std::function<void(T)> {
    void operator()(T x) { delete x.first; }
  };

protected:
  virtual void remove(AbstractProduct *const pProduct) {
    typename FetchedObjToKeyMap::iterator fetchedItr =
        providedObjects.find(pProduct);
    if (fetchedItr != providedObjects.end()) // object is unreleased.
      throw CacheException();
    bool productRemoved = false;
    typename KeyToObjVectorMap::iterator objVectorItr;
    typename ObjVector::iterator objItr;
    for (objVectorItr = fromKeyToObjVector.begin();
         objVectorItr != fromKeyToObjVector.end(); ++objVectorItr) {
      ObjVector &v((*objVectorItr).second);
      objItr = remove_if(v.begin(), v.end(),
                         std::bind(std::equal_to<AbstractProduct *>(),
                                   std::placeholders::_1, pProduct));
      if (objItr !=
          v.end()) // we found the vector containing pProduct and removed it
      {
        onDestroy(
            pProduct); // warning policies we are about to destroy an object
        v.erase(objItr, v.end()); // real removing
        productRemoved = true;
        break;
      }
    }
    if (productRemoved == false)
      throw CacheException(); // the product is not in the cache ?!
    delete pProduct;          // deleting it
  }

public:
  CachedFactory()
      : factory(), fromKeyToObjVector(), providedObjects(), outObjects(0) {}

  ~CachedFactory() {
    using namespace std;
    // debug information
    std::stringstream ss;
    SP::onDebug(ss);
    std::cout << ss.str();
    // cleaning the Cache
    for_each(fromKeyToObjVector.begin(), fromKeyToObjVector.end(),
             deleteVectorObjects<typename KeyToObjVectorMap::value_type>());
    if (!providedObjects.empty()) {
      // The factory is responsible for the creation and destruction of objects.
      // If objects are out during the destruction of the Factory : deleting
      // anyway. This might not be a good idea. But throwing an exception in a
      // destructor is considered as a bad pratice and asserting might be too
      // much. What to do ? Leaking memory or corrupting in use pointers ?
      // hmm...
      D(cout << "====>>  Cache destructor : deleting " << providedObjects.size()
             << " in use objects  <<====" << endl
             << endl;)
      for_each(providedObjects.begin(), providedObjects.end(),
               deleteMapKeys<typename FetchedObjToKeyMap::value_type>());
    }
  }

  ///////////////////////////////////
  // Acts as the proxy pattern and //
  // forwards factory methods      //
  ///////////////////////////////////

  bool Register(const IdentifierType &id, ProductCreator creator) {
    return factory.Register(id, creator);
  }

  template <class PtrObj, typename CreaFn>
  bool Register(const IdentifierType &id, const PtrObj &p, CreaFn fn) {
    return factory.Register(id, p, fn);
  }

  bool Unregister(const IdentifierType &id) { return factory.Unregister(id); }

  /// Return the registered ID in this Factory
  std::vector<IdentifierType> &RegisteredIds() {
    return factory.RegisteredIds();
  }

  ProductReturn CreateObject(const IdentifierType &id, Parms...parms) {
    MyKey key(id, parms...);
    AbstractProduct *pProduct(
        getPointerToObjectInContainer(getContainerFromKey(key)));
    if (shouldCreateObject(pProduct)) {
      pProduct = factory.CreateObject(key.id, parms...);
      onCreate(pProduct);
    }
    onFetch(pProduct);
    providedObjects[pProduct] = key;
    return NP::encapsulate(pProduct);
  }

  /// Use this function to release the object
  /**
   * if execution brakes in this function then you tried
   * to release an object that wasn't provided by this Cache
   * ... which is bad :-)
   */
  void ReleaseObject(ProductReturn &object) {
    AbstractProduct *pProduct(NP::release(object));
    typename FetchedObjToKeyMap::iterator itr = providedObjects.find(pProduct);
    if (itr == providedObjects.end())
      throw CacheException();
    onRelease(pProduct);
    ReleaseObjectFromContainer(getContainerFromKey((*itr).second), pProduct);
    providedObjects.erase(itr);
  }

  /// display the cache configuration
  void GetConfigure(std::stringstream& ss) {
    ss << "############################" << std::endl;
    ss << "## Cache configuration" << std::endl;
    ss << "## + Encapsulation " << NP::name() << std::endl;
    ss << "## + Creating      " << CP::name() << std::endl;
    ss << "## + Eviction      " << EP::name() << std::endl;
    ss << "## + Statistics    " << SP::name() << std::endl;
    ss << "############################" << std::endl;
  }
};
template <class AbstractProduct, typename IdentifierType,
          class StatisticPolicy,
          typename... Parms>
class CustomStatisticCachedFactory
  : public CachedFactory<AbstractProduct, IdentifierType,
                         SimplePointer, AlwaysCreate, EvictRandom,
                         StatisticPolicy,
                         DefaultFactoryError,
                         std::vector<AbstractProduct *>,
                         Parms...> {
};
} // namespace Loki

#endif // end file guardian
