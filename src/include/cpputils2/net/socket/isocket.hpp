#pragma once

#include <cstdint>
#include <iostream>
#include <string>

#ifdef _WIN32
using ssize_t = std::int32_t;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace CppUtils2
{
  namespace net
  {

    class ISocket
    {
    public:
      virtual void disconnect() = 0;

      virtual ssize_t write_data(const void *data, ssize_t size) = 0;

      virtual bool write_string(const std::string &data) = 0;

      virtual ssize_t read_data(void *buffer, ssize_t size) const = 0;

      virtual bool read_string(std::string &data) const = 0;

      virtual std::int32_t get_socket_fd() const = 0;

      virtual ~ISocket() = default;
    };
  } // namespace net
} // namespace CppUtils2
