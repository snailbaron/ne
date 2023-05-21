#include <hehe/io.hpp>

#include <hehe/error.hpp>

#include <arpa/inet.h>

#include <array>
#include <string>

namespace hehe {

std::ostream& operator<<(std::ostream& output, const sockaddr_in& addr)
{
    auto buffer = std::array<char, INET_ADDRSTRLEN>{};
    if (!inet_ntop(AF_INET, &addr.sin_addr, buffer.data(), buffer.size())) {
        throwErrno();
    }
    return output << buffer.data() << ":" << ntohs(addr.sin_port);
}

std::ostream& operator<<(std::ostream& output, const sockaddr_in6& addr)
{
    auto buffer = std::array<char, INET6_ADDRSTRLEN>{};
    if (!inet_ntop(AF_INET6, &addr.sin6_addr, buffer.data(), buffer.size())) {
        throwErrno();
    }
    return output << "[" << buffer.data() << "]:" << ntohs(addr.sin6_port);
}

std::ostream& operator<<(std::ostream& output, const sockaddr& addr)
{
    if (addr.sa_family == AF_INET) {
        return output << reinterpret_cast<const sockaddr_in&>(addr);
    }

    if (addr.sa_family == AF_INET6) {
        return output << reinterpret_cast<const sockaddr_in6&>(addr);
    }

    throw Error{"unknown sa_family inside sockaddr: " +
        std::to_string(addr.sa_family)};
}

std::ostream& operator<<(std::ostream& output, const sockaddr_storage& addr)
{
    if (addr.ss_family == AF_INET) {
        return output << reinterpret_cast<const sockaddr_in&>(addr);
    }

    if (addr.ss_family == AF_INET6) {
        return output << reinterpret_cast<const sockaddr_in6&>(addr);
    }

    throw Error{"unknown ss_family inside sockaddr_storage: " +
        std::to_string(addr.ss_family)};
}

} // namespace hehe
