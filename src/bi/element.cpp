#include <bi/element.hpp>

#include <array>
#include <utility>
#include <iomanip>

namespace bi {

namespace {

void prettyPrint(
    std::ostream& output, const Element& x, size_t startOffset, size_t offset)
{
    switch (x.type()) {
        case Type::Empty:
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
                if (key == "pieces") {
                    output << "<pieces>";
                } else {
                    prettyPrint(output, value, 0, offset + 4);
                }
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

Element::Element(const Element& other)
{
    *_data = *other._data;
}

Element& Element::operator=(const Element& other)
{
    *_data = *other._data;
    return *this;
}

Element::Element(long long number)
{
    _data->variant.emplace<long long>(number);
}

Element::Element(std::string string)
{
    _data->variant.emplace<std::string>(std::move(string));
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

const std::vector<Element>& Element::list() const
{
    return _data->as<Type::List>();
}

const std::map<std::string, Element> Element::dictionary() const
{
    return _data->as<Type::Dictionary>();
}

void Element::append(const Element& element)
{
    _data->create<Type::List>();
    _data->as<Type::List>().push_back(element);
}

void Element::append(Element&& element)
{
    _data->create<Type::List>();
    _data->as<Type::List>().push_back(std::move(element));
}

const Element& Element::operator[](size_t index) const
{
    return _data->as<Type::List>().at(index);
}

Element& Element::operator[](size_t index)
{
    return _data->as<Type::List>().at(index);
}

const Element& Element::operator[](const std::string& key) const
{
    return _data->as<Type::Dictionary>().at(key);
}

Element& Element::operator[](const std::string& key)
{
    _data->create<Type::Dictionary>();
    return _data->as<Type::Dictionary>()[key];
}

size_t Element::size() const
{
    if (_data->type() == Type::List) {
        return _data->as<Type::List>().size();
    }
    return _data->as<Type::Dictionary>().size();
}

const Element Element::empty {};

Type Data::type() const
{
    return Type{variant.index()};
}

std::ostream& operator<<(std::ostream& output, const Element& element)
{
    prettyPrint(output, element, 0, 0);
    return output;
}

} // namespace bi
