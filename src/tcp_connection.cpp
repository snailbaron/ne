#include <hehe/tcp_connection.hpp>

#include <hehe/error.hpp>
#include <hehe/io.hpp>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>

#include <iostream>

namespace hehe {

TcpConnection::TcpConnection(std::string_view address, uint16_t port)
{
    connect(address, port);
}

TcpConnection::TcpConnection(int fd)
    : _fd{fd}
{
    std::cout << "starting to manage socket " << fd <<
        " as a TCP connection" << std::endl;
}

TcpConnection::TcpConnection(TcpConnection&& other) noexcept
{
    std::swap(_fd, other._fd);
}

TcpConnection& TcpConnection::operator=(TcpConnection&& other) noexcept
{
    this->disconnect();
    std::swap(_fd, other._fd);
    return *this;
}

TcpConnection::~TcpConnection()
{
    this->disconnect();
}

void TcpConnection::connect(std::string_view address, uint16_t port)
{
    addrinfo* info = nullptr;
    {
        const auto desiredInfo = addrinfo{
            .ai_family = AF_UNSPEC,
            .ai_socktype = SOCK_STREAM,
        };
        if (auto code = ::getaddrinfo(
                std::string{address}.c_str(),
                std::to_string(port).c_str(),
                &desiredInfo,
                &info); code != 0) {
            throw Error{::gai_strerror(code)};
        }
    }

    auto errors = std::ostringstream{};
    for (; info != nullptr; info = info->ai_next) {
        _fd = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (_fd == -1) {
            auto errorCode = errno;
            errors << std::strerror(errorCode) << "\n";
            continue;
        }
        std::cout << "created connection socket " << _fd << std::endl;

        if (::connect(_fd, info->ai_addr, info->ai_addrlen) == -1) {
            auto errorCode = errno;
            errors << " * " << std::strerror(errorCode) << "\n";
            continue;
        }
        std::cout << "connected socket " << _fd << std::endl;

        break;
    }

    if (info == nullptr) {
        throw Error{"could not connect:\n" + errors.str()};
    }

    ::freeaddrinfo(info);
}

void TcpConnection::disconnect()
{
    if (_fd != -1) {
        std::cout << "shutting down socket " << _fd << std::endl;
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
        _fd = -1;
    }
}

bool TcpConnection::connected() const
{
    return _fd != -1;
}

size_t TcpConnection::read(std::span<char> target)
{
    if (!this->connected()) {
        throw Error{"TcpConnection::read called for disconnected socket"};
    }

    size_t bytesRead = errnoCheck(::read(_fd, target.data(), target.size()));
    if (bytesRead == 0) {
        disconnect();
    }
    return bytesRead;
}

void TcpConnection::write(const char* buffer, size_t size)
{
    if (!this->connected()) {
        throw Error{"TcpConnection::write called for disconnected socket"};
    }

    size_t written = 0;
    for (;;) {
        auto bytesWritten =
            errnoCheck(::write(_fd, buffer + written, size - written));
        written += bytesWritten;
        if (written >= size) {
            break;
        }
    }
}

void TcpConnection::write(const unsigned char* buffer, size_t size)
{
    write(reinterpret_cast<const char*>(buffer), size);
}

void TcpConnection::write(const signed char* buffer, size_t size)
{
    write(reinterpret_cast<const char*>(buffer), size);
}

void TcpConnection::write(const std::byte* buffer, size_t size)
{
    write(reinterpret_cast<const char*>(buffer), size);
}

void TcpConnection::write(std::span<const char> data)
{
    write(data.data(), data.size());
}

void TcpConnection::write(std::span<const unsigned char> data)
{
    write(data.data(), data.size());
}

void TcpConnection::write(std::span<const signed char> data)
{
    write(data.data(), data.size());
}

void TcpConnection::write(std::span<const std::byte> data)
{
    write(data.data(), data.size());
}

void TcpConnection::write(std::string_view string)
{
    write(string.data(), string.size());
}

std::string TcpConnection::peer() const
{
    auto storage = sockaddr_storage{};
    auto storageLen = socklen_t{sizeof(storage)};
    errnoCheck(::getpeername(
        _fd, reinterpret_cast<sockaddr*>(&storage), &storageLen));
    auto stream = std::ostringstream{};
    stream << storage;
    return stream.str();
}

} // namespace hehe
