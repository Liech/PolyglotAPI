#include "Node.h"

namespace PolyglotAPI
{
    Node::Node()
      : value(nullptr)
    {
    }
    Node::~Node() {}

    Node::Node(bool b)
      : value(b)
    {
    }
    Node::Node(double d)
      : value(d)
    {
    }
    Node::Node(const std::string& s)
      : value(s)
    {
    }
    Node::Node(const char* s)
      : value(std::string(s))
    {
    }
    Node::Node(std::function<Node(const Node&)> cb)
      : value(cb)
    {
    }
    Node::Node(const std::map<std::string, Node>& obj)
      : value(obj)
    {
    }
    Node::Node(const std::vector<Node>& arr)
      : value(arr)
    {
    }

    Node::Node(const nlohmann::json& j)
    {
        if (j.is_boolean())
            value = j.get<bool>();
        else if (j.is_number())
            value = j.get<double>();
        else if (j.is_string())
            value = j.get<std::string>();
        else if (j.is_object())
        {
            std::map<std::string, Node> m;
            for (auto it = j.begin(); it != j.end(); ++it)
                m[it.key()] = Node(it.value());
            value = m;
        }
        else if (j.is_array())
        {
            std::vector<Node> m;
            for (auto it = j.begin(); it != j.end(); ++it)
                m.push_back(it.value());
            value = m;
        }
    }

    Node Node::operator()(const Node& arg)
    {
        if (auto cb = std::get_if<std::function<Node(const Node&)>>(&value))
        {
            return (*cb)(arg);
        }
        throw std::runtime_error("Node is not a function");
    }

    Node& Node::operator[](const std::string& key)
    {
        if (std::holds_alternative<std::nullptr_t>(value))
        {
            value = std::map<std::string, Node>{};
        }
        if (auto map_ptr = std::get_if<std::map<std::string, Node>>(&value))
        {
            return (*map_ptr)[key];
        }
        throw std::runtime_error("Node is not object/map");
    }

    nlohmann::json Node::toJson() const
    {
        return std::visit(
          [](auto&& arg) -> nlohmann::json
          {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, std::nullptr_t>)
                  return nullptr;
              else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, double> || std::is_same_v<T, std::string>)
                  return arg;
              else if constexpr (std::is_same_v<T, std::map<std::string, Node>>)
              {
                  nlohmann::json j;
                  for (auto const& [k, v] : arg)
                      j[k] = v.toJson();
                  return j;
              }
              else if constexpr (std::is_same_v<T, std::vector<Node>>)
              {
                  nlohmann::json j = nlohmann::json::array();
                  for (auto const& v : arg)
                      j.push_back(v.toJson());
                  return j;
              }
              return nullptr;
          },
          value);
    }

    Node::operator bool() const
    {
        if (auto val = std::get_if<bool>(&value))
            return *val;
        throw std::runtime_error("Invalid Type");
    }

    Node::operator double() const
    {
        if (auto val = std::get_if<double>(&value))
            return *val;
        throw std::runtime_error("Invalid Type");
    }

    Node::operator std::string() const
    {
        if (auto val = std::get_if<std::string>(&value))
            return *val;
        throw std::runtime_error("Invalid Type");
    }

    Node::operator nlohmann::json() const
    {
        return this->toJson();
    }
}

#ifdef ISTESTPROJECT

#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

using namespace PolyglotAPI;

TEST_CASE("Node Basetest", "[Node]")
{
    Node n_bool = true;
    REQUIRE(std::get<bool>(n_bool.value) == true);

    Node n_double = 10.5;
    REQUIRE(std::get<double>(n_double.value) == 10.5);

    Node n_str = "Hello";
    REQUIRE(std::get<std::string>(n_str.value) == "Hello");
}

TEST_CASE("Node Map Access", "[Node]")
{
    Node x;
    x["Banana"] = 10.0;
    x["Apple"]  = 5.0;

    REQUIRE(std::get<double>(x["Banana"].value) == 10.0);
    REQUIRE(std::get<double>(x["Apple"].value) == 5.0);
}

TEST_CASE("Node to nlohmann::json", "[Node]")
{
    Node x;
    x["key"] = 42.0;

    nlohmann::json j = x.toJson();
    REQUIRE(j.is_object());
    REQUIRE(j["key"] == 42.0);
}

TEST_CASE("Node from nlohmann::json", "[Node]")
{
    nlohmann::json j = {
        { "test", 123.0 }
    };
    Node x = j; // Implizite Konstruktion

    REQUIRE(std::get<double>(x["test"].value) == 123.0);
}

TEST_CASE("Node Callback", "[Node]")
{
    std::function<Node(const Node&)> my_callback = [](const Node& arg) -> Node
    {
        double val = std::get<double>(arg.value);
        return val * 2.0;
    };

    Node cb(my_callback);
    Node result = cb(21.0);
    REQUIRE(std::get<double>(result.value) == 42.0);
}

TEST_CASE("Node access violation", "[Node]")
{
    Node x = 10.0;
    REQUIRE_THROWS_AS(x["fail"], std::runtime_error);
}

TEST_CASE("Node implicit conversion", "[Node]")
{
    Node n_bool = true;
    bool b      = n_bool;
    REQUIRE(b == true);

    Node   n_double = 3.14;
    double d        = n_double;
    REQUIRE(d == 3.14);

    Node        n_str = "Hallo";
    std::string s     = n_str;
    REQUIRE(s == "Hallo");
}
#endif