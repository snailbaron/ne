#pragma once

#include <ne/url.hpp>

#include <cstdint>
#include <map>
#include <ostream>
#include <string>
#include <string_view>

namespace ne::http {

enum class Method {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
};

std::ostream& operator<<(std::ostream& output, Method httpMethod);

struct Request {
    Method method;
    std::string version = "HTTP/1.1";
    URL url;
    std::map<std::string, std::string> headers;
};

std::ostream& operator<<(std::ostream& output, const Request& request);

struct Response {
    std::string version;
    int statusCode = 0;
    std::string statusText;
    std::map<std::string, std::string> headers;
};

std::ostream& operator<<(std::ostream& output, const Response& response);

Response request(const Request& request);

} // namespace ne::http
