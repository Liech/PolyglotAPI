#include "PythonEngine.h"

#include "Conversion.h"
#include "PolyglotAPI/Node.h"
#include <atomic>
#include <pybind11/embed.h>

namespace PolyglotAPI::Python
{
    PythonEngine::PythonEngine()
    {
        if (instanceCount++ == 0)
        {
            pybind11::initialize_interpreter();
        }
    }

    PythonEngine::~PythonEngine()
    {
        if (--instanceCount == 0)
        {
            pybind11::finalize_interpreter();
        }
    }

    void PythonEngine::executeString(const std::string& str)
    {
        pybind11::gil_scoped_acquire acquire;
        pybind11::exec(str);
    }
    
    void PythonEngine::executeFile(const std::string& filename)
    {
        pybind11::gil_scoped_acquire acquire;
        pybind11::eval_file(filename, pybind11::globals());
    }

    void PythonEngine::setVar(const std::string& name, const Node& value)
    {
        pybind11::gil_scoped_acquire acquire;
        pybind11::globals()[name.c_str()] = Conversion::node2py(value);
    }

    Node PythonEngine::getVar(const std::string& name)
    {
        pybind11::gil_scoped_acquire acquire;
        return Conversion::py2node(pybind11::globals()[name.c_str()]);
    }
}


#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

using namespace PolyglotAPI;

TEST_CASE("PythonEngine: Simple", "[PythonEngine]")
{
    PolyglotAPI::Python::PythonEngine engine;

    engine.setVar("a", 10.0);
    engine.executeString("b = a + 5");

    Node res = engine.getVar("b");
    REQUIRE(static_cast<double>(res) == 15.0);
}

TEST_CASE("PythonEngine: List&Dict", "[PythonEngine]")
{
    PolyglotAPI::Python::PythonEngine engine;

    Node n;
    n["key"] = "value";

    engine.setVar("myDict", n);
    engine.executeString("myList = [myDict['key'], 'test']");

    Node res = engine.getVar("myList");
    auto vec = std::get<std::vector<Node>>(res.value);

    REQUIRE(static_cast<std::string>(vec[0]) == "value");
    REQUIRE(static_cast<std::string>(vec[1]) == "test");
}

TEST_CASE("PythonEngine: Callback C++ -> Python", "[PythonEngine]")
{
    PolyglotAPI::Python::PythonEngine engine;

    std::function<Node(const Node&)> cb = [](const Node& arg) -> Node { return static_cast<double>(arg) * 2.0; };

    engine.setVar("pyCallback", cb);
    engine.executeString("result = pyCallback(21)");

    Node res = engine.getVar("result");
    REQUIRE(static_cast<double>(res) == 42.0);
}

TEST_CASE("PythonEngine: Callback Python -> C++", "[PythonEngine]")
{
    PolyglotAPI::Python::PythonEngine engine;

    engine.executeString("def my_adder(x): return x + 10");
    Node func = engine.getVar("my_adder");

    Node res = func(5.0);
    REQUIRE(static_cast<double>(res) == 15.0);
}

TEST_CASE("PythonEngine: Instances", "[PythonEngine]")
{
    // more a warning then a test!

    PolyglotAPI::Python::PythonEngine engine1;
    PolyglotAPI::Python::PythonEngine engine2;

    engine1.setVar("x", 1.0);
    engine2.setVar("x", 100.0);

    REQUIRE(static_cast<double>(engine1.getVar("x")) == 100.0);
}

#endif