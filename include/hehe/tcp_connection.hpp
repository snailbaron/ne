#pragma once

#include <cstddef>
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

    size_t read(std::span<char> target);

    void write(const char* buffer, size_t size);
    void write(const unsigned char* buffer, size_t size);
    void write(const signed char* buffer, size_t size);
    void write(const std::byte* buffer, size_t size);

    void write(std::span<const char> data);
    void write(std::span<const unsigned char> data);
    void write(std::span<const signed char> data);
    void write(std::span<const std::byte> data);

    void write(std::string_view string);

    std::string peer() const;

    int fd() const { return _fd; }

private:
    explicit TcpConnection(int fd);

    int _fd = -1;

    friend class TcpStream;
};

} // namespace hehe
