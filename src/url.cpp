#include <ne/url.hpp>

#include <cctype>
#include <sstream>

namespace ne {

namespace {

bool isUnreserved(char c)
{
    return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~';
}

void printUrl(std::ostream& output, const URL& url, bool absoluteForm)
{
    if (absoluteForm) {
        output << url.scheme << "://" << url.domain;
        if (url.port) {
            output << ":" << url.port.value();
        }
    }

    output << url.path;
    if (auto kv = url.parameters.begin(); kv != url.parameters.end()) {
        output << "?" << URLEncode{kv->first} << "=" << URLEncode{kv->second};
        ++kv;
        for (; kv != url.parameters.end(); ++kv) {
            output << "&" << URLEncode{kv->first} << "=" <<
                URLEncode{kv->second};
        }
    }
    if (url.anchor) {
        output << "#" << url.anchor.value();
    }
}

} // namespace

template <class DataView>
URLEncode<DataView>::URLEncode(DataView dataView)
    : _dataView(dataView)
{ }

template <class DataView>
std::ostream& operator<<(
    std::ostream& output, const URLEncode<DataView>& urlEncode)
{
    output << std::hex;
    for (auto x : urlEncode._dataView) {
        auto c = static_cast<char>(x);
        if (isUnreserved(c)) {
            output << c;
        } else {
            output << "%" << static_cast<int>(c);
        }
    }
    return output << std::dec;
}

template class URLEncode<std::string_view>;
template class URLEncode<std::span<const std::byte>>;

std::string urlencode(std::string_view string)
{
    auto stream = std::ostringstream{};
    stream << URLEncode{string};
    return stream.str();
}

std::string urlencode(std::span<const std::byte> bytes)
{
    auto stream = std::ostringstream{};
    stream << URLEncode{bytes};
    return stream.str();
}

std::string URL::absolute() const
{
    auto stream = std::ostringstream{};
    printUrl(stream, *this, false);
    return stream.str();
}

std::ostream& operator<<(std::ostream& output, const URL& url)
{
    printUrl(output, url, true);
    return output;
}

} // namespace ne
