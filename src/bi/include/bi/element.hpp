#pragma once

#include <concepts>
#include <cstddef>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace bi {

struct Data;

enum class Type : size_t {
    Empty,
    Number,
    String,
    List,
    Dictionary,
};

std::string name(Type type);

class Element {
public:
    Element() = default;
    Element(const Element& other);
    Element& operator=(const Element& other);

    explicit Element(long long number);
    explicit Element(std::string string);

    Type type() const;

    long long number() const;
    const std::string& string() const;
    const std::vector<Element>& list() const;
    const std::map<std::string, Element> dictionary() const;

    void append(const Element& element);
    void append(Element&& element);

    const Element& operator[](size_t index) const;
    Element& operator[](size_t index);
    const Element& operator[](const std::string& key) const;
    Element& operator[](const std::string& key);

    static const Element empty;

private:
    std::unique_ptr<Data> _data = std::make_unique<Data>();
};

struct Data {
    using VariantType = std::variant<
        std::monostate,
        long long,
        std::string,
        std::vector<Element>,
        std::map<std::string, Element>>;

    Type type() const;

    template <Type type>
    void create()
    {
        if (this->type() == Type::Empty) {
            constexpr auto index = static_cast<size_t>(type);
            variant.emplace<std::variant_alternative_t<index, VariantType>>();
        }
    }

    template <Type type>
    const auto& as() const
    {
        return std::get<static_cast<size_t>(type)>(variant);
    }

    template <Type type>
    auto& as()
    {
        return std::get<static_cast<size_t>(type)>(variant);
    }

    VariantType variant;
};

std::ostream& operator<<(std::ostream& output, const Element& element);

} // namespace bi
