#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace hehe {

class TcpConnection {
public:
    TcpConnection(std::string_view address, uint16_t port);
    ~TcpConnection();

    TcpConnection(const TcpConnection&) = delete;
    TcpConnection& operator=(const TcpConnection&) = delete;

    TcpConnection(TcpConnection&&) noexcept;
    TcpConnection& operator=(TcpConnection&&) noexcept;

    bool read(std::span<char> target);

    void write(std::span<const std::byte> data);
    void write(std::string_view string);

private:
    explicit TcpConnection(int fd);

    int _fd = -1;

    friend class TcpStream;
};

} // namespace hehe
