#pragma once

#include <hehe/tcp_connection.hpp>

#include <istream>
#include <streambuf>
#include <vector>

namespace hehe {

class TcpStreambuf : public std::streambuf {
public:
    // TODO: automatically set unitbuf in constructor, like cerr does. Thus,
    // by default, any written data will be immediately flushed to the socket.

    explicit TcpStreambuf(
        TcpConnection&& tcpConnection, size_t bufferSize = 1024);

    // Leaving these virtual functions
    // * imbue - no need for locale support
    // * setbuf - user-provided buffer is not supported
    // * seekoff - relative positioning is not supported
    // * seekpos - absolute positioning is not supported
    // * showmanyc - we don't know how many characters are available
    // * uflow - seems to work fine when underflow() is properly defined
    // * xsgetn - works as is, although may not be optimal
    // * xsputn - works as is, although may not be optimal

    int sync() override;
    int underflow() override;
    int overflow(int ch) override;
    int pbackfail(int c) override;

private:
    TcpConnection _connection;
    std::vector<char> _getArea;
    std::vector<char> _putArea;
};

class TcpStream : public std::iostream {
public:
    TcpStream(std::string_view address, uint16_t port);

private:
    explicit TcpStream(int fd);

    TcpStreambuf _streambuf;

    friend class TcpListen;
};

} // namespace hehe
