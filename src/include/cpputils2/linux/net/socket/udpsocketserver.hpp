#pragma once

#include "cpputils2/common/types.hpp"
#include "cpputils2/net/socket/isocket.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace CppUtils2
{
  namespace net
  {

    class UDPSocketServer
    {
    public:
      UDPSocketServer() : serv_addr{}, cli_addr{}, sock_fd(-1) {}

      UDPSocketServer(std::uint16_t port) : serv_addr{}, cli_addr{}, sock_fd(-1)
      {
        init(port);
      }

      Result init(std::uint16_t port)
      {
        // Creating socket file descriptor
        if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
          SPDLOG_ERROR("Something went wrong creating the socket. Error {0}", errno);
          return Result::RET_ERROR;
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        memset(&cli_addr, 0, sizeof(cli_addr));

        // Filling server information
        serv_addr.sin_family = AF_INET; // IPv4
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        // Bind the socket with the server address
        if (bind(sock_fd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0)
        {
          SPDLOG_ERROR("Something went wrong in bind. Error {0}", errno);
          sock_fd = -1;
          return Result::RET_ERROR;
        }
        return Result::RET_OK;
      }

      void disconnect()
      {
        shutdown(sock_fd, SHUT_RDWR);
        close(sock_fd);
      }

      ssize_t read_data(void *buffer, ssize_t size)
      {
        socklen_t len = sizeof(cli_addr); // len is value/result

        auto read_bytes = recvfrom(sock_fd, buffer, size, MSG_WAITALL, reinterpret_cast<sockaddr *>(&cli_addr), &len);
        return read_bytes;
      }

      std::int32_t get_socket_fd() const
      {
        return sock_fd;
      }

    private:
      sockaddr_in serv_addr;
      sockaddr_in cli_addr;
      std::int32_t sock_fd;
    };
  } // namespace net
} // namespace CppUtils2
