#include <hehe.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstddef>
#include <exception>
#include <iostream>

using namespace hehe;

int main() try
{
    int socketFd = errnoCheck(socket(AF_INET, SOCK_STREAM, 0));

    {
        auto socketAddress = sockaddr_in{
            .sin_family = AF_INET,
            .sin_port = htons(0),
            .sin_addr = {.s_addr = INADDR_ANY},
        };

        errnoCheck(bind(
            socketFd,
            reinterpret_cast<const sockaddr*>(&socketAddress),
            sizeof(socketAddress)));
    }

    errnoCheck(listen(socketFd, 1000));

    {
        auto resultAddress = sockaddr_in{};
        auto resultAddressLength = socklen_t{sizeof(resultAddress)};
        errnoCheck(getsockname(
            socketFd,
            reinterpret_cast<sockaddr*>(&resultAddress),
            &resultAddressLength));

        auto addressText = std::array<char, INET_ADDRSTRLEN>{};
        errnoCheck(
            inet_ntop(
                AF_INET,
                &resultAddress.sin_addr,
                addressText.data(),
                addressText.size()),
            nullptr);
        std::cout << "listening on " << addressText.data() << ":" <<
            ntohs(resultAddress.sin_port) << std::endl;
    }

    auto acceptedSocket = accept(socketFd, nullptr, nullptr);
    std::cout << "incoming connection" << std::endl;

    auto buffer = std::array<char, 1000>{};
    while (auto result = read(acceptedSocket, buffer.data(), buffer.size())) {
        std::cout << result << " bytes: ";
        std::cout.write(buffer.data(), result);
        std::cout.flush();
    }
} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
