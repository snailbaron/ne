#include <ne/tcp_stream.hpp>

#include <utility>

namespace ne {

TcpStream::TcpStream(std::string_view address, uint16_t port)
    : _streambuf{TcpConnection{address, port}}
{
    rdbuf(&_streambuf);
}

TcpStream::TcpStream(int fd)
    : _streambuf{TcpConnection{fd}}
{
    rdbuf(&_streambuf);
}

TcpStream::TcpStream(TcpStream&& other) noexcept
    : _streambuf(std::move(other._streambuf))
{
    rdbuf(&_streambuf);
}

TcpStream& TcpStream::operator=(TcpStream&& other) noexcept
{
    _streambuf = std::move(other._streambuf);
    rdbuf(&_streambuf);
    return *this;
}

} // namespace ne
