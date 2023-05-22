#include <ne.hpp>

#include <iostream>

int main()
{
    auto request = ne::http::Request{
        .method = ne::http::Method::GET,
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

    auto response = ne::http::request(request);
    std::cout << "response:\n" << response << "\n";
}
