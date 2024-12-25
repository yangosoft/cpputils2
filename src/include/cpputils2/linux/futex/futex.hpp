/**
 * @file futex.hpp
 * @brief Implementation of a futex-based mutex.
 *
 * This file contains the implementation of a mutex using futexes, based on Ulrich Drepper's
 * "Futexes Are Tricky" paper.
 *
 * @see https://www.akkadia.org/drepper/futex.pdf
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <thread>

#include <linux/futex.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

namespace CppUtils2
{

  // 0 means unlocked
  const int UNLOCKED(0);
  // 1 means locked, no waiters
  const int LOCKED_NO_WAITERS(1);
  // 2 means locked, there are waiters in lock()
  const int LOCKED(2);

  // An atomic_compare_exchange as expected in the paper
  static int cmpxchg(std::atomic<int> *atom, int expected, int desired)
  {
    int *ep = &expected;
    std::atomic_compare_exchange_strong(atom, ep, desired);
    return *ep;
  }

  /// @brief Futex-based mutex
  ///
  /// This class implements a mutex using futexes. It's based on Ulrich Drepper's "Futexes Are
  /// Tricky" paper.
  ///
  /// @see https://www.akkadia.org/drepper/futex.pdf
  class Futex
  {
  public:
    /// @brief Default constructor
    Futex()
        : free_resources(true)
    {
      ptr_atom = new std::atomic<int>;
      ptr_atom->store(UNLOCKED);
    }

    /// @brief Constructor that allows to pass a pointer to the atomic to use
    /// @param val_atomic Pointer to atomic value
    Futex(std::atomic<int> *val_atomic)
        : ptr_atom(val_atomic), free_resources(false)
    {
      ptr_atom->store(UNLOCKED);
    }

    /// @brief Locks the mutex
    void lock()
    {
      int c = cmpxchg(ptr_atom, UNLOCKED, LOCKED_NO_WAITERS);
      // If the lock was previously unlocked, there's nothing else for us to do.
      // Otherwise, we'll probably have to wait.
      if (c != UNLOCKED)
      {
        do
        {
          // If the mutex is locked, we signal that we're waiting by setting the
          // atom to 2. A shortcut checks is it's 2 already and avoids the atomic
          // operation in this case.
          if (c == LOCKED || cmpxchg(ptr_atom, LOCKED_NO_WAITERS, LOCKED) != UNLOCKED)
          {
            // Here we have to actually sleep, because the mutex is actually
            // locked. Note that it's not necessary to loop around this syscall;
            // a spurious wakeup will do no harm since we only exit the do...while
            // loop when ptr_atom is indeed 0.
            syscall(SYS_futex, reinterpret_cast<int *>(ptr_atom), FUTEX_WAIT, LOCKED, 0, 0, 0);
          }
          // We're here when either:
          // (a) the mutex was in fact unlocked (by an intervening thread).
          // (b) we slept waiting for the atom and were awoken.
          //
          // So we try to lock the atom again. We set teh state to 2 because we
          // can't be certain there's no other thread at this exact point. So we
          // prefer to err on the safe side.
        } while ((c = cmpxchg(ptr_atom, UNLOCKED, LOCKED)) != UNLOCKED);
      }
    }

    /// @brief Unlock the mutex
    void unlock()
    {
      if (ptr_atom->fetch_sub(1) != LOCKED_NO_WAITERS)
      {
        ptr_atom->store(UNLOCKED);
        syscall(SYS_futex, reinterpret_cast<int *>(ptr_atom), FUTEX_WAKE, LOCKED_NO_WAITERS, 0, 0, 0);
      }
    }

    /// @brief Notifies the number of waiters
    /// @param number_of_waiters
    /// @return -1 on ERROR, 0 on success
    int32_t post(uint32_t number_of_waiters)
    {
      return syscall(SYS_futex, reinterpret_cast<int *>(ptr_atom), FUTEX_WAKE, number_of_waiters, 0, 0, 0);
    }

    /// @brief Wait for the value to change
    /// @param wait_value Value to wait for
    /// @return -1 on ERROR, 0 on success
    int32_t wait(uint32_t wait_value)
    {

      return syscall(SYS_futex, reinterpret_cast<int *>(ptr_atom), FUTEX_WAIT, wait_value, 0, 0, 0);
    }

    /// @brief Destructor
    virtual ~Futex()
    {
      if (free_resources)
      {
        delete ptr_atom;
      }
    }

  private:
    std::atomic<int> *ptr_atom;
    bool free_resources;
  };
} // namespace CppUtils