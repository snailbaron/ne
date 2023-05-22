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

URLEncode::URLEncode(std::string_view string)
    : _string(string)
{ }

std::ostream& operator<<(std::ostream& output, const URLEncode& urlEncode)
{
    output << std::hex;
    for (char c : urlEncode._string) {
        if (isUnreserved(c)) {
            output << c;
        } else {
            output << "%" << static_cast<int>(c);
        }
    }
    return output << std::dec;
}

std::string urlEncode(std::string_view string)
{
    auto stream = std::ostringstream{};
    stream << URLEncode{string};
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
