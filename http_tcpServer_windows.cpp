#include <http_tcpServer_windows.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
namespace
{
    const int BUFFER_SIZE = 30720;
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
        m_socketAddress.sin_family = AF_INET;                              // socket Family
        m_socketAddress.sin_port = htons(m_port);                          // htons ensures server uses NETWORK BYTE ORDER Use IBM documentation
        m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str()); // server addresss inet converts address from CHAR* to UNSIGNED INT
        if (startServer() != 0)
        {
            std::ostringstream ss;
            ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
            log(ss.str());
        }
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
        if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0) // To bind socket to the address
        {
            exitWithError("Cannot connect socket to address");
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
    void TcpServer::startListen()
    {
        if (listen(m_socket, 20) < 0)
        {
            exitWithError("Socket Listen Failed");
        }
        std::ostringstream ss;
        ss << "\n*** Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << " PORT: " << ntohs(m_socketAddress.sin_port) << " ***\n\n";
        log(ss.str());

        int bytesReceived;
        char buffer[BUFFER_SIZE] = {0};
        bytesReceived = recv(m_new_socket, buffer, BUFFER_SIZE, 0); // USED TO READ THE REQUEST 0 = FLAG FOR WINDOWS
        if (bytesReceived < 0)
        {
            exitWithError("FAILED TO RECIEVE BYTES FROM CLIENT SOCKET CONNECTION");
        }
    }
    void TcpServer ::acceptConnection(SOCKET &new_socket)
    {
        new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len); // System Call to accept new socket
        if (new_socket < 0)
        {
            std::ostringstream ss;
            ss << "Server Failed to accept incoming connection from ADDRESS " << inet_ntoa(m_socketAddress.sin_addr) << "PORT : " << ntohs(m_socketAddress.sin_port);
            exitWithError(ss.str());
        };
    }

} // namespace http