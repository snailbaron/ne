#include <ne.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: touch-tracker TORRENT_FILE\n";
        return EXIT_FAILURE;
    }

    auto torrentFilePath = fs::path{argv[1]};

    auto metainfoFile = std::ifstream{torrentFilePath};
    auto metainfo = ne::decodeMetainfo(metainfoFile);
    std::cout << metainfo << "\n";

    size_t totalSize = metainfo.info.length;
    if (totalSize == 0) {
        for (const auto& file : metainfo.info.files) {
            totalSize += file.length;
        }
    }

    auto response = ne::http::request({
        .method = ne::http::Method::GET,
        .url = {
            .scheme = "http",
            .domain = metainfo.announce,
        },
        .headers = {
            {"info_hash", ne::urlencode(metainfo.infoHash.string())},
            {"peer_id", ne::urlencode(ne::Random{}.bytes<20>())},
            {"uploaded", "0"},
            {"downloaded", "0"},
            {"left", std::to_string(totalSize)},
        },
    });

    std::cout << response << "\n";
}
