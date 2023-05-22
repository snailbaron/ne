#include <ne/bencoding.hpp>

#include <optional>
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
    expect(input, "e");
    long long result = base * sign;

    return ne::Element{result};
}

ne::Element parseList(std::istream& input)
{
    ne::Element list;

    expect(input, "l");
    while (input.peek() != 'e') {
        list.append(parseElement(input));
    }
    expect(input, "e");

    return list;
}

ne::Element parseDictionary(std::istream& input)
{
    ne::Element dictionary;

    expect(input, "d");
    while (input.peek() != 'e') {
        auto key = parseString(input);
        auto value = parseElement(input);
        dictionary[key] = value;
    }
    expect(input, "e");

    return dictionary;
}

} // namespace

ne::Element parseElement(std::istream& input)
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
        return ne::Element::empty;
    } else {
        throw std::runtime_error{"expected start of new element"};
    }
}

} // namespace ne