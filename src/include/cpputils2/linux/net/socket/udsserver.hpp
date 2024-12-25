#pragma once

#include "cpputils2/linux/net/socket/dgramsocket.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

namespace CppUtils2
{
  namespace net
  {

    class UDSServer
    {
    public:
      UDSServer() : fd_socket(-1) {}

      UDSServer(const std::string &path) : path(path)
      {
        init(path);
      }

      UDSServer(const UDSServer &other)
      {
        fd_socket = other.fd_socket;
        path = other.path;
      }

      UDSServer &operator=(const UDSServer &other)
      {
        fd_socket = other.fd_socket;
        path = other.path;
        return *this;
      }

      Result init(const std::string_view path)
      {
        std::int32_t fdSck = 0;

        memset(&serv_addr, 0, sizeof(serv_addr));

        fdSck = socket(AF_UNIX, SOCK_DGRAM, 0);

        serv_addr.sun_family = AF_UNIX;

        strcpy(serv_addr.sun_path, path.data());
        unlink(serv_addr.sun_path);

        auto status = bind(fdSck, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr));
        if (-1 == status)
        {
          SPDLOG_ERROR("Something went wrong in bind {0}", errno);
          close(fdSck);
          return Result::RET_ERROR;
        }
        fd_socket = fdSck;
        SPDLOG_INFO("Listening on {0}", path);
        return Result::RET_OK;
      }

      virtual std::int32_t listen()
      {
        return 0;
      }

      virtual ssize_t read_data(void *buffer, ssize_t buffer_size)
      {
        SPDLOG_DEBUG("Reading data from socket {0} bytes", buffer_size);
        sockaddr_un client_addr;
        int length = sizeof(client_addr);
        auto recv_size = recvfrom(fd_socket, buffer, buffer_size, MSG_WAITALL, reinterpret_cast<sockaddr *>(&client_addr),
                                  reinterpret_cast<socklen_t *>(&length));
        SPDLOG_DEBUG("Readed {0} bytes of {1}", recv_size, buffer_size);
        return recv_size;
      };

      virtual std::int32_t get_socket_fd() const
      {
        return fd_socket;
      };

      virtual void disconnect()
      {
        shutdown(fd_socket, SHUT_RD);
        close(fd_socket);
      };

      virtual ~UDSServer() = default;

    protected:
      std::string path;
      std::int32_t fd_socket;
      sockaddr_un serv_addr;
    };
  } // namespace net
} // namespace CppUtils2
