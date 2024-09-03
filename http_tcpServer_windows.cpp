#include <http_tcpServer_windows.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
namespace
{
    void log(const std::string &message)
    {
        std::cout << message << std::endl; // error handling I am too lazy to change the method
    }
    void exitWithError(const std::string &errorMessage)
    {
        std::cout << WSAGetLastError() << std::endl;
        log("ERROR: " + errorMessage);
        exit(1);
    }
}
namespace http
{
    TcpServer::TcpServer(std::string ip_address, int port) : m_ip_address(ip_address), m_port(port), m_socket(),
                                                             m_new_socket(), m_incomingMessage(), m_socketAddress(),
                                                             m_socketAddress_len(sizeof(m_socketAddress)),
                                                             m_serverMessage(buildResponse()), m_wsaData()
    {
        startServer();
    }
    TcpServer::~TcpServer()
    {
        closeServer();
    }

    int TcpServer::startServer()
    {
        if (WSAStartup(MAKEWORD(2, 0), &m_wsaData) != 0)
        {
            exitWithError("WSAStartup Failed");
        }
        m_socket = socket(AF_INET, SOCK_STREAM, 0); // IPV4 Protocols are defined by AF_INET DOMAIN, And SOCK_STEAM ALLOWS FOR FULL DUPLEX TYPE SYSTEM , 0 = protcol
        if (m_socket < 0)
        {
            exitWithError("Cannot Create Socket");
            return 1;
        }
        return 0;
    }
    void TcpServer ::closeServer()
    {
        closesocket(m_socket);
        closesocket(m_new_socket);
        WSACleanup();
        exit(0);
    }

} // namespace http