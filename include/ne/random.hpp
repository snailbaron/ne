#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>

namespace ne {

class Random {
public:
    Random();

    uint32_t uint32();

    template <size_t N> std::array<std::byte, N> bytes()
    {
        auto data = std::array<std::byte, N>{};
        for (size_t i = 0; i < N; i += 4) {
            uint32_t r = this->uint32();
            data.at(i) = static_cast<std::byte>((r >> 24) & 0xFF);
            if (i + 1 < N) {
                data.at(i + 1) = static_cast<std::byte>((r >> 16) & 0xFF);
            }
            if (i + 2 < N) {
                data.at(i + 2) = static_cast<std::byte>((r >> 8) & 0xFF);
            }
            if (i + 3 < N) {
                data.at(i + 3) = static_cast<std::byte>(r & 0xFF);
            }
        }
        return data;
    }

private:
    std::mt19937 _engine;
};

} // namespace ne
