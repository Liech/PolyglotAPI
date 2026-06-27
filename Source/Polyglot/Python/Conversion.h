#pragma once

namespace pybind11
{
    class object;
}

namespace Polyglot
{
    class Node;

    namespace Python
    {
        class Conversion
        {
          public:
            static Node             py2node(const pybind11::object&);
            static pybind11::object node2py(const Node&);
        };
    }
}