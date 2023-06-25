#include <ne/element.hpp>

#include <array>
#include <iomanip>
#include <sstream>
#include <utility>

namespace ne {

namespace {

void prettyPrint(
    std::ostream& output, const Element& x, size_t startOffset, size_t offset)
{
    switch (x.type()) {
        case Type::Null:
            output << std::string(startOffset, ' ') << "<>";
            break;
        case Type::String:
            output << std::string(startOffset, ' ') << "\"" << x.string() << "\"";
            break;
        case Type::Number:
            output << std::string(startOffset, ' ') << x.number();
            break;
        case Type::List:
        {
            output << std::string(startOffset, ' ') << "[\n";
            for (const auto& listElement : x.list()) {
                prettyPrint(output, listElement, offset + 4, offset + 4);
                output << "\n";
            }
            output << std::string(offset, ' ') << "]";
            break;
        }
        case Type::Dictionary:
        {
            output << std::string(startOffset, ' ') << "{\n";

            for (const auto& [key, value] : x.dictionary()) {
                output << std::string(offset + 4, ' ') <<
                    "\"" << key << "\"" << " : ";
                prettyPrint(output, value, 0, offset + 4);
                output << "\n";
            }
            output << std::string(offset, ' ') << "}";
            break;
        }
    }
}

} // namespace

std::string name(Type type)
{
    static constexpr auto names = std::array<std::string, 5>{
        "Empty",
        "Number",
        "String",
        "List",
        "Dictionary",
    };
    return names.at(static_cast<size_t>(type));
}

Element::Element(long long number)
    : _data(std::make_unique<Data>(Data{.variant = number}))
{ }

Element::Element(std::string string)
    : _data(std::make_unique<Data>(Data{.variant = std::move(string)}))
{ }

Element::Element(const Element& other)
    : _data(std::make_unique<Data>(*other._data))
{ }

Element& Element::operator=(const Element& other)
{
    if (this != &other) {
        *_data = *other._data;
    }
    return *this;
}

Type Element::type() const
{
    return _data->type();
}

long long Element::number() const
{
    return _data->as<Type::Number>();
}

const std::string& Element::string() const
{
    return _data->as<Type::String>();
}

std::span<const std::byte> Element::bytes() const
{
    const auto& string = this->string();
    return {reinterpret_cast<const std::byte*>(string.data()), string.length()};
}

const std::vector<Element>& Element::list() const
{
    return _data->as<Type::List>();
}

const std::map<std::string, Element>& Element::dictionary() const
{
    return _data->as<Type::Dictionary>();
}

void Element::append(const Element& element)
{
    _data->create<Type::List>().push_back(element);
}

void Element::append(Element&& element)
{
    _data->create<Type::List>().push_back(std::move(element));
}

Element& Element::operator[](size_t index)
{
    return _data->as<Type::List>().at(index);
}

const Element& Element::operator[](size_t index) const
{
    return _data->as<Type::List>().at(index);
}

Element& Element::operator[](const std::string& key)
{
    return _data->create<Type::Dictionary>()[key];
}

Element& Element::operator[](std::string&& key)
{
    return _data->create<Type::Dictionary>()[std::move(key)];
}

Element& Element::at(const std::string& key)
{
    return _data->as<Type::Dictionary>().at(key);
}

const Element& Element::at(const std::string& key) const
{
    return _data->as<Type::Dictionary>().at(key);
}

bool Element::contains(const std::string& key) const
{
    return _data->as<Type::Dictionary>().contains(key);
}

size_t Element::size() const
{
    if (_data->type() == Type::List) {
        return _data->as<Type::List>().size();
    }
    return _data->as<Type::Dictionary>().size();
}

bool Element::empty() const
{
    return this->size() == 0;
}

constexpr Type Data::type() const
{
    return Type{variant.index()};
}

std::ostream& operator<<(std::ostream& output, const Element& element)
{
    prettyPrint(output, element, 0, 0);
    return output;
}

std::string toString(const Element& element)
{
    auto stream = std::ostringstream{};
    stream << element;
    return stream.str();
}

void prettyPrint(std::ostream& output, const Element& x, size_t offset)
{
    prettyPrint(output, x, 0, offset);
}

} // namespace ne
