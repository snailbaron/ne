#include <hehe/tcp_streambuf.hpp>

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
        _connection.write(std::span{pbase(), pptr()});
        setp(_putArea.data(), _putArea.data() + _putArea.size());
    }

    return 0;
}

int TcpStreambuf::underflow()
{
    if (gptr() >= egptr()) {
        setg(eback(), eback(), egptr());
        if (!_connection.read({eback(), egptr()})) {
            std::cout << "TcpStreambuf::underflow: read returned false" << std::endl;
            return traits_type::eof();
        }
    }

    return *gptr();
}

int TcpStreambuf::overflow(int ch)
{
    std::cout << "TcpStreambuf::overflow" << std::endl;

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
    std::cout << "TcpStreambuf::pbackfail" << std::endl;
    throw Error{"TcpStreambuf::pbackfail called"};
}

// Unused virtual functions

void TcpStreambuf::imbue(const std::locale& loc)
{
    std::cout << "TcpStreambuf::imbue" << std::endl;
    std::streambuf::imbue(loc);
}

std::streambuf* TcpStreambuf::setbuf(char_type* s, std::streamsize n)
{
    std::cout << "TcpStreambuf::setbuf" << std::endl;
    return std::streambuf::setbuf(s, n);
}

std::streampos TcpStreambuf::seekoff(
    off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
    std::cout << "TcpStreambuf::seekoff" << std::endl;
    return std::streambuf::seekoff(off, dir, which);
}

std::streampos TcpStreambuf::seekpos(std::streampos pos, std::ios_base::openmode which)
{
    std::cout << "TcpStreambuf::seekpos" << std::endl;
    return std::streambuf::seekpos(pos, which);
}

std::streamsize TcpStreambuf::showmanyc()
{
    std::cout << "TcpStreambuf::showmanyc" << std::endl;
    return std::streambuf::showmanyc();
}

int TcpStreambuf::uflow()
{
    std::cout << "TcpStreambuf::uflow" << std::endl;
    return std::streambuf::uflow();
}

std::streamsize TcpStreambuf::xsgetn(char_type* s, std::streamsize count)
{
    std::cout << "TcpStreambuf::xsgetn" << std::endl;
    return std::streambuf::xsgetn(s, count);
}

std::streamsize TcpStreambuf::xsputn(const char_type* s, std::streamsize count)
{
    std::cout << "TcpStreambuf::xsputn" << std::endl;
    return std::streambuf::xsputn(s, count);
}

} // namespace hehe
