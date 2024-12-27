#pragma once
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#include "cpputils2/common/types.hpp"
#include "cpputils2/win/net/socket/socket.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

namespace CppUtils2
{
  namespace net
  {

    class TCPSocketClient : public Socket
    {
    public:
      TCPSocketClient() : Socket() {};

      TCPSocketClient(std::string hostname, std::uint16_t port) : Socket(), host(std::move(hostname)), port(port) {};

      virtual Result connect(const std::string &hostname, std::uint16_t port)
      {
        host = hostname;
        this->port = port;
        return connect();
      }

      virtual Result connect()
      {
        addrinfo *result = nullptr,
                 *ptr = nullptr,
                 hints;

        // Initialize Winsock
        auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
          // printf("WSAStartup failed with error: %d\n", iResult);
          return Result::RET_ERROR;
        }
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        iResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result);
        if (iResult != 0)
        {
          // printf("getaddrinfo failed with error: %d\n", iResult);
          WSACleanup();
          return Result::RET_ERROR;
        }
        // Attempt to connect to an address until one succeeds
        for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
        {

          // Create a SOCKET for connecting to server
          sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
          if (sock == INVALID_SOCKET)
          {
            // printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return Result::RET_ERROR;
          }

          // Connect to server.
          iResult = ::connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
          if (iResult == SOCKET_ERROR)
          {
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
          }
          break;
        }

        freeaddrinfo(result);

        if (sock == INVALID_SOCKET)
        {
          return Result::RET_ERROR;
        }
        return Result::RET_OK;
      }

      virtual ~TCPSocketClient()
      {
        disconnect();
      };

    private:
      std::string host;
      std::uint16_t port;
      WSADATA wsaData;
    };
  } // namespace net
} // namespace CppUtils2
