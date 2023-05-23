#include <ne/metainfo.hpp>

#include <ne/bencoding.hpp>
#include <ne/error.hpp>

#include <algorithm>
#include <cstddef>
#include <span>

#include <iostream>

namespace ne {

std::ostream& operator<<(std::ostream& output, Size size)
{
    output.setf(std::ios::fixed, std::ios::floatfield);
    output.precision(2);

    output << size._sizeInBytes;

    if (size._sizeInBytes >= 1 << 30) {
        return output << " (" <<
            static_cast<double>(size._sizeInBytes) / (1 << 30) << "G)";
    }
    if (size._sizeInBytes >= 1 << 20) {
        return output << " (" <<
            static_cast<double>(size._sizeInBytes) / (1 << 20) << "M)";
    }
    if (size._sizeInBytes >= 1 << 10) {
        return output << " (" <<
            static_cast<double>(size._sizeInBytes) / (1 << 10) << "K)";
    }
    return output << size._sizeInBytes;
}

SHA1::SHA1(std::span<const std::byte, 20> data)
{
    std::copy(data.begin(), data.end(), _bytes.begin());
}

std::ostream& operator<<(std::ostream& output, const SHA1& sha1)
{
    output << std::hex;
    output.fill('0');
    for (std::byte b : sha1._bytes) {
        output << std::setw(2) << static_cast<int>(b);
    }
    return output << std::dec;
}

std::ostream& operator<<(std::ostream& output, const Metainfo& metainfo)
{
    output <<
        "announce: " << metainfo.announce << "\n" <<
        "info:\n" <<
        "  name: " << metainfo.info.name << "\n" <<
        "  pieceLength: " << metainfo.info.pieceLength << "\n" <<
        "  pieces:\n";
    for (const auto& sha1 : metainfo.info.pieces) {
        output << "    " << sha1 << "\n";
    }

    if (metainfo.info.files.empty()) {
        output << "  length: " << metainfo.info.length << "\n";
    } else {
        output << "  files:\n";
        for (const auto& file : metainfo.info.files) {
            output << "    length: " << file.length << "; path: " <<
                file.path << "\n";
        }
    }

    output << "extra:\n";
    for (const auto& [key, value] : metainfo.extra) {
        output << "    \"" << key << "\": ";
        prettyPrint(output, value, 4);
        output << "\n";
    }

    return output;
}

Metainfo decodeMetainfo(std::istream& input)
{
    const auto be = parseElement(input);

    auto metainfo = Metainfo{};

    metainfo.announce = be.at("announce").string();
    metainfo.info.name = be.at("info").at("name").string();
    metainfo.info.pieceLength = Size(be.at("info").at("piece length").number());

    auto pieces = be.at("info").at("pieces").bytes();
    if (pieces.size() % 20 != 0) {
        throw Error{"metainfo \"pieces\" length must be a multiple of 20"};
    }
    for (size_t i = 0; i < pieces.size(); i += 20) {
        auto span = std::span<const std::byte, 20>{pieces.data() + i, 20};
        metainfo.info.pieces.emplace_back(span);
    }

    if (be.at("info").contains("length")) {
        metainfo.info.length = Size(be.at("info").at("length").number());
    } else {
        for (auto file : be.at("info").at("files").list()) {
            auto filePath = std::filesystem::path{};
            for (const auto& pathComponent : file.at("path").list()) {
                filePath /= pathComponent.string();
            }

            metainfo.info.files.push_back({
                .length = Size(file.at("length").number()),
                .path = filePath,
            });
        }
    }

    for (const auto& [key, value] : be.dictionary()) {
        if (key != "announce" && key != "info") {
            metainfo.extra[key] = value;
        }
    }

    return metainfo;
}

} // namespace ne
