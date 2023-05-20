#include <hehe.hpp>

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

    auto stream = hehe::TcpStream{address, port};
    stream.exceptions(std::ios::badbit | std::ios::failbit);

    for (std::string line; std::getline(std::cin, line); ) {
        stream << line << std::endl;
    }
}
