#pragma once

#include <hehe/tcp_stream.hpp>

#include <string>

namespace hehe {

class TcpListen {
public:
    TcpListen();
    ~TcpListen();

    TcpListen(const TcpListen&) = delete;
    TcpListen& operator=(const TcpListen&) = delete;

    TcpListen(TcpListen&&) = default;
    TcpListen& operator=(TcpListen&&) = default;

    std::string name() const;
    TcpStream accept();

private:
    int _fd = -1;
};

} // namespace hehe
