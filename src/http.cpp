#include <ne/http.hpp>

#include <ne/error.hpp>
#include <ne/tcp_stream.hpp>

#include <iostream>

namespace ne::http {

std::ostream& operator<<(std::ostream& output, Method httpMethod)
{
    switch (httpMethod) {
        case Method::GET: return output << "GET";
        case Method::HEAD: return output << "HEAD";
        case Method::POST: return output << "POST";
        case Method::PUT: return output << "PUT";
        case Method::DELETE: return output << "DELETE";
        case Method::CONNECT: return output << "CONNECT";
        case Method::OPTIONS: return output << "OPTIONS";
        case Method::TRACE: return output << "TRACE";
        case Method::PATCH: return output << "PATCH";
    }

    throw Error{"unknown Method value: " +
        std::to_string(static_cast<int>(httpMethod))};
}

std::ostream& operator<<(std::ostream& output, const Request& request)
{
    output << request.method << " " << request.url.absolute() << " " << request.version <<
        "\n";
    for (const auto& [key, value] : request.headers) {
        output << key << ": " << value << "\n";
    }
    return output;
}

std::ostream& operator<<(std::ostream& output, const Response& response)
{
    output << response.version << " " << response.statusCode << " " <<
        response.statusText << "\n";
    for (const auto& [key, value] : response.headers) {
        output << key << ": " << value << "\n";
    }
    return output;
}

Response request(const Request& request)
{
    auto stream = TcpStream{request.url.domain, request.url.port.value_or(80)};
    stream.exceptions(std::ios::failbit | std::ios::badbit);

    std::cout << "abs url: " << request.url.absolute() << std::endl;
    stream << request.method << " " << request.url.absolute() << " " << request.version <<
        "\r\n";
    for (const auto& [key, value] : request.headers) {
        stream << key << ": " << value << "\r\n";
    }
    stream << "\r\n";
    stream.flush();

    auto response = Response{};
    {
        stream >> response.version >> response.statusCode >> std::ws;
        std::getline(stream, response.statusText, '\r');
        if (stream.get() != '\n') {
            throw Error{"expected \\n after \\r"};
        }

        auto line = std::string{};
        std::getline(stream, line, '\r');
        if (stream.get() != '\n') {
            throw Error{"expected \\n after \\r"};
        }

        while (!line.empty()) {
            auto view = std::string_view{line};
            auto sep = view.find(':');
            auto key = view.substr(0, sep);
            auto value = view.substr(sep + 2);
            response.headers.emplace(key, value);
            std::getline(stream, line, '\r');
            if (stream.get() != '\n') {
                throw Error{"expected \\n after \\r"};
            }
        }
    }

    return response;
}

} // namespace ne::http
