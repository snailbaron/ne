#include <ne.hpp>

#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <vector>

using namespace ne;

using namespace std::chrono_literals;

void chat(TcpStream& stream)
{
    std::cout << "task started" << std::endl;
    static const auto strings = std::vector<std::string>{
        "one",
        "two",
        "three",
    };
    size_t index = 0;
    for (;;) {
        const auto& s = strings.at(index);
        std::cout << "writing \"" << s << "\"" << std::endl;
        stream << s << std::endl;
        index = (index + 1) % strings.size();
        std::this_thread::sleep_for(1s);
    }
}

int main(int argc, char* argv[]) try
{
    if (argc > 2) {
        std::cerr << "usage: chatter-tcp-server [PORT]\n";
        return EXIT_FAILURE;
    }

    uint16_t port = 0;
    if (argc >= 2) {
        port = std::stol(argv[1]);
    }

    int pipes[2];
    errnoCheck(pipe(pipes));

    auto listen = TcpListen{port};
    std::cout << "listening on " << listen.name() << std::endl;

    auto futures = std::vector<std::future<void>>{};
    for (;;) {
        std::cout << "listening" << std::endl;
        auto stream = listen.accept();
        std::cout << "incoming connection: " << stream.peer() << std::endl;

        futures.push_back(std::async(
            std::launch::async,
            [stream = std::move(stream)]() mutable { chat(stream); }));
    }
} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
