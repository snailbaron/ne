#pragma once

#include <ne/tcp_connection.hpp>

#include <ios>
#include <locale>
#include <streambuf>
#include <string>
#include <vector>

namespace ne {

class TcpStreambuf : public std::streambuf {
public:
    // TODO: automatically set unitbuf in constructor, like cerr does. Thus,
    // by default, any written data will be immediately flushed to the socket.

    explicit TcpStreambuf(
        TcpConnection&& tcpConnection, size_t bufferSize = 1024);

    int sync() override;
    int underflow() override;
    int overflow(int ch) override;
    int pbackfail(int c) override;

    std::string peer() const { return _connection.peer(); }

private:
    TcpConnection _connection;
    std::vector<char> _getArea;
    std::vector<char> _putArea;
};

} // namespace ne
