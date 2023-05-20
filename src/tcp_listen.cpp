#include <hehe/tcp_listen.hpp>

#include <hehe/error.hpp>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>

namespace hehe {

TcpListen::TcpListen()
{
    addrinfo* info = nullptr;
    {
        const auto desiredInfo = addrinfo{
            .ai_flags = AI_PASSIVE,
            .ai_family = AF_UNSPEC,
            .ai_socktype = SOCK_STREAM,
        };
        if (auto code = getaddrinfo(
                nullptr,
                "0",
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

        if (bind(_fd, info->ai_addr, info->ai_addrlen) == -1) {
            auto errorCode = errno;
            errors << std::strerror(errorCode) << "\n";
            continue;
        }

        if (listen(_fd, 20) == -1) {
            auto errorCode = errno;
            errors << std::strerror(errorCode) << "\n";
            continue;
        }

        break;
    }

    if (info == nullptr) {
        throw Error{"could not connect:\n" + errors.str()};
    }

    freeaddrinfo(info);
}

TcpListen::~TcpListen()
{
    close(_fd);
}

std::string TcpListen::name() const
{
    auto addrStorage = sockaddr_storage{};
    auto addrLen = socklen_t{sizeof(addrStorage)};
    errnoCheck(getsockname(
        _fd, reinterpret_cast<sockaddr*>(&addrStorage), &addrLen));

    constexpr auto maxBufferSize = std::max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN);
    auto buffer = std::array<char, maxBufferSize>{};

    auto stream = std::ostringstream{};
    if (addrStorage.ss_family == AF_INET) {
        const auto& addr = reinterpret_cast<const sockaddr_in&>(addrStorage);
        if (!inet_ntop(AF_INET, &addr.sin_addr, buffer.data(), buffer.size())) {
            throwErrno();
        }
        stream << buffer.data() << ":" << ntohs(addr.sin_port);
    } else if (addrStorage.ss_family == AF_INET6) {
        const auto& addr = reinterpret_cast<const sockaddr_in6&>(addrStorage);
        if (!inet_ntop(AF_INET6, &addr.sin6_addr, buffer.data(), buffer.size())) {
            throwErrno();
        }
        stream << buffer.data() << ":" << ntohs(addr.sin6_port);
    } else {
        throw Error{"TcpListen::name: unknown protocol family: " +
            std::to_string(addrStorage.ss_family)};
    }

    return stream.str();
}

TcpStream TcpListen::accept()
{
    auto stream = TcpStream{errnoCheck(::accept(_fd, nullptr, nullptr))};
    stream.exceptions(std::ios::badbit | std::ios::failbit);
    stream.rdbuf();
    return stream;
}

} // namespace hehe
