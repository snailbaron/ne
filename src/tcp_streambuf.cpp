#include <hehe/tcp_streambuf.hpp>

#include <hehe/error.hpp>

#include <iostream>
#include <utility>

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
    // "Syncing" input seems to be impossible for a socket, so do nothing in the
    // get area.

    if (pbase() < pptr()) {
        _connection.write(std::span{pbase(), pptr()});
        setp(_putArea.data(), _putArea.data() + _putArea.size());
    }

    return 0;
}

int TcpStreambuf::underflow()
{
    if (gptr() >= egptr()) {
        setg(eback(), eback(), egptr());
        if (_connection.read({eback(), egptr()}) == 0) {
            setg(eback(), egptr(), egptr());
            return traits_type::eof();
        }
    }

    return traits_type::to_int_type(*gptr());
}

int TcpStreambuf::overflow(int ch)
{
    if (pptr() > pbase()) {
        _connection.write(std::span{pbase(), pptr()});
        setp(pbase(), epptr());
    }

    if (!traits_type::eq_int_type(ch, traits_type::eof())) {
        *pptr() = static_cast<char>(ch);
    }

    return 0;
}

int TcpStreambuf::pbackfail(int)
{
    throw Error{"TcpStreambuf::pbackfail called"};
}

} // namespace hehe
