#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include "cpputils2/common/types.hpp"
#include "cpputils2/win/net/socket/socket.hpp"

#include <windows.h>
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

    class TCPSocketServer
    {
    public:
      using OnNewClientCallback = std::function<void(std::unique_ptr<ISocket> fdClient)>;
      TCPSocketServer(std::uint16_t port, OnNewClientCallback onNewClientCallback)
          : sock(INVALID_SOCKET), port(port), fn_callback(std::move(onNewClientCallback))
      {
        addrinfo *result;
        addrinfo hints;

        // Initialize Winsock
        auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
          // printf("WSAStartup failed with error: %d\n", iResult);
          return;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        iResult = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &result);
        if (iResult != 0)
        {
          // printf("getaddrinfo failed with error: %d\n", iResult);
          WSACleanup();
          return;
        }

        // Create a SOCKET for the server to listen for client connections.
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (sock == INVALID_SOCKET)
        {
          // printf("socket failed with error: %ld\n", WSAGetLastError());
          freeaddrinfo(result);
          WSACleanup();
          return;
        }
        // Setup the TCP listening socket
        iResult = bind(sock, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
          // printf("bind failed with error: %d\n", WSAGetLastError());
          freeaddrinfo(result);
          closesocket(sock);
          WSACleanup();
          sock = INVALID_SOCKET;
          return;
        }

        freeaddrinfo(result);
      }

      TCPSocketServer(const TCPSocketServer &other)
      {
        sock = other.sock;
        port = other.port;
        fn_callback = other.fn_callback;
      }

      TCPSocketServer &operator=(const TCPSocketServer &other)
      {
        sock = other.sock;
        port = other.port;
        fn_callback = other.fn_callback;

        return *this;
      }

      virtual std::int32_t listen()
      {

        auto iResult = ::listen(sock, SOMAXCONN);
        if (iResult == SOCKET_ERROR)
        {
          // printf("listen failed with error: %d\n", WSAGetLastError());
          closesocket(sock);
          WSACleanup();
          sock = INVALID_SOCKET;
          return -1;
        }
        return 0;
      }

      virtual void do_accept()
      {
        // std::int32_t fdClient = accept(fd_socket, nullptr, nullptr);
        // if (fdClient > 0)
        //{
        //  auto s = std::make_unique<Socket>(fdClient);
        // fn_callback(std::move(s));
        //}

        // Accept a client socket
        SOCKET ClientSocket = accept(sock, nullptr, nullptr);
        if (ClientSocket == INVALID_SOCKET)
        {
          // printf("accept failed with error: %d\n", WSAGetLastError());
          closesocket(sock);
          WSACleanup();
          sock = INVALID_SOCKET;
          return;
        }
        auto s = std::make_unique<Socket>(ClientSocket);
        fn_callback(std::move(s));
      }

      virtual std::int32_t get_socket_fd() const
      {
        return -1;
      }

      Socket get_socket_handle() const
      {
        return sock;
      }

      virtual void disconnect()
      {
        if (sock != INVALID_SOCKET)
        {
          shutdown(sock, SD_SEND);
          closesocket(sock);
          WSACleanup();
          sock = INVALID_SOCKET;
        }
      }

      virtual bool is_listening() const
      {
        return sock != INVALID_SOCKET;
      }

      virtual ~TCPSocketServer()
      {
        disconnect();
      }

    protected:
      SOCKET sock;
      std::uint16_t port;
      OnNewClientCallback fn_callback;
      WSADATA wsaData;
    };
  } // namespace net
} // namespace CppUtils2
