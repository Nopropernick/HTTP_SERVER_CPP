#ifndef INCLUDED_HTTP_TCPSERVER_WINDOWS
#define INCLUDED_HTTP_TCPSERVER_WINDOWS

#include <stdio.h>
#include <string>
// Windows-specific includes
#include <winsock.h>
#include <stdlib.h>
// Constructor and Destructor
namespace http
{
    class TcpServer
    {
    public:
        TcpServer(std::string ip_address, int port);
        ~TcpServer();
        void startListen();

    private:
        std::string m_ip_address;
        int m_port;
        SOCKET m_socket;     // socket for member variable
        SOCKET m_new_socket; // cannot use INT in windows, needs SOCKET = unsigned INT
        long m_incomingMessage;
        struct sockaddr_in m_socketAddress;
        int m_socketAddress_len;
        std::string m_serverMessage;
        WSADATA m_wsaData; // has to be used in windows to show which version is being used

        int startServer();
        void closeServer();
        void acceptConnection(SOCKET &new_socket);
        std::string buildResponse();
        void sendResponse();
    };
} // namespace http
#endif
