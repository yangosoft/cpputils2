#pragma once

#include "cpputils2/linux/net/socket/socket.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <string>

#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace CppUtils2
{
  namespace net
  {

    class TCPSocketClient : public Socket
    {
    public:
      TCPSocketClient() : Socket(-1) {};

      TCPSocketClient(std::string hostname, std::uint16_t port) : Socket(-1), host(std::move(hostname)), port(port) {};

      virtual std::int32_t connect(const std::string &hostname, std::uint16_t port)
      {
        host = hostname;
        this->port = port;
        return connect();
      }

      virtual std::int32_t connect()
      {
        int sockfd = 0;

        sockaddr_in serv_addr{};
        hostent *he = nullptr;

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
          SPDLOG_ERROR("Cannot create socket");
          return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        he = gethostbyname(host.c_str());
        if (he == nullptr)
        {
          SPDLOG_ERROR("Error gethostbyname()");
          return -1;
        }

        serv_addr.sin_addr = *(reinterpret_cast<in_addr *>(he->h_addr));

        auto ret = ::connect(sockfd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr));
        if (ret < 0)
        {
          SPDLOG_ERROR("Error connecting socket. Error: {0}", ret);
          return ret;
        }
        fd_socket = sockfd;
        return sockfd;
      }

      virtual ~TCPSocketClient() = default;

    private:
      std::string host;
      std::uint16_t port;
    };
  } // namespace net
} // namespace CppUtils2
