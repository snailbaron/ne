#include <hehe.hpp>

#include <array>
#include <cstddef>
#include <exception>
#include <iostream>
#include <string>

using namespace hehe;

int main() try
{
    auto listen = TcpListen{};
    std::cout << "listening on " << listen.name() << std::endl;

    auto connection = listen.accept();
    std::cout << "incoming connection" << std::endl;

    for (std::string line; std::getline(connection, line); ) {
        std::cout << "[" << line.length() << " bytes] " << line << std::endl;
    }
} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
