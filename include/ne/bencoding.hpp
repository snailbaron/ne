#pragma once

#include <ne/element.hpp>

#include <istream>

namespace ne {

ne::Element bdecode(std::istream& input);
std::string bencode(const ne::Element& element);

} // namespace ne
