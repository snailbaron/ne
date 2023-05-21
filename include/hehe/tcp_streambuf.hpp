#pragma once

#include <hehe/tcp_connection.hpp>

#include <ios>
#include <locale>
#include <streambuf>
#include <string>
#include <vector>

namespace hehe {

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
    int fd() const { return _connection.fd(); }

    // Unused virtual functions:
    void imbue(const std::locale& loc) override;
    std::streambuf* setbuf(char_type* s, std::streamsize n) override;
    std::streampos seekoff(
        off_type off,
        std::ios_base::seekdir dir,
        std::ios_base::openmode which =
            std::ios_base::in | std::ios_base::out) override;
    std::streampos seekpos(
        std::streampos pos,
        std::ios_base::openmode which =
            std::ios_base::in | std::ios_base::out) override;
    std::streamsize showmanyc() override;
    int uflow() override;
    std::streamsize xsgetn(char_type* s, std::streamsize count) override;
    std::streamsize xsputn(const char_type* s, std::streamsize count) override;

private:
    TcpConnection _connection;
    std::vector<char> _getArea;
    std::vector<char> _putArea;
};

} // namespace hehe
