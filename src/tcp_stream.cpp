#include <hehe/tcp_stream.hpp>

#include <hehe/error.hpp>

#include <utility>

#include <iostream>

namespace hehe {

TcpStreambuf::TcpStreambuf(TcpConnection&& connection, size_t bufferSize)
    : _connection{std::move(connection)}
    , _getArea(bufferSize)
    , _putArea(bufferSize)
{
    setg(_getArea.data(), _getArea.data(), _getArea.data() + _getArea.size());
    setp(_putArea.data(), _putArea.data() + _putArea.size());
}

int TcpStreambuf::sync()
{
    std::cout << "TcpStreambuf::sync" << std::endl;

    // "Syncing" input seems to be impossible for a socket, so do nothing in the
    // get area.

    if (pbase() < pptr()) {
        _connection.write({pbase(), pptr()});
        setp(_putArea.data(), _putArea.data() + _putArea.size());
    }

    return 0;
}

int TcpStreambuf::underflow()
{
    std::cout << "TcpStreambuf::underflow" << std::endl;

    if (gptr() >= egptr()) {
        setg(eback(), eback(), egptr());
        if (!_connection.read({eback(), egptr()})) {
            return traits_type::eof();
        }
    }

    return *gptr();
}

int TcpStreambuf::overflow(int ch)
{
    std::cout << "TcpStreambuf::overflow" << std::endl;

    if (pptr() > pbase()) {
        _connection.write({pbase(), pptr()});
        setp(pbase(), epptr());
    }

    if (!traits_type::eq_int_type(ch, traits_type::eof())) {
        *pptr() = static_cast<char>(ch);
    }

    return 0;
}

int TcpStreambuf::pbackfail(int)
{
    std::cout << "TcpStreambuf::pbackfail" << std::endl;
    throw Error{"TcpStreambuf::pbackfail called"};
}

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

} // namespace hehe
