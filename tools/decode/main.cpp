#include <ne.hpp>

#include <array>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

int main(int argc, char* argv[]) try
{
    if (argc > 2) {
        std::cerr << "usage: " << argv[0] << " [INPUT_FILE]\n";
        return EXIT_FAILURE;
    }

    auto document = ne::Element{};
    if (argc == 2) {
        auto input = std::ifstream{argv[1]};
        input.exceptions(std::ios::badbit | std::ios::failbit);
        document = ne::parseElement(input);
    } else {
        document = ne::parseElement(std::cin);
    }

    std::cout << document << "\n";
} catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
}
