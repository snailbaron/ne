#pragma once

#include <ne/element.hpp>

#include <array>
#include <cstddef>
#include <filesystem>
#include <istream>
#include <ostream>
#include <span>
#include <string>
#include <vector>

namespace ne {

class Size {
public:
    Size() = default;
    explicit Size(size_t sizeInBytes) : _sizeInBytes(sizeInBytes) {}
    operator size_t() const { return _sizeInBytes; }

    friend std::ostream& operator<<(std::ostream& output, Size size);

private:
    size_t _sizeInBytes = 0;
};

class SHA1 {
public:
    SHA1(std::span<const std::byte, 20> data);

    friend std::ostream& operator<<(std::ostream& output, const SHA1& sha1);

private:
    std::array<std::byte, 20> _bytes {};
};

// Content of a metainfo file, as described here:
// http://bittorrent.org/beps/bep_0003.html
struct Metainfo {
    struct Info {
        struct File {
            Size length;
            std::filesystem::path path;
        };

        std::string name;
        Size pieceLength;
        std::vector<SHA1> pieces;

        Size length; // only in single-file metainfo
        std::vector<File> files; // only in multi-file metainfo
    };

    std::string announce;
    Info info;

    std::map<std::string, ne::Element> extra;
};

std::ostream& operator<<(std::ostream& output, const Metainfo& metainfo);
Metainfo decodeMetainfo(std::istream& input);

} // namespace ne
