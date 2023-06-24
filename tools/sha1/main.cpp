#include <ne.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: sha1 FILE\n";
        return EXIT_FAILURE;
    }

    auto stream = std::ifstream{argv[1], std::ios::binary};
    auto sha = ne::sha1(stream);
    std::cout << sha << "\n";
}
