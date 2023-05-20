#include <hehe/tcp_connection.hpp>

#include <hehe/error.hpp>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>

namespace hehe {

TcpConnection::TcpConnection(std::string_view address, uint16_t port)
{
    addrinfo* info = nullptr;
    {
        const auto desiredInfo = addrinfo{
            .ai_family = AF_UNSPEC,
            .ai_socktype = SOCK_STREAM,
        };
        if (auto code = getaddrinfo(
                std::string{address}.c_str(),
                std::to_string(port).c_str(),
                &desiredInfo,
                &info); code != 0) {
            throw Error{gai_strerror(code)};
        }
    }

    auto errors = std::ostringstream{};
    for (; info != nullptr; info = info->ai_next) {
        _fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (_fd == -1) {
            auto errorCode = errno;
            errors << std::strerror(errorCode) << "\n";
            continue;
        }

        if (connect(_fd, info->ai_addr, info->ai_addrlen) == -1) {
            auto errorCode = errno;
            errors << " * " << std::strerror(errorCode) << "\n";
            continue;
        }

        break;
    }

    if (info == nullptr) {
        throw Error{"could not connect:\n" + errors.str()};
    }

    freeaddrinfo(info);
}

TcpConnection::TcpConnection(int fd)
    : _fd{fd}
{ }

TcpConnection::TcpConnection(TcpConnection&& other) noexcept
{
    std::swap(_fd, other._fd);
}

TcpConnection& TcpConnection::operator=(TcpConnection&& other) noexcept
{
    if (_fd != -1) {
        close(_fd);
    }
    _fd = -1;

    std::swap(_fd, other._fd);

    return *this;
}

TcpConnection::~TcpConnection()
{
    if (_fd != -1) {
        close(_fd);
    }
}

bool TcpConnection::read(std::span<char> target)
{
    auto result = errnoCheck(::read(_fd, target.data(), target.size()));
    return result > 0;
}

void TcpConnection::write(std::span<const std::byte> data)
{
    size_t written = 0;
    size_t left = data.size();
    for (;;) {
        auto bytesWritten =
            errnoCheck(::write(_fd, data.data() + written, left));
        if (bytesWritten == left) {
            break;
        }

        written += bytesWritten;
        left -= bytesWritten;
    }
}

void TcpConnection::write(std::string_view string)
{
    write(std::span<const std::byte>{
        reinterpret_cast<const std::byte*>(string.data()),
        string.size()});
}

} // namespace hehe
