#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include <ostream>

namespace hehe {

std::ostream& operator<<(std::ostream& output, const sockaddr_in& addr);
std::ostream& operator<<(std::ostream& output, const sockaddr_in6& addr);
std::ostream& operator<<(std::ostream& output, const sockaddr& addr);
std::ostream& operator<<(std::ostream& output, const sockaddr_storage& addr);

} // namespace hehe
