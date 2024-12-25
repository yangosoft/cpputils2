/**
 * @file shm.hpp
 * @brief Shared memory abstraction for POSIX systems.
 *
 *
 */

#pragma once

#include "cpputils2/common/types.hpp"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// #include <expected>
#include <string>
#include <string_view>

namespace CppUtils2
{

  const int32_t INVALID_FD = -1;
  class Shm
  {
  public:
    Shm()
        : fd(INVALID_FD), ptr_shared_mem{nullptr}
    {
    }

    Shm(const std::string &file_mem_path)
        : mem_path(file_mem_path), fd(INVALID_FD), ptr_shared_mem{nullptr}
    {
    }

    /// @brief Open an existing shared memory
    /// @param file_mem_path Path to the shared memory
    /// @param mem_size Size of the shared memory
    /// @return 0 on success, -1 on error
    Result open_existing(const std::string &file_mem_path, std::size_t mem_size)
    {
      mem_path = file_mem_path;
      return open_existing(mem_size);
    }

    /// @brief Open an existing shared memory
    /// @param mem_size Size of the shared memory
    /// @return 0 on success, -1 on error
    Result open_existing(std::size_t mem_size)
    {
      if (fd != INVALID_FD)
      {
        return Result::RET_ERROR;
      }
      int flags = O_RDWR;
      return shared_open(flags, mem_size);
    }

    /// @brief Allocate a new shared memory
    /// @param mem_size Size of the shared memory
    /// @return 0 on success, -1 on error
    Result allocate(std::size_t mem_size)
    {
      if (fd != INVALID_FD)
      {
        return Result::RET_ERROR;
      }

      int flags = O_CREAT | O_EXCL | O_RDWR;

      return shared_open(flags, mem_size);
    }

    /// @brief Removes the shared memory file
    /// @return 0 on success, -1 on error
    Result unlink()
    {
      auto ret = shm_unlink(mem_path.c_str());
      if (ret != -1)
      {
        return Result::RET_OK;
      }
      return Result::RET_ERROR;
    }

    /// @brief Closes the shared memory
    void close()
    {
      if (fd != INVALID_FD)
      {
        ::close(fd);
      }
      fd = INVALID_FD;
    }

    /// @brief Get the pointer to the shared memory
    /// @return Pointer to the shared memory
    void *get_raw_ptr()
    {
      return ptr_shared_mem;
    }

    virtual ~Shm()
    {
      close();
    }

  private:
    std::string mem_path;
    int fd;
    void *ptr_shared_mem;

    Result shared_open(int flags, std::size_t mem_size)
    {

      fd = shm_open(mem_path.c_str(), flags, S_IRUSR | S_IWUSR);

      if (fd == INVALID_FD)
      {
        return Result::RET_ERROR;
      }

      if (ftruncate(fd, mem_size) == INVALID_FD)
      {
        return Result::RET_ERROR;
      }

      ptr_shared_mem = mmap(nullptr, mem_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
      if (ptr_shared_mem == MAP_FAILED)
      {
        return Result::RET_ERROR;
      }

      return Result::RET_OK;
    }
  };
} // namespace CppUtils2