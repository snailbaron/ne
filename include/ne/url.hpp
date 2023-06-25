#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>

namespace ne {

template <class DataView>
class URLEncode {
public:
    explicit URLEncode(DataView dataView);

    template <class V>
    friend std::ostream& operator<<(
        std::ostream& output, const URLEncode<V>& urlEncode);

private:
    DataView _dataView;
};

extern template class URLEncode<std::string_view>;
extern template class URLEncode<std::span<const std::byte>>;

std::string urlencode(std::string_view string);
std::string urlencode(std::span<const std::byte> bytes);

struct URL {
    std::string absolute() const;

    std::string scheme;
    std::string domain;
    std::optional<uint16_t> port = std::nullopt;
    std::string path = "/";
    std::map<std::string, std::string> parameters = {};
    std::optional<std::string> anchor = std::nullopt;
};

std::ostream& operator<<(std::ostream& output, const URL& url);

} // namespace ne
