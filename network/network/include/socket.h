#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>

#include <stdexcept>
#include <string>

namespace VNS {

class Socket {
private:
    int m_sock;
    int m_port;

public:
    int& sock() { return m_sock; }
    const int& sock() const { return m_sock; }

private:
    bool Valid() { return m_sock != -1; }

public:
    Socket() : m_sock(-1), m_port(-1) {}
    Socket(int sock);
    ~Socket() {}

    /**
     * Opens a socket at a given port.
     */
    int Open(const int port);

    /**
     * Accepts a connection.
     */
    int Accept();

    int Connect(const std::string& ip, const int port);
    void Close();

    ssize_t Read(char* buf, ssize_t size);
    void Send(const std::string& message);
};

}  // namespace VNS

#endif  // SOCKET_H
