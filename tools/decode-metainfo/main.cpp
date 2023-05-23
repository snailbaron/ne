#include <ne.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr <<
            "usage:\n" <<
            "  decode-metainfo [FILE]\n" <<
            "  decode-metainfo -\n";
        return EXIT_FAILURE;
    }

    auto path = std::string{argv[1]};

    auto metainfo = ne::Metainfo{};
    if (path == "-") {
        metainfo = ne::decodeMetainfo(std::cin);
    } else {
        auto input = std::ifstream{path, std::ios::binary};
        input.exceptions(std::ios::badbit | std::ios::failbit);
        metainfo = ne::decodeMetainfo(input);
    }

    std::cout << metainfo << "\n";
}
