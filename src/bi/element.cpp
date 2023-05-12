#include <bi/element.hpp>

#include <array>
#include <utility>
#include <iomanip>

namespace bi {

namespace {

void prettyPrint(std::ostream& output, const Element& x, size_t offset)
{
    switch (x.type()) {
        case Type::Empty:
            output << "<>";
            break;
        case Type::String:
            output << "\"" << x.string() << "\"";
            break;
        case Type::Number:
            output << x.number();
            break;
        case Type::List:
        {
            output << "[";
            const auto& list = x.list();
            if (auto it = list.begin(); it != list.begin()) {
                prettyPrint(output, *it++, offset);
                for (; it != list.end(); ++it) {
                    output << ", ";
                    prettyPrint(output, *it, offset);
                }
            }
            output << "]";
            break;
        }
        case Type::Dictionary:
        {
            output << "{\n";
            offset += 4;

            size_t maxKeyLength = 0;
            for (const auto& [key, value] : x.dictionary()) {
                maxKeyLength = std::max(maxKeyLength, key.length());
            }

            output << std::left;
            for (const auto& [key, value] : x.dictionary()) {
                output << std::string(offset, ' ') <<
                    std::setw(maxKeyLength + 2) << "\"" + key + "\"" << " : ";
                if (key == "pieces") {
                    output << "<pieces>";
                } else {
                    prettyPrint(output, value, offset);
                }
                output << "\n";
            }
            offset -= 4;
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

const Element Element::empty {};

Type Data::type() const
{
    return Type{variant.index()};
}

std::ostream& operator<<(std::ostream& output, const Element& element)
{
    prettyPrint(output, element, 0);
    return output;
}

} // namespace bi
