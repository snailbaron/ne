#include <ne/sha.hpp>

#include <bit>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include <iostream>

namespace ne {

namespace {

constexpr uint32_t byteswap(uint32_t number)
{
    return
        ((number & 0xFF000000) >> 24) +
        ((number & 0x00FF0000) >> 8) +
        ((number & 0x0000FF00) << 8) +
        ((number & 0x000000FF) << 24);
}

class ShaByteStream {
public:
    ShaByteStream(std::istream& source)
        : _source(source)
    { }

    uint8_t getByte()
    {
        char c = 0;
        if (_source.get(c)) {
            _bytesRead++;
            std::cerr << "byte from input: [" << c << "] (0x" <<
                std::hex << (int)c << std::dec << ")\n";
            return c;
        }

        if (!_borderByteReturned) {
            _borderByteReturned = true;

            _zeroBytesToReturn = 64 + 64 - 8 - ((_bytesRead + 1) % 64);
            if (_zeroBytesToReturn >= 64) {
                _zeroBytesToReturn -= 64;
            }
            std::cerr << "zero bytes to return: " << _zeroBytesToReturn << "\n";

            std::cerr << "border byte 0x80\n";

            return 0x80;
        }

        if (_zeroBytesToReturn > 0) {
            _zeroBytesToReturn--;
            std::cerr << "zero byte\n";
            return 0;
        }

        if (_sizeBytesToReturn > 0) {
            _sizeBytesToReturn--;
            uint8_t result = ((_bytesRead * 8) >> (_sizeBytesToReturn * 8)) & 0xFF;
            std::cerr << "size byte: " << std::hex << "0x" << (int)result <<
                std::dec << " (" << (int)result << ")\n";
            std::cerr << "more size bytes: " << _sizeBytesToReturn << "\n";
            return result;
        }

        throw std::runtime_error{"ShaByteStream: no more bytes"};
    }

    uint32_t getUint32()
    {
        return
            (getByte() << 24) +
            (getByte() << 16) +
            (getByte() << 8) +
            getByte();
    }

    bool hasMoreBytes() const
    {
        return _sizeBytesToReturn > 0;
    }

private:
    std::istream& _source;
    size_t _bytesRead = 0;
    bool _borderByteReturned = false;
    size_t _zeroBytesToReturn = 0;
    size_t _sizeBytesToReturn = 8;
};

} // namespace

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

SHA1 sha1(std::istream& stream)
{
    static_assert(CHAR_BIT == 8);

    auto bytes = ShaByteStream{stream};

    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    while (bytes.hasMoreBytes()) {
        std::array<uint32_t, 80> w {};
        std::cerr << "getting 16 32-bit values\n";
        for (size_t i = 0; i < 16; i++) {
            w.at(i) = bytes.getUint32();
            std::cerr << "32-bit value " << i << " is 0x" << std::hex <<
                w.at(i) << std::dec << " (" << w.at(i) << ")\n";
        }

        for (size_t i = 16; i < 80; i++) {
            w[i] = std::rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        uint32_t f = 0;
        uint32_t k = 0;

        for (size_t i = 0; i < 80; i++) {
            if (i < 20) {
                f = (b & c) | (~b & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = std::rotl(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = std::rotl(b, 30);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    auto hh = std::array<std::byte, 20>{
        static_cast<std::byte>((h0 >> 24) & 0xFF),
        static_cast<std::byte>((h0 >> 16) & 0xFF),
        static_cast<std::byte>((h0 >> 8) & 0xFF),
        static_cast<std::byte>((h0) & 0xFF),
        static_cast<std::byte>((h1 >> 24) & 0xFF),
        static_cast<std::byte>((h1 >> 16) & 0xFF),
        static_cast<std::byte>((h1 >> 8) & 0xFF),
        static_cast<std::byte>((h1) & 0xFF),
        static_cast<std::byte>((h2 >> 24) & 0xFF),
        static_cast<std::byte>((h2 >> 16) & 0xFF),
        static_cast<std::byte>((h2 >> 8) & 0xFF),
        static_cast<std::byte>((h2) & 0xFF),
        static_cast<std::byte>((h3 >> 24) & 0xFF),
        static_cast<std::byte>((h3 >> 16) & 0xFF),
        static_cast<std::byte>((h3 >> 8) & 0xFF),
        static_cast<std::byte>((h3) & 0xFF),
        static_cast<std::byte>((h4 >> 24) & 0xFF),
        static_cast<std::byte>((h4 >> 16) & 0xFF),
        static_cast<std::byte>((h4 >> 8) & 0xFF),
        static_cast<std::byte>((h4) & 0xFF),
    };

    return SHA1{hh};
}

} // namespace ne
