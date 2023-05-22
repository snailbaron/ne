#pragma once

#include <ne/tcp_streambuf.hpp>

#include <cstdint>
#include <istream>
#include <string>
#include <string_view>

namespace ne {

class TcpStream : public std::iostream {
public:
    TcpStream(std::string_view address, uint16_t port);

    TcpStream(TcpStream&& other) noexcept;
    TcpStream& operator=(TcpStream&& other) noexcept;

    std::string peer() const { return _streambuf.peer(); }

private:
    explicit TcpStream(int fd);

    TcpStreambuf _streambuf;

    friend class TcpListen;
};

} // namespace ne
