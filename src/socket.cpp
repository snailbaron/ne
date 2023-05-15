#include <hehe/socket.hpp>

#include <hehe/error.hpp>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>

namespace hehe {

IPV4 IPV4::any = IPV4(INADDR_ANY);

IPV4::IPV4(uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
    uint32_t address = 0;
    auto p = reinterpret_cast<std::byte*>(&address);
    p[0] = static_cast<std::byte>(x1);
    p[1] = static_cast<std::byte>(x2);
    p[2] = static_cast<std::byte>(x3);
    p[3] = static_cast<std::byte>(x4);
    _addr = htonl(address);
}

IPV4::IPV4(const std::string& address)
{
    auto addr = in_addr{};
    auto result = errnoCheck(inet_pton(AF_INET, address.c_str(), &addr));
    if (result != 1) {
        throw Error{"failed to convert string to IPV4 address: " + address};
    }
    _addr = addr.s_addr;
}

IPV4::IPV4(in_addr_t addr)
    : _addr(addr)
{ }

std::ostream& operator<<(std::ostream& output, const IPV4& ipv4)
{
    uint32_t addr = ntohl(ipv4._addr);
    std::byte* p = reinterpret_cast<std::byte*>(&addr);
    return output <<
        static_cast<int>(p[0]) << "." <<
        static_cast<int>(p[1]) << "." <<
        static_cast<int>(p[2]) << "." <<
        static_cast<int>(p[3]);
}

IPV4Port::IPV4Port(const IPV4& address, uint16_t port)
    : _address(address)
    , _port(port)
{ }

std::ostream& operator<<(std::ostream& output, const IPV4Port& ipv4port)
{
    return output << ipv4port._address << ":" << ipv4port._port;
}

TcpSocket::TcpSocket()
    : _fd(errnoCheck(socket(AF_INET, SOCK_STREAM, 0)))
{ }

TcpSocket::TcpSocket(int fd)
    : _fd(fd)
{ }

void TcpSocket::bind(const IPV4& address, uint16_t port)
{
    auto socketAddress = sockaddr_in{
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = address._addr},
    };
    errnoCheck(::bind(
        _fd,
        reinterpret_cast<const sockaddr*>(&socketAddress),
        sizeof(socketAddress)));
}

void TcpSocket::listen(int queueSize)
{
    errnoCheck(::listen(_fd, queueSize));
}

IPV4Port TcpSocket::name() const
{
    auto address = sockaddr_in{};
    auto size = socklen_t{sizeof(address)};
    errnoCheck(getsockname(_fd, reinterpret_cast<sockaddr*>(&address), &size));

    return {ntohl(address.sin_addr.s_addr), ntohs(address.sin_port)};
}

TcpSocket TcpSocket::accept()
{
    return TcpSocket{errnoCheck(::accept(_fd, nullptr, nullptr))};
}

void TcpSocket::connect(const IPV4& address, uint16_t port)
{
    auto socketAddress = sockaddr_in{
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = address._addr},
    };
    errnoCheck(::connect(
        _fd,
        reinterpret_cast<const sockaddr*>(&socketAddress),
        sizeof(socketAddress)));
}

std::vector<std::byte> TcpSocket::read()
{
    auto result = std::vector<std::byte>{};
    auto buffer = std::array<std::byte, 1000>{};
    while (auto readCode =
            errnoCheck(::read(_fd, buffer.data(), buffer.size()))) {
        std::copy_n(buffer.begin(), readCode, std::back_inserter(result));
    }
    return result;
}

} // namespace hehe
