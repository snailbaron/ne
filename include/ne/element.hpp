#pragma once

#include <concepts>
#include <cstddef>
#include <map>
#include <memory>
#include <ostream>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace ne {

struct Data;

enum class Type : size_t {
    Null,
    Number,
    String,
    List,
    Dictionary,
};

std::string name(Type type);

class Element {
public:
    Element() = default;
    explicit Element(long long number);
    explicit Element(std::string string);

    Element(const Element& other);
    Element& operator=(const Element& other);

    Element(Element&& other) = default;
    Element& operator=(Element&& other) = default;

    ~Element() = default;

    Type type() const;

    long long number() const;
    const std::string& string() const;
    std::span<const std::byte> bytes() const;
    const std::vector<Element>& list() const;
    const std::map<std::string, Element>& dictionary() const;

    void append(const Element& element);
    void append(Element&& element);

    Element& operator[](size_t index);
    const Element& operator[](size_t index) const;

    Element& operator[](const std::string& key);
    Element& operator[](std::string&& key);
    Element& at(const std::string& key);
    const Element& at(const std::string& key) const;

    bool contains(const std::string& key) const;

    size_t size() const;
    bool empty() const;

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

    constexpr Type type() const;

    template <Type type>
    constexpr auto& create()
    {
        if (this->type() == Type::Null) {
            constexpr auto index = static_cast<size_t>(type);
            variant.emplace<std::variant_alternative_t<index, VariantType>>();
        }
        return this->as<type>();
    }

    template <Type type>
    constexpr auto& as()
    {
        return std::get<static_cast<size_t>(type)>(variant);
    }

    template <Type type>
    constexpr const auto& as() const
    {
        return std::get<static_cast<size_t>(type)>(variant);
    }

    VariantType variant;
};

std::ostream& operator<<(std::ostream& output, const Element& element);
std::string toString(const Element& element);

void prettyPrint(std::ostream& output, const Element& x, size_t offset);

} // namespace ne
