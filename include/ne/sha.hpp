#pragma once

#include <array>
#include <ostream>
#include <span>

namespace ne {

class SHA1 {
public:
    explicit SHA1(std::span<const std::byte, 20> data);

    friend std::ostream& operator<<(std::ostream& output, const SHA1& sha1);

private:
    std::array<std::byte, 20> _bytes {};
};

SHA1 sha1(std::istream& stream);

} // namespace ne
