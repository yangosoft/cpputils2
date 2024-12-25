#pragma once

#include "cpputils2/linux/net/socket/isocket.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace CppUtils2
{
  namespace net
  {

    class Socket : public ISocket
    {
    public:
      Socket(std::int32_t fd_socket) : fd_socket(fd_socket) {};

      void disconnect() override
      {
        if (fd_socket > 0)
        {
          close(fd_socket);
        }
      }

      ssize_t write_data(const void *data, ssize_t size) override
      {
        if (-1 == fd_socket)
        {
          SPDLOG_WARN("Wrong socket id");
        }
        ssize_t n = send(fd_socket, data, size, MSG_NOSIGNAL);
        return n;
      }

      bool write_string(const std::string &data) override
      {
        ssize_t n = write_data(data.c_str(), data.size());
        return (n == static_cast<ssize_t>(data.size()));
      }

      ssize_t read_data(void *buffer, ssize_t size) const override
      {
        // SPDLOG_DEBUG("Read data. Up to {0} bytes", size);
        if (-1 == fd_socket)
        {
          SPDLOG_WARN("Wrong socket FD -1");
        }
        char *ptrBuffer = static_cast<char *>(buffer);
        ssize_t current_len = 0;
        ssize_t len = 0;
        do
        {
          len = recv(fd_socket, ptrBuffer, size - current_len, MSG_NOSIGNAL);
          // SPDLOG_DEBUG("Read {0} bytes", len);
          if (len < 0)
          {
            // Client disconnected
            SPDLOG_WARN("Client {0} disconnected", fd_socket);
            close(fd_socket);
          }
          current_len += len;
          ptrBuffer += len;
        } while (current_len != size && len > 0);

        return current_len;
      }

      bool read_string(std::string &data) const override
      {

        char buffer[2048]{};
        std::size_t n = read_data(buffer, sizeof(buffer));
        if ((n > 0) && (n < sizeof(buffer)))
        {
          buffer[n] = '\0';
          data = std::string(buffer);
        }

        return (n == static_cast<size_t>(data.size()));
      }

      std::int32_t get_socket_fd() const override
      {
        return fd_socket;
      }

      virtual ~Socket() = default;

    protected:
      std::int32_t fd_socket;
    };
  } // namespace net
} // namespace CppUtils2
