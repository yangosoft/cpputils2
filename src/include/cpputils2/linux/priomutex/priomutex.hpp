#pragma once

#include "cpputils2/common/types.hpp"

#include <pthread.h>

namespace CppUtils2
{

    class priomutex
    {

    public:
        using native_handle_type = pthread_mutex_t *;

        priomutex()
        {
            initialized = false;
            pthread_mutexattr_t attr;

            int ret = pthread_mutexattr_init(&attr);
            if (ret != 0)
            {
                return;
            }

            ret = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
            if (ret != 0)
            {
                return;
            }

            ret = pthread_mutex_init(&pt_mutex, &attr);
            if (ret != 0)
            {
                return;
            }

            initialized = true;
        }

        virtual ~priomutex()
        {
            pthread_mutex_destroy(&pt_mutex);
        }

        priomutex(const priomutex &) = delete;
        priomutex &operator=(const priomutex &) = delete;

        void lock()
        {
            auto ret = pthread_mutex_lock(&pt_mutex);
            if (ret != 0)
            {
                // (yangosoft) TODO handle error
                initialized = false;
            }
        }

        void unlock() noexcept
        {
            pthread_mutex_unlock(&pt_mutex);
        }

        bool try_lock() noexcept
        {
            return pthread_mutex_trylock(&pt_mutex) == 0;
        }

        native_handle_type native_handle() noexcept
        {
            return &pt_mutex;
        }

        bool is_lock_free() const noexcept
        {
            return false;
        }

        bool is_initialized() const noexcept
        {
            return initialized;
        }

    private:
        pthread_mutex_t pt_mutex;
        bool initialized;
    };
} // namespace CppUtils2