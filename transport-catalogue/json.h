#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <variant>
#include <unordered_map>

namespace json {
    class Node;
    // —охраните объ€влени€ Dict и Array без изменени€
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    // Ёта ошибка должна выбрасыватьс€ при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };
    class Node {
    public:
        /* –еализуйте Node, использу€ std::variant */
        Node();
        template <typename T>
        Node(T value) :
            json_node_(std::move(value)) {
        }

        bool IsInt() const;

        bool IsDouble() const;

        bool IsPureDouble() const;

        bool IsBool() const;

        bool IsString() const;

        bool IsNull() const;

        bool IsArray() const;

        bool IsMap() const;

        const Array& AsArray() const;

        const Dict& AsMap() const;

        const std::string& AsString() const;

        bool AsBool() const;

        int AsInt() const;

        double AsDouble() const;

        const std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>& GetValue() const;

        bool operator==(const Node& rhs) const;

        bool operator!=(const Node& rhs) const;

    private:
        std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> json_node_;
    };

    class Document {
    public:

        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator ==(const Document& rhs) const;

        bool operator !=(const Document& rhs) const;

    private:
        Node root_;
    };

    static std::unordered_map<char, std::string> esc_symbols_save = {
    {'\\', std::string("\\\\")},
    {'"', std::string("\\\"")},
    {'\n', std::string("\\n")},
    {'\r',std::string("\\r")},
    {'\b',std::string("\\b")},
    {'\f',std::string("\\f")}
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

    struct OstreamSolutionPrinter
    {
        std::ostream& out;
        void operator()(std::nullptr_t) const;
        void operator()(Array array) const;
        void operator()(Dict map) const;
        void operator()(bool value) const;
        void operator()(int num) const;
        void operator()(double num) const;
        void operator()(std::string line) const;
    };
}