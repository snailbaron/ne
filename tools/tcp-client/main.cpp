#include <ne.hpp>

#include <poll.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "usage: tcp-client ADDRESS PORT\n";
        return EXIT_FAILURE;
    }

    const char* address = argv[1];
    uint16_t port = std::stoi(argv[2]);

    auto connection = ne::TcpConnection{address, port};

    auto fds = std::array{
        pollfd{.fd = STDIN_FILENO, .events = POLLIN, .revents = 0},
        pollfd{.fd = connection.fd(), .events = POLLIN, .revents = 0},
    };

    auto buffer = std::array<char, 2 << 20>{};
    for (;;) {
        ne::errnoCheck(poll(fds.data(), fds.size(), -1));
        if (fds.at(0).revents & POLLIN) {
            auto bytesRead = ne::errnoCheck(read(
                STDIN_FILENO, buffer.data(), buffer.size()));
            connection.write(buffer.data(), bytesRead);
        }
        if (fds.at(1).revents & POLLHUP) {
            break;
        }
        if (fds.at(1).revents & POLLIN) {
            auto bytesRead = connection.read(buffer);
            std::cout << std::string_view{buffer.data(), bytesRead};
            std::cout.flush();
        }
    }
}
