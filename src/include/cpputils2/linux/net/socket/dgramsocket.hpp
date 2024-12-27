#pragma once

#include "cpputils2/net/socket/isocket.hpp"

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

    class DSocket : public ISocket
    {
    public:
      DSocket(std::int32_t fd_socket, sockaddr *client_addr, sockaddr *server_addr)
          : fd_socket(fd_socket), client_addr(client_addr), server_addr(server_addr) {};

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
        ssize_t n = sendto(fd_socket, data, size, MSG_NOSIGNAL, server_addr, sizeof(server_addr));
        return n;
      }

      bool write_string(const std::string &data) override
      {
        ssize_t n = write_data(data.c_str(), data.size());

        SPDLOG_INFO("Written {0} vs size {1}", n, data.size());

        return (n == static_cast<ssize_t>(data.size()));
      }

      ssize_t read_data(void *buffer, ssize_t size) const override
      {
        if (-1 == fd_socket)
        {
          SPDLOG_WARN("Wrong socket FD -1");
        }

        socklen_t slen = sizeof(client_addr);

        ssize_t len = recvfrom(fd_socket, buffer, size, MSG_NOSIGNAL, client_addr, &slen);
        if (len <= 0)
        {
          // Client disconnected
          SPDLOG_WARN("Client {0} disconnected", fd_socket);
          close(fd_socket);
        }

        return len;
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

      virtual ~DSocket() = default;

    protected:
      std::int32_t fd_socket;
      sockaddr *client_addr;
      sockaddr *server_addr;
    };
  } // namespace net
} // namespace CppUtils2
