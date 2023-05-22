#include <hehe.hpp>

#include <iostream>

int main()
{
    auto request = hehe::http::Request{
        .method = hehe::http::Method::GET,
        .url {
            .scheme = "http",
            .domain = "google.com",
            .path = "/",
        },
        .headers {
            {"Accept", "*/*"},
        },
    };
    std::cout << "request:\n" << request;

    auto response = hehe::http::request(request);
    std::cout << "response:\n" << response << "\n";
}
