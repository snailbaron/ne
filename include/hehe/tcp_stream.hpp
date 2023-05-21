#pragma once

#include <hehe/tcp_streambuf.hpp>

#include <cstdint>
#include <istream>
#include <string>
#include <string_view>

namespace hehe {

class TcpStream : public std::iostream {
public:
    TcpStream(std::string_view address, uint16_t port);

    TcpStream(TcpStream&& other) noexcept;
    TcpStream& operator=(TcpStream&& other) noexcept;

    std::string peer() const { return _streambuf.peer(); }
    int fd() const { return _streambuf.fd(); }

private:
    explicit TcpStream(int fd);

    TcpStreambuf _streambuf;

    friend class TcpListen;
};

} // namespace hehe
