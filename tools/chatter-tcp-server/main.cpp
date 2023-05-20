#include <hehe.hpp>

#include <chrono>
#include <future>
#include <iostream>
#include <vector>

using namespace hehe;

using namespace std::chrono_literals;

int main() try
{
    auto listen = TcpListen{};
    std::cout << "listening on " << listen.name() << std::endl;

    auto futures = std::vector<std::future<void>>{};
    for (;;) {
        auto stream = listen.accept();
        std::cout << "incoming connection" << std::endl;

        futures.push_back(std::async(
            std::launch::async,
            [&stream] {
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
                    stream << s;
                    index = (index + 1) % strings.size();
                    std::this_thread::sleep_for(1s);
                }
            }));
    }

} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
}
