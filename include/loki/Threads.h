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
#ifndef LOKI_THREADS_INC_
#define LOKI_THREADS_INC_

// $Id$

///  @defgroup  ThreadingGroup Threading
///  Policies to for the threading model:
///
///  - SingleThreaded
///  - ObjectLevelLockable
///  - ClassLevelLockable
///
///  All classes in Loki have configurable threading model.
///
///  The macro LOKI_DEFAULT_THREADING selects the default
///  threading model for certain components of Loki
///  (it affects only default template arguments)
///
///  \par Usage:
///
///  To use a specific threading model define
///
///  - nothing, single-theading is default
///  - LOKI_OBJECT_LEVEL_THREADING for object-level-threading
///  - LOKI_CLASS_LEVEL_THREADING for class-level-threading
///
///  \par Supported platfroms:
///
///  - Windows (windows.h)
///  - POSIX (pthread.h):
///    No recursive mutex support with pthread.
///    This means: calling Lock() on a Loki::Mutex twice from the
///    same thread before unlocking the mutex deadlocks the system.
///    To avoid this redesign your synchronization. See also:
///    http://sourceforge.net/tracker/index.php?func=detail&aid=1516182&group_id=29557&atid=396647

#include <cassert>
#include <mutex>

#define LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL ::Loki::ClassLevelLockable

#if defined(LOKI_CLASS_LEVEL_THREADING) && !defined(LOKI_OBJECT_LEVEL_THREADING)
#define LOKI_DEFAULT_THREADING ::Loki::ClassLevelLockable
#else
#define LOKI_DEFAULT_THREADING ::Loki::ObjectLevelLockable
#endif

#if !defined(LOKI_DEFAULT_MUTEX)
#define LOKI_DEFAULT_MUTEX Loki::Mutex<std::mutex>
#endif
#if !defined(LOKI_DEFAULT_RECURSIVE_MUTEX)
#define LOKI_DEFAULT_RECURSIVE_MUTEX Loki::Mutex<std::recursive_mutex>
#endif

namespace Loki {
////////////////////////////////////////////////////////////////////////////////
///  \class Mutex
//
///  \ingroup ThreadingGroup
///  A simple and portable Mutex.  A default policy class for locking objects.
////////////////////////////////////////////////////////////////////////////////

template <class T = std::mutex> class Mutex {
public:
  Mutex() {}
  ~Mutex() {}
  void Lock() { mtx_.lock(); }
  inline void Unlock() { mtx_.unlock(); }

private:
  /// Copy-constructor not implemented.
  Mutex(const Mutex &);
  /// Copy-assignement operator not implemented.
  Mutex &operator=(const Mutex &);
  T mtx_;
};

////////////////////////////////////////////////////////////////////////////////
///  \class SingleThreaded
///
///  \ingroup ThreadingGroup
///  Implementation of the ThreadingModel policy used by various classes
///  Implements a single-threaded model; no synchronization
////////////////////////////////////////////////////////////////////////////////
template <class Host, class MutexPolicy = LOKI_DEFAULT_MUTEX>
class SingleThreaded {
public:
  /// \struct Lock
  /// Dummy Lock class
  struct Lock {
    Lock() {}
    explicit Lock(const SingleThreaded &) {}
    explicit Lock(const SingleThreaded *) {}
  };

  typedef Host VolatileType;

  typedef int IntType;
};

////////////////////////////////////////////////////////////////////////////////
///  \class ObjectLevelLockable
///
///  \ingroup ThreadingGroup
///  Implementation of the ThreadingModel policy used by various classes
///  Implements a object-level locking scheme
////////////////////////////////////////////////////////////////////////////////
template <class Host, class MutexPolicy = LOKI_DEFAULT_MUTEX>
class ObjectLevelLockable {
  mutable MutexPolicy mtx_;

public:
  ObjectLevelLockable() : mtx_() {}

  ObjectLevelLockable(const ObjectLevelLockable &) : mtx_() {}

  ~ObjectLevelLockable() {}

  class Lock;
  friend class Lock;

  ///  \struct Lock
  ///  Lock class to lock on object level
  class Lock {
  public:
    /// Lock object
    explicit Lock(const ObjectLevelLockable &host) : host_(host) {
      host_.mtx_.Lock();
    }

    /// Lock object
    explicit Lock(const ObjectLevelLockable *host) : host_(*host) {
      host_.mtx_.Lock();
    }

    /// Unlock object
    ~Lock() { host_.mtx_.Unlock(); }

  private:
    /// private by design of the object level threading
    Lock();
    Lock(const Lock &);
    Lock &operator=(const Lock &);
    const ObjectLevelLockable &host_;
  };

  typedef volatile Host VolatileType;
  static MutexPolicy atomic_mutex_;
};

template <class Host, class MutexPolicy>
MutexPolicy ObjectLevelLockable<Host, MutexPolicy>::atomic_mutex_;

////////////////////////////////////////////////////////////////////////////////
///  \class ClassLevelLockable
///
///  \ingroup ThreadingGroup
///  Implementation of the ThreadingModel policy used by various classes
///  Implements a class-level locking scheme
////////////////////////////////////////////////////////////////////////////////
template <class Host, class MutexPolicy = LOKI_DEFAULT_MUTEX>
class ClassLevelLockable {
  struct Initializer {
    friend ClassLevelLockable;

    static Initializer &GetIt(void) { return initializer_; }

    inline bool IsInit(void) { return init_; }
    inline MutexPolicy &GetMutex(void) { return mtx_; }

  private:
    bool init_;
    MutexPolicy mtx_;

    Initializer() : init_(false), mtx_() { init_ = true; }

    ~Initializer() { assert(init_); }

    Initializer(const Initializer &) = delete;
    Initializer &operator=(const Initializer &) = delete;
  };

  static Initializer initializer_;

public:
  class Lock;
  friend class Lock;

  ///  \struct Lock
  ///  Lock class to lock on class level
  class Lock {
  public:
    /// Lock class
    Lock() {
      Initializer &initializer = Initializer::GetIt();
      assert(initializer.IsInit());
      initializer.GetMutex().Lock();
    }

    /// Lock class
    explicit Lock(const ClassLevelLockable &) {
      Initializer &initializer = Initializer::GetIt();
      assert(initializer.IsInit());
      initializer.GetMutex().Lock();
    }

    /// Lock class
    explicit Lock(const ClassLevelLockable *) {
      Initializer &initializer = Initializer::GetIt();
      assert(initializer.IsInit());
      initializer.GetMutex().Lock();
    }

    /// Unlock class
    ~Lock() {
      Initializer &initializer = Initializer::GetIt();
      assert(initializer.IsInit());
      initializer.GetMutex().Unlock();
    }

  private:
    Lock(const Lock &);
    Lock &operator=(const Lock &);
  };

  typedef volatile Host VolatileType;
  static MutexPolicy atomic_mutex_;
};

template <class Host, class MutexPolicy>
MutexPolicy ClassLevelLockable<Host, MutexPolicy>::atomic_mutex_;

template <class Host, class MutexPolicy>
typename ClassLevelLockable<Host, MutexPolicy>::Initializer
    ClassLevelLockable<Host, MutexPolicy>::initializer_;
} // namespace Loki

#endif // end file guardian
