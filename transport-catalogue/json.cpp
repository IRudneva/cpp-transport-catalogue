#include "json.h"
using namespace std;
#include <cstring>
namespace json {
    namespace {
        static std::unordered_map<char, char> esc_symbols = {
        {'\\', '\\'},
        {'"', '"'},
        {'n', '\n'},
        {'r', '\r'},
        {'/', '/'},
        {'b', '\b'},
        {'f', '\f'},
        {'t', '\t'}
        };

        Node LoadNode(istream& input);

        Node LoadArray(istream& input)
        {
            if (input.peek() == std::char_traits<char>::eof())     // std::char_traits<char>::eof() == -1
            {
                throw ParsingError("LoadArray::unexpected end of file"s);
            }
            else if (input.peek() == ',')
            {
                throw ParsingError("LoadArray::wrong format"s);
            }
            Array result;
            for (char c; input >> c && c != ']';)
            {
                if (c != ',')
                {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            return Node(result);
        }

        Node LoadBoolNull(istream& input)
        {
            char str_buf[4];
            input.get(str_buf, 4);
            if (input.gcount() == 3)
            {
                if (0 == std::memcmp(str_buf, "rue", 3))
                {
                    return Node{ true };
                }
                else if (0 == std::memcmp(str_buf, "ull", 3))
                {
                    return Node{ nullptr };
                }
                else if ((0 == std::memcmp(str_buf, "als", 3)) && (input.get() == 'e'))
                {
                    return Node{ false };
                }
                else
                {
                    throw ParsingError("LoadBoolNull::invalid data: "s + std::string(str_buf));
                }
            }
            else
            {
                throw ParsingError("LoadBoolNull::unexpected end of file on function entry. Data: "s + std::string(str_buf));
            }
        }

        Node LoadNumber(istream& input)
        {
            std::string parsed_num;
            auto read_char = [&parsed_num, &input]
            {
                parsed_num += static_cast<char>(input.get());
                if (!input)
                {
                    throw ParsingError("LoadNumber::read_char - Failed to read number from stream"s);
                }
            };

            auto read_digits = [&input, read_char]
            {
                if (!std::isdigit(input.peek()))
                {
                    throw ParsingError("LoadNumber::read_digits - A digit is expected"s);
                }
                while (std::isdigit(input.peek()))
                {
                    read_char();
                }
            };

            if (input.peek() == '-')
            {
                read_char();
            }
            if (input.peek() == '0')
            {
                read_char();
            }
            else
            {
                read_digits();
            }

            bool is_int = true;
            if (input.peek() == '.')
            {
                read_char();
                read_digits();
                is_int = false;
            }
            if (int ch = input.peek(); ch == 'e' || ch == 'E')
            {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-')
                {
                    read_char();
                }
                read_digits();
                is_int = false;
            }
            try
            {
                if (is_int)
                {
                    try
                    {
                        int val = std::stoi(parsed_num);
                        return Node{ val };
                    }
                    catch (...)
                    {
                    }
                }
                double d_val = std::stod(parsed_num);
                return Node{ d_val };
            }
            catch (...)
            {
                throw ParsingError("LoadNumber::Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadString(istream& input)
        {
            if (input.peek() == std::char_traits<char>::eof())     // std::char_traits<char>::eof() == -1
            {
                throw ParsingError("LoadString::unexpected end of file"s);
            }
            std::string line;
            char c;
            while (input.get(c))
            {
                if (c == '\\')
                {
                    if (input.peek() != std::char_traits<char>::eof())
                    {
                        input.get(c);
                        if (esc_symbols.find(c) != esc_symbols.end())
                        {
                            c = esc_symbols[c];
                        }
                        else
                        {
                            throw ParsingError("LoadString::wrong control character: \\"s + c);
                        }
                    }
                }
                else if (c == '"')
                {
                    return Node(line);
                }
                line += c;
            }
            throw ParsingError("LoadString::closing double quote is missing"s);
        }

        Node LoadDict(istream& input)
        {
            if (input.peek() == std::char_traits<char>::eof())
            {
                throw ParsingError("LoadDict::unexpected end of file"s);
            }
            else if (input.peek() == ',')
            {
                throw ParsingError("LoadDict::wrong format"s);
            }
            Dict result;
            for (char c; input >> c && c != '}';)
            {
                if (c == ',')
                {
                    input >> c;
                }
                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            return Node(result);
        }

        Node LoadNode(istream& input)
        {
            char c;
            if (input >> c)
            {
                if (c == '[')
                {
                    return LoadArray(input);
                }
                else if (c == '{')
                {
                    return LoadDict(input);
                }
                else if (c == '"')
                {
                    return LoadString(input);
                }
                else if ((c == 't') || (c == 'f') || (c == 'n'))
                {
                    return LoadBoolNull(input);
                }
                else
                {
                    input.putback(c);
                    return LoadNumber(input);
                }
            }
            else
            {
                throw ParsingError("LoadNode - unexpected end of file"s);
            }
        }
    }  // namespace

    Node::Node() :
        json_node_(nullptr) {
    }

    bool Node::IsNull() const
    {
        return std::holds_alternative<std::nullptr_t>(json_node_);
    }

    bool Node::IsInt() const
    {
        return std::holds_alternative<int>(json_node_);
    }

    bool Node::IsDouble() const
    {
        return std::holds_alternative<double>(json_node_)
            || std::holds_alternative<int>(json_node_);
    }

    bool Node::IsPureDouble() const
    {
        return std::holds_alternative<double>(json_node_)
            && !std::holds_alternative<int>(json_node_);
    }

    bool Node::IsString() const
    {
        return std::holds_alternative<std::string>(json_node_);
    }

    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(json_node_);
    }

    bool Node::IsArray() const
    {
        return std::holds_alternative<Array>(json_node_);
    }

    bool Node::IsMap() const
    {
        return std::holds_alternative<Dict>(json_node_);
    }

    bool Node::operator==(const Node& rhs) const
    {
        return (json_node_ == rhs.json_node_);
    }

    bool Node::operator!=(const Node& rhs) const
    {
        return !(*this == rhs);
    }

    Document::Document(Node root)
        : root_(move(root)){}

    const Node& Document::GetRoot() const
    {
        return root_;
    }

    bool Document::operator==(const Document& rhs) const
    {
        return (this->GetRoot().GetValue() == rhs.GetRoot().GetValue());
    }

    bool Document::operator!=(const Document& rhs) const
    {
        return !(*this == rhs);
    }

    const Array& Node::AsArray() const
    {
        if (IsArray())
        {
            return std::get<Array>(json_node_);
        }
        else
        {
            throw std::logic_error("Node::AsArray cant return non-array");
        }
    }

    const Dict& Node::AsMap() const
    {
        if (IsMap())
        {
            return std::get<Dict>(json_node_);
        }
        else
        {
            throw std::logic_error("Node::AsMap cant return non-map");
        }
    }

    bool Node::AsBool() const
    {
        if (IsBool())
        {
            return std::get<bool>(json_node_);
        }
        else
        {
            throw std::logic_error("Node::AsBool cant return non-bool");
        }
    }

    int Node::AsInt() const
    {
        if (IsInt())
        {
            return std::get<int>(json_node_);
        }
        else
        {
            throw std::logic_error("Node::AsInt cant return non-int");
        }
    }

    double Node::AsDouble() const
    {
        if (IsInt())
        {
            return std::get<int>(json_node_) * 1.0L;
        }
        else if (IsDouble())
        {
            return std::get<double>(json_node_);
        }
        else
        {
            throw std::logic_error("Node::AsDouble cant return non-double");
        }
    }

    const std::string& Node::AsString() const
    {
        if (IsString())
        {
            return std::get<std::string>(json_node_);
        }
        else
        {
            throw std::logic_error("Node::AsString cant return non-string");
        }
    }

    const std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>& Node::GetValue() const
    {
        return json_node_;
    }

    Document Load(istream& input)
    {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output)
    {
        std::visit(OstreamSolutionPrinter{ output }, doc.GetRoot().GetValue());
    }

    void OstreamSolutionPrinter::operator()(std::nullptr_t) const
    {
        using namespace std::literals;
        out << "null"sv;
    }

    void OstreamSolutionPrinter::operator()(Array array) const
    {
        using namespace std::literals;
        out << "["sv << std::endl;
        bool print_comma = false;
        for (const auto& elem : array)
        {
            if (print_comma)
            {
                out << ", "sv << std::endl;
            }
            std::visit(OstreamSolutionPrinter{ out }, elem.GetValue());
            print_comma = true;
        }
        out << std::endl << "]"sv;
    }

    void OstreamSolutionPrinter::operator()(Dict map) const
    {
        using namespace std::literals;
        out << "{"sv << std::endl;
        bool print_comma = false;
        for (const auto& elem : map)
        {
            if (print_comma)
            {
                out << ", "sv << std::endl;
            }
            out << "\""sv << elem.first << "\": "sv;
            std::visit(OstreamSolutionPrinter{ out }, elem.second.GetValue());
            print_comma = true;
        }
        out << std::endl << "}"sv;
    }

    void OstreamSolutionPrinter::operator()(bool value) const
    {
        using namespace std::literals;
        if (value)
        {
            out << "true"sv;
        }
        else
        {
            out << "false"sv;
        }
    }

    void OstreamSolutionPrinter::operator()(int num) const
    {
        out << num;
    }

    void OstreamSolutionPrinter::operator()(double num) const
    {
        out << num;
    }

    void OstreamSolutionPrinter::operator()(std::string line) const
    {
        using namespace std::literals;
        std::istringstream strm(line);
        out << "\""sv;
        char ch;
        while (strm.get(ch))
        {
            if (esc_symbols_save.find(ch) != esc_symbols_save.end())
            {
                out << esc_symbols_save[ch];
            }
            else
            {
                out << ch;
            }
        }
        out << "\""sv;
    }
}  // namespace json