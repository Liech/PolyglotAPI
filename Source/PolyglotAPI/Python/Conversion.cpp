#include "Conversion.h"

#include <Python.h>
#include <pybind11/complex.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "PolyglotAPI/Node.h"
#include <iostream>

namespace PolyglotAPI::Python
{
    Node Conversion::py2node(const pybind11::object& input)
    {
        if (pybind11::isinstance<pybind11::bool_>(input))
            return input.cast<bool>();

        if (pybind11::isinstance<pybind11::float_>(input))
            return input.cast<double>();

        if (pybind11::isinstance<pybind11::int_>(input))
            return input.cast<int>();

        if (pybind11::isinstance<pybind11::str>(input))
            return input.cast<std::string>();

        if (input.is_none())
            return Node();

        if (pybind11::isinstance<pybind11::dict>(input))
        {
            std::map<std::string, Node> dict_map;
            for (auto item : input.cast<pybind11::dict>())
            {
                dict_map[item.first.cast<std::string>()] = py2node(item.second.cast<pybind11::object>());
            }
            return Node(dict_map);
        }

        if (pybind11::isinstance<pybind11::list>(input) || pybind11::isinstance<pybind11::tuple>(input))
        {
            std::vector<Node> vec;
            for (auto item : input.cast<pybind11::iterable>())
            {
                vec.push_back(py2node(item.cast<pybind11::object>()));
            }
            return Node(vec);
        }

        if (pybind11::isinstance<pybind11::function>(input))
        {
            pybind11::function func = input.cast<pybind11::function>();
            return Node(
              [func](const Node& arg) -> Node
              {
                  pybind11::object py_arg = node2py(arg);
                  pybind11::object py_res = func(py_arg);
                  return py2node(py_res);
              });
        }

        throw std::runtime_error("Type conversion not supported");
    }

    pybind11::object Conversion::node2py(const Node& input)
    {
        return std::visit(
          [](auto&& arg) -> pybind11::object
          {
              using T = std::decay_t<decltype(arg)>;

              if constexpr (std::is_same_v<T, bool>)
                  return pybind11::cast(arg);
              else if constexpr (std::is_same_v<T, double>)
                  return pybind11::cast(arg);
              else if constexpr (std::is_same_v<T, int>)
                  return pybind11::cast(arg);
              else if constexpr (std::is_same_v<T, float>)
                  return pybind11::cast(arg);
              else if constexpr (std::is_same_v<T, std::string>)
                  return pybind11::cast(arg);
              else if constexpr (std::is_same_v<T, std::nullptr_t>)
                  return pybind11::none();
              else if constexpr (std::is_same_v<T, std::map<std::string, Node>>)
              {
                  pybind11::dict d;
                  for (auto& [k, v] : arg)
                      d[pybind11::str(k)] = node2py(v);
                  return d;
              }
              else if constexpr (std::is_same_v<T, std::vector<Node>>)
              {
                  pybind11::list l;
                  for (auto& v : arg)
                      l.append(node2py(v));
                  return l;
              }
              else if constexpr (std::is_same_v<T, std::function<Node(const Node&)>>)
              {
                  return pybind11::cpp_function(
                    [arg](pybind11::args py_args) -> pybind11::object
                    {
                        Node n_args;
                        if (py_args.size() == 0)
                        {
                            n_args = Node(nullptr);
                        }
                        else if (py_args.size() == 1)
                        {
                            // Konvertiere handle zu object
                            n_args = py2node(pybind11::reinterpret_borrow<pybind11::object>(py_args[0]));
                        }
                        else
                        {
                            std::vector<Node> vec;
                            for (auto item : py_args)
                            {
                                // Auch hier: item ist ein handle, cast zu object
                                vec.push_back(py2node(pybind11::reinterpret_borrow<pybind11::object>(item)));
                            }
                            n_args = Node(vec);
                        }

                        Node result = arg(n_args);
                        return node2py(result);
                    });
              }
              else
              {
                  throw std::runtime_error("Type conversion not supported");
              }
          },
          input.value);
    }
}
