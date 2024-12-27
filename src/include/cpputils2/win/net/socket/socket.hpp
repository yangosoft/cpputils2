#pragma once

#include "cpputils2/net/socket/isocket.hpp"

#include <winsock2.h>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

namespace CppUtils2
{
    namespace net
    {

        class Socket : public ISocket
        {
        public:
            Socket() : sock(INVALID_SOCKET) {};

            Socket(SOCKET s) : sock(s) {};

            void disconnect() override
            {
                if (sock != INVALID_SOCKET)
                {
                    auto iResult = shutdown(sock, SD_SEND);

                    // printf("shutdown failed with error: %d\n", WSAGetLastError());
                    closesocket(sock);
                    WSACleanup();
                    sock = INVALID_SOCKET;
                }
            }

            ssize_t write_data(const void *data, ssize_t size) override
            {
                auto iResult = send(sock, reinterpret_cast<const char *>(data), size, 0);
                if (iResult == SOCKET_ERROR)
                {
                    return -1;
                }
                return iResult;
            }

            bool write_string(const std::string &data) override
            {
                return write_data(data.c_str(), data.size()) != -1;
            }

            ssize_t read_data(void *buffer, ssize_t size) const override
            {
                auto iResult = recv(sock, reinterpret_cast<char *>(buffer), size, 0);
                if (iResult == SOCKET_ERROR)
                {
                    return -1;
                }

                return iResult;
            }

            bool read_string(std::string &data) const override
            {
                char buffer[1024];
                int iResult;
                do
                {
                    iResult = recv(sock, buffer, 1024, 0);
                    if (iResult > 0)
                    {
                        data.append(buffer, iResult);
                    }
                    else if (iResult == 0)
                    {
                        return false;
                    }
                    else
                    {
                        return false;
                    }
                } while (iResult != 0);

                return true;
            }

            std::int32_t get_socket_fd() const override
            {
                return 0;
            }

            SOCKET get_socket_handle() const
            {
                return sock;
            }

            virtual ~Socket()
            {
                disconnect();
            };

        protected:
            SOCKET sock;
        };
    } // namespace net
} // namespace CppUtils2
