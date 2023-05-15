#pragma once

#include <cerrno>
#include <concepts>
#include <cstring>
#include <exception>
#include <source_location>
#include <sstream>
#include <string>

namespace hehe {

class Error : public std::exception {
public:
    Error(
        std::string_view message,
        std::source_location sourceLocation = std::source_location::current())
    {
        auto stream = std::ostringstream{};
        stream <<
            sourceLocation.file_name() << ":" << sourceLocation.line() << ":" <<
            sourceLocation.column() << " (" << sourceLocation.function_name() <<
            "): " << message;
        _message = stream.str();
    }

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

private:
    std::string _message;
};

template <class U, class V = int>
inline auto errnoCheck(
    U returnValue,
    V returnValueIndicatingError = -1,
    std::source_location sourceLocation = std::source_location::current())
{
    if (returnValue == returnValueIndicatingError) {
        auto errorCode = errno;
        auto stream = std::ostringstream{};
        stream << errorCode << ": " << std::strerror(errorCode);
        throw Error{stream.str(), sourceLocation};
    }
    return returnValue;
}

} // namespace hehe
