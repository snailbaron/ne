#include <ne/random.hpp>

#include <array>
#include <ctime>

namespace ne {

Random::Random()
{
    auto rd = std::random_device{};
    auto seed = std::array<uint32_t, 624>{};
    for (size_t i = 0; i < 624; i++) {
        uint32_t fromTime =
            (time(nullptr) & 0xFF) | ((time(nullptr) & 0xFF) << 8);
        uint32_t fromRd = rd();
        seed.at(i) = fromTime ^ fromRd;
    }
    auto seq = std::seed_seq{seed.begin(), seed.end()};
    _engine = std::mt19937{seq};
}

uint32_t Random::uint32()
{
    return _engine();
}

} // namespace ne
