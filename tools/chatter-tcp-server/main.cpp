#include <hehe.hpp>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cstddef>
#include <exception>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace hehe;

using namespace std::chrono_literals;

int main() try
{
    auto tcpSocket = TcpSocket{};
    tcpSocket.bind(IPV4::any, 0);
    tcpSocket.listen(1000);
    std::cout << "listening on " << tcpSocket.name() << std::endl;

    auto futures = std::vector<std::future<void>>{};
    for (;;) {
        auto acceptedSocket = tcpSocket.accept();
        std::cout << "incoming connection" << std::endl;

        futures.push_back(std::async(
            std::launch::async,
            [&acceptedSocket] {
                static const auto strings = std::vector<std::string>{
                    "one",
                    "two",
                    "three",
                };
                size_t index = 0;
                for (;;) {
                    const auto& s = strings.at(index);
                    std::cout << "writing \"" << s << "\"" << std::endl;
                    acceptedSocket.write(s);
                    index = (index + 1) % strings.size();
                    std::this_thread::sleep_for(1s);
                }
            }));
    }

} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
