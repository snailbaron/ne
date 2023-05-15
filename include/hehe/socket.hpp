#pragma once

#include <netinet/in.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

namespace hehe {

class IPV4 {
public:
    IPV4(uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
    IPV4(const std::string& address);

    static IPV4 any;

    friend std::ostream& operator<<(std::ostream& output, const IPV4& ipv4);

private:
    IPV4(in_addr_t addr);

    in_addr_t _addr {};

    friend class TcpSocket;
};

class IPV4Port {
public:
    IPV4Port(const IPV4& address, uint16_t port);

    friend std::ostream& operator<<(
        std::ostream& output, const IPV4Port& ipv4port);

private:
    IPV4 _address;
    uint16_t _port = 0;
};

class TcpSocket {
public:
    TcpSocket();

    void bind(const IPV4& address, uint16_t port);
    void listen(int queueSize);
    IPV4Port name() const;
    [[nodiscard]] TcpSocket accept();
    void connect(const IPV4& address, uint16_t port);

    std::vector<std::byte> read();

    template <class T>
    void write(const T& data)
    {
        size_t written = 0;
        size_t left = data.size();
        for (;;) {
            auto bytesWritten =
                errnoCheck(::write(_fd, data.data() + written, left));
            if (bytesWritten == left) {
                break;
            }

            written += bytesWritten;
            left -= bytesWritten;
        }
    }

private:
    explicit TcpSocket(int fd);

    int _fd = 0;
};

} // namespace hehe
