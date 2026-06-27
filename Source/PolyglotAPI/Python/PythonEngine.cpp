#include "PythonEngine.h"

#include "Conversion.h"
#include <atomic>
#include <pybind11/embed.h>

namespace PolyglotAPI::Python
{
    class PythonEngine::pimpl
    {
      public:
        std::unique_ptr<pybind11::dict> globals;

        static void ensure_interpreter()
        {
            static std::once_flag init_flag;
            std::call_once(init_flag,
                           []()
                           {
                               std::filesystem::path exe_path   = std::filesystem::current_path();
                               std::filesystem::path python_dir = exe_path / "python";

#ifdef _WIN32
                               static std::wstring python_home = python_dir.wstring();
                               Py_SetPythonHome(python_home.c_str());
#else
                               static std::string python_home = python_dir.string();
                               Py_SetPythonHome(python_home.c_str());
#endif

                               static pybind11::scoped_interpreter guard{};
                           });
        }

        pimpl()
        {
            ensure_interpreter();
            globals = std::make_unique<pybind11::dict>(pybind11::module_::import("builtins").attr("__dict__").attr("copy")());
        }
    };

    PythonEngine::PythonEngine()
      : p(std::make_unique<pimpl>())
    {
    }
    PythonEngine::~PythonEngine() = default;

    void PythonEngine::executeString(const std::string& str)
    {
        pybind11::exec(str, *p->globals);
    }

    void PythonEngine::executeFile(const std::string& filename)
    {
        pybind11::eval_file(filename, *p->globals);
    }

    void PythonEngine::setVar(const std::string& name, const Node& value)
    {
        auto& globals         = *p->globals;
        globals[name.c_str()] = Conversion::node2py(value);
    }

    Node PythonEngine::getVar(const std::string& name)
    {
        if (p->globals->contains(name.c_str()))
        {
            auto& globals = *p->globals;
            return Conversion::py2node(globals[name.c_str()]);
        }
        throw std::runtime_error("Variable not found: " + name);
    }

    bool PythonEngine::addExtension(const std::string& extensionName)
    {
        std::filesystem::path exe_path   = std::filesystem::current_path();
        std::filesystem::path python_exe = exe_path / "python" / "python.exe";
        std::string           command    = "\"" + python_exe.string() + "\" -m pip install " + extensionName;
        int                   result     = std::system(command.c_str());
        return result == 0;
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
    PolyglotAPI::Python::PythonEngine engine1;
    PolyglotAPI::Python::PythonEngine engine2;

    engine1.setVar("x", 1.0);
    engine2.setVar("x", 100.0);

    REQUIRE(static_cast<double>(engine1.getVar("x")) == 1.0);
}

TEST_CASE("PythonEngine: Pip", "[PythonEngine]")
{
    PolyglotAPI::Python::PythonEngine engine;

#ifndef DEBUG
    // numpy does not like debug builds
    engine.executeString("import numpy;");
#else
    WARN("Test skipped: NumPy C-extensions are incompatible with Debug builds.");
#endif

    REQUIRE(true);
}

TEST_CASE("PythonEngine: Dynamic Pip Installation", "[PythonEngine][Pip]")
{
    PolyglotAPI::Python::PythonEngine engine;
    engine.executeString("import pip;");
    REQUIRE_NOTHROW(engine.addExtension("urlman"));
    REQUIRE_NOTHROW(engine.executeString("import urlman;"));
}

#endif