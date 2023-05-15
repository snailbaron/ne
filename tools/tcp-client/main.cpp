#include <hehe.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "usage: tcp-client ADDRESS PORT\n";
        return EXIT_FAILURE;
    }

    auto address = std::string{argv[1]};
    int port = atoi(argv[2]);

    auto tcpSocket = hehe::TcpSocket{};
    tcpSocket.connect(address, port);

    for (std::string line; std::getline(std::cin, line); ) {
        line += "\n";
        tcpSocket.write(line);
    }
}
