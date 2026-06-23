#include <functional>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>
#include <vector>

namespace PolyglotAPI
{
    class Node
    {
      public:
        std::variant<std::nullptr_t, bool,int, double, float, std::string, std::map<std::string, Node>, std::vector<Node>, std::function<Node(const Node&)>> value;

        Node();
        Node(bool b);
        Node(double d);
        Node(float d);
        Node(int d);
        Node(const std::string& s);
        Node(const char* s);
        Node(std::function<Node(const Node&)> cb);
        Node(const std::map<std::string, Node>& obj);
        Node(const std::vector<Node>& arr);
        Node(const nlohmann::json& j);
        virtual ~Node();

        operator bool() const;
        operator double() const;
        operator float() const;
        operator int() const;
        operator std::string() const;
        operator nlohmann::json() const;

        Node           operator()(const Node& arg);
        Node&          operator[](const std::string& key);
        nlohmann::json toJson() const;
    };
}