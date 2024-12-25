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

#include "cpputils2/common/types.hpp"
#include "cpputils2/file/file.hpp"
#include "cpputils2/linux/futex/futex.hpp"
#include "cpputils2/linux/shm/shm.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <memory>

namespace CppUtils2
{

    class SharedFutex
    {
    public:
        SharedFutex(const std::string &mem_file_path)
            : file_path(mem_file_path), ptr_futex(nullptr)
        {
            init(file_path);
        }

        SharedFutex()
            : ptr_futex(nullptr)
        {
        }

        Result init(const std::string &mem_path)
        {
            if (ptr_futex != nullptr)
            {
                return Result::RET_ERROR;
            }

            file_path = mem_path;

            if (file_exists("/dev/shm/" + mem_path))
            {
                shm_atomic.open_existing(file_path, sizeof(int));
            }
            else
            {

                auto ret = shm_atomic.open_existing(file_path, sizeof(int));

                ret = shm_atomic.allocate(sizeof(int));

                if (ret != Result::RET_ERROR)
                {
                    return ret;
                }
            }

            auto *ptr_int = static_cast<std::atomic_int *>(shm_atomic.get_raw_ptr());

            ptr_futex = std::make_unique<Futex>(ptr_int);
            return Result::RET_OK;
        }

        void lock()
        {
            ptr_futex->lock();
        }

        void unlock()
        {
            ptr_futex->unlock();
        }

    private:
        std::string file_path;
        Shm shm_atomic;
        std::unique_ptr<Futex> ptr_futex;
    };
} // namespace CppUtils