#pragma once

#include <ne/tcp_stream.hpp>

#include <string>

namespace ne {

class TcpListen {
public:
    TcpListen(uint16_t port = 0);
    ~TcpListen();

    TcpListen(const TcpListen&) = delete;
    TcpListen& operator=(const TcpListen&) = delete;

    TcpListen(TcpListen&& other) noexcept;
    TcpListen& operator=(TcpListen&& other) noexcept;

    std::string name() const;
    TcpStream accept();

private:
    int _fd = -1;
};

} // namespace ne
