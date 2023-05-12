#pragma once

#include <bi/element.hpp>

#include <string>

namespace bi {

struct Metainfo {
    struct Info {
        std::string name;
        size_t pieceLength = 0;
        std::vector<std::array<std::byte, 20>> pieces;
    };

    std::string announce;

    std::map<std::string, bi::Element> extra;
};

} // namespace
