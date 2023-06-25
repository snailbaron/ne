#include <ne/bencoding.hpp>

#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace ne {

namespace {

std::string parseString(std::istream& input);
long long parseRawNumber(std::istream& input);
ne::Element parseInteger(std::istream& input);
ne::Element parseList(std::istream& input);

constexpr bool isDigit(int c)
{
    return
        c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
        c == '5' || c == '6' || c == '7' || c == '8' || c == '9';
}

void expect(std::istream& input, std::string_view expectedString)
{
    for (char c : expectedString) {
        if (input.get() != c) {
            throw std::runtime_error{"unexpected symbol"};
        }
    }
}

long long parseRawNumber(std::istream& input)
{
    auto s = std::string{};
    while (isDigit(input.peek())) {
        s += (char)input.get();
    }

    if (s.length() > 1 && s.front() == '0') {
        throw std::runtime_error{"leading zero in integer: " + s};
    }

    return std::stoll(s);
}

std::string parseString(std::istream& input)
{
    std::streamsize stringLength = parseRawNumber(input);
    expect(input, ":");
    std::string string(stringLength, ' ');
    input.read(string.data(), stringLength);
    return string;
}

ne::Element parseInteger(std::istream& input)
{
    expect(input, "i");

    bool sign = true;
    if (input.peek() == '-') {
        sign = false;
        input.get();
    }

    auto base = parseRawNumber(input);
    if (!sign && base == 0) {
        throw std::runtime_error{"-0 is not allowed"};
    }

    expect(input, "e");
    long long result = base * sign;

    return ne::Element{result};
}

ne::Element parseList(std::istream& input)
{
    ne::Element list;

    expect(input, "l");
    while (input.peek() != 'e') {
        list.append(bdecode(input));
    }
    expect(input, "e");

    return list;
}

ne::Element parseDictionary(std::istream& input)
{
    ne::Element dictionary;

    expect(input, "d");
    auto previousKey = std::optional<std::string>{};
    while (input.peek() != 'e') {
        auto key = parseString(input);
        if (previousKey && previousKey >= key) {
            throw std::runtime_error{
                "dictionary keys must be lexicographically sorted, but [" +
                    *previousKey + "] >= [" + key + "]"
            };
        }
        auto value = bdecode(input);
        dictionary[key] = value;
    }
    expect(input, "e");

    return dictionary;
}

std::string encodeString(const std::string& string)
{
    return std::to_string(string.length()) + ":" + string;
}

} // namespace

ne::Element bdecode(std::istream& input)
{
    int c = input.peek();
    if (isDigit(c)) {
        return ne::Element{parseString(input)};
    } else if (c == 'i') {
        return parseInteger(input);
    } else if (c == 'l') {
        return parseList(input);
    } else if (c == 'd') {
        return parseDictionary(input);
    } else if (c == EOF) {
        return ne::Element{};
    } else {
        throw std::runtime_error{"expected start of new element"};
    }
}

std::string bencode(const ne::Element& element)
{
    switch (element.type()) {
        case Type::Null: return "";
        case Type::Number:
            return "i" + std::to_string(element.number()) + "e";
        case Type::String:
            return encodeString(element.string());
        case Type::List:
        {
            auto output = std::ostringstream{};
            output << "l";
            for (const auto& x : element.list()) {
                output << bencode(x);
            }
            output << "e";
            return output.str();
        }
        case Type::Dictionary:
        {
            auto output = std::ostringstream{};
            output << "d";
            for (const auto& [k, v] : element.dictionary()) {
                output << encodeString(k) << bencode(v);
            }
            output << "e";
            return output.str();
        }
    }

    throw std::runtime_error{"unknown element type: " +
        std::to_string(static_cast<size_t>(element.type()))};
}

} // namespace ne
