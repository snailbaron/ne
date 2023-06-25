#pragma once

#include <array>
#include <ostream>
#include <span>
#include <string_view>

namespace ne {

class SHA1 {
public:
    SHA1() = default;
    explicit SHA1(std::span<const std::byte, 20> data);

    std::string string() const;
    friend std::ostream& operator<<(std::ostream& output, const SHA1& sha1);

private:
    std::array<std::byte, 20> _bytes {};
};

SHA1 sha1(std::istream& stream);
SHA1 sha1(std::string_view string);

} // namespace ne
