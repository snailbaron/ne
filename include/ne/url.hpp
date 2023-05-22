#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

namespace ne {

class URLEncode {
public:
    explicit URLEncode(std::string_view string);
    friend std::ostream& operator<<(
        std::ostream& output, const URLEncode& urlEncode);

private:
    std::string_view _string;
};

std::string urlEncode(std::string_view string);

struct URL {
    std::string absolute() const;

    std::string scheme;
    std::string domain;
    std::optional<uint16_t> port;
    std::string path;
    std::map<std::string, std::string> parameters;
    std::optional<std::string> anchor;
};

std::ostream& operator<<(std::ostream& output, const URL& url);

} // namespace ne
