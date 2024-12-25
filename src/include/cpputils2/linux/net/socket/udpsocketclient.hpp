#pragma once

#include "cpputils2/linux/net/socket/socket.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <string>

#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace CppUtils2
{
  namespace net
  {

    class UDPSocketClient
    {
    public:
      UDPSocketClient() : sock_fd(-1) {}

      UDPSocketClient(const std::string &hostname, std::uint16_t port) : host(hostname), port(port)
      {
        init_socket();
      }

      void init_socket()
      {
        // Creating socket file descriptor
        if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
          SPDLOG_ERROR("Error initialitzing socket. Error: {0}", sock_fd);
        }
        hostent *he = nullptr;
        memset(&serv_addr, 0, sizeof(serv_addr));

        // Filling server information
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        he = gethostbyname(host.c_str());
        if (he == nullptr)
        {
          SPDLOG_ERROR("Error gethostbyname()");
        }

        int flags = fcntl(sock_fd, F_GETFL, 0);
        fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);

        serv_addr.sin_addr = *(reinterpret_cast<in_addr *>(he->h_addr));
      }

      std::size_t send_data(const std::string &host, std::uint16_t port, void *buffer, ssize_t size)
      {
        this->host = host;
        this->port = port;
        init_socket();
        return send_data(buffer, size);
      }

      std::size_t send_data(void *buffer, ssize_t size)
      {
        if (sock_fd == -1)
        {
          return sock_fd;
        }

        auto ret = sendto(sock_fd, buffer, size, MSG_CONFIRM, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr));
        return ret;
      }

      std::string get_host() const
      {
        return host;
      }

      std::uint16_t get_port() const
      {
        return port;
      }

      virtual ~UDPSocketClient() = default;

    private:
      std::int32_t sock_fd;
      sockaddr_in serv_addr;
      std::string host;
      std::uint16_t port;
    };
  } // namespace net
} // namespace CppUtils2