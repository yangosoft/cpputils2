#pragma once

#include "cpputils2/net/socket/isocket.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <cstdio>
#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace CppUtils2
{
  namespace net
  {

    class UDSClient : public ISocket
    {
    public:
      UDSClient(const std::string &server_path) : ISocket(), server_path(server_path)
      {
        int sock;

        /* Create socket on which to send. */
        sock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sock < 0)
        {
          SPDLOG_ERROR("Error opening datagram socket");
        }
        /* Construct name of socket to send to. */
        serv_addr.sun_family = AF_UNIX;
        strcpy(serv_addr.sun_path, server_path.c_str());

        fd_socket = sock;
      }

      void disconnect() override
      {
        close(fd_socket);
        fd_socket = -1;
      }

      ssize_t send_data(const void *data, ssize_t size)
      {
        if (-1 == fd_socket)
        {
          SPDLOG_WARN("Wrong socket id");
        }
        // ssize_t n = sendto(fd_socket, data, size, MSG_NOSIGNAL, reinterpret_cast<sockaddr*>(&serv_addr),
        // sizeof(serv_addr));
        SPDLOG_INFO("Sending data to {0}", server_path);
        auto n = sendto(fd_socket, data, size, 0, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(sockaddr_un));
        if (n < 0)
        {
          SPDLOG_ERROR("Error sending data");
        }

        return n;
      }

      ssize_t write_data(const void *data, ssize_t size) override
      {
        return send_data(data, size);
      }

      bool write_string(const std::string &data) override
      {
        auto sent = write_data(data.data(), data.size());
        return sent == static_cast<ssize_t>(data.size());
      }

      ssize_t read_data(void *buffer, ssize_t size) const override
      {
        return -1;
      };

      bool read_string(std::string &data) const override
      {
        char buffer[256];
        auto readed = read_data(buffer, sizeof(buffer));
        data = std::string(buffer);
        return readed > 0;
      };

      virtual std::int32_t get_socket_fd() const override
      {
        return fd_socket;
      }

      std::string get_host() const
      {
        return server_path;
      }

      std::string get_port() const
      {
        return server_path;
      }

      virtual ~UDSClient() = default;

    private:
      std::int32_t fd_socket;

      std::string server_path;
      sockaddr_un serv_addr;
    };
  } // namespace net
} // namespace CppUtils2
