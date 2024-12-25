#pragma once

#include "cpputils2/linux/net/socket/socket.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <functional>
#include <memory>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace CppUtils2
{
  namespace net
  {

    class TCPSocketServer
    {
    public:
      using OnNewClientCallback = std::function<void(std::unique_ptr<ISocket> fdClient)>;
      TCPSocketServer(std::uint16_t port, OnNewClientCallback onNewClientCallback)
          : port(port), fn_callback(std::move(onNewClientCallback))
      {
        std::int32_t fdSck = 0;
        sockaddr_in serv_addr{};

        fdSck = socket(AF_INET, SOCK_STREAM, 0);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(port);

        std::int32_t status = bind(fdSck, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr));
        if (0 != status)
        {
          SPDLOG_ERROR("Something went wrong in bind");
          fd_socket = -1;
          return;
        }
        fd_socket = fdSck;
      }

      TCPSocketServer(const TCPSocketServer &other)
      {
        fd_socket = other.fd_socket;
        port = other.port;
        fn_callback = other.fn_callback;
      }

      TCPSocketServer &operator=(const TCPSocketServer &other)
      {
        fd_socket = other.fd_socket;
        port = other.port;
        fn_callback = other.fn_callback;

        return *this;
      }

      virtual std::int32_t listen()
      {
        std::int32_t status = ::listen(fd_socket, 1024);
        if (0 != status)
        {
          SPDLOG_ERROR("Something went wrong in listen");
        }

        return status;
      }

      virtual void do_accept()
      {
        std::int32_t fdClient = accept(fd_socket, nullptr, nullptr);
        if (fdClient > 0)
        {
          auto s = std::make_unique<Socket>(fdClient);
          fn_callback(std::move(s));
        }
      }

      virtual std::int32_t get_socket_fd() const
      {
        return fd_socket;
      }

      virtual void disconnect()
      {
        shutdown(fd_socket, SHUT_RD);
        close(fd_socket);
        fd_socket = -1;
      }

      virtual bool is_listening() const
      {
        return fd_socket > 0;
      }

      virtual ~TCPSocketServer() = default;

    protected:
      std::uint16_t port;
      std::int32_t fd_socket;
      OnNewClientCallback fn_callback;
    };
  } // namespace net
} // namespace CppUtils2
