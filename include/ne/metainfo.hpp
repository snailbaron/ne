#pragma once

#include <ne/element.hpp>

#include <string>

namespace ne {

struct Metainfo {
    struct Info {
        std::string name;
        size_t pieceLength = 0;
        std::vector<std::array<std::byte, 20>> pieces;
    };

    std::string announce;

    std::map<std::string, ne::Element> extra;
};

} // namespace ne
