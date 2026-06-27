# todo for v1

 * luarocks integration
 * rename internally everything polyglotapi->polyglot
 * class interface "Engine" is missing

# Polyglot

Single API for Python and Lua scripting. Extendable for any normal scripting language.

```cpp
void executeFile(const std::string& filename);
void executeString(const std::string& str);
Node getVar(const std::string& name);
void setVar(const std::string& name, const Node& value);
bool addExtension(const std::string& extensionName);
```

# One Interface, multiple engines

In this example the syntax of the query is the same for python and lua. Usually you need to provide the script in the desired language.

```cpp
void run(Engine& engine)
{
    engine.setVar("a", 10.0);
    engine.executeString("b = a + 5");
    Node result = engine.getVar("b");
    double numberResult = result;
}

LuaEngine lua;
PythonEngine py;
run(lua);
run(py);
```

# "Node" class and Lambdas

Node is an nlohmann::json compatible class, that additionally can store functions.


This works in both ways. Here is a way to call a c++ function from lua.

```cpp
    PolyglotAPI::Lua::LuaEngine engine;
    std::function<Node(const Node& arg)> func = [](const Node& arg) -> Node { return static_cast<double>(arg) * 2.0; };
    engine.setVar("myCallback", func);
    engine.executeString("result = myCallback(21)");
    Node res = engine.getVar("result");
    int result = res;
```

And here one that calls lua from c++

```cpp
    PolyglotAPI::Lua::LuaEngine engine;
    engine.executeString("my_func = function(x) return x * 2 end");
    auto fun = engine.getVar("my_func");
    Node result = fun(21.0);
    double result = res;
```

# Language Extensions

There are cmake these variables to install the packages in the buildstep.

You can also decide here to not compile a language.

```cmake
option(POLYGLOT_ENABLE_PYTHON "Polyglot Python" ON)
option(POLYGLOT_ENABLE_LUA "Polyglot Lua" ON)
set(POLYGLOT_PYTHON_EXTENSIONS "numpy" CACHE STRING "Space-separated list of pip packages")
set(POLYGLOT_LUA_EXTENSIONS "LuaFileSystem" CACHE STRING "Space-separated list of luarocks packages")
```

you also can add extensions at runtime:

```cpp
Polyglot::Python::PythonEngine engine;
engine.addExtension("urlman");
engine.executeString("import urlman;");
```

```cpp
Polyglot::Lua::LuaEngine engine;
engine.addExtension("LuaFileSystem");
engine.executeString("local lfs = require\"lfs\";");
```

# Installation

This project utilizes vcpkg and CMake for dependency management and building.

You need to specify a toolchain file in cmake.
```
 -DCMAKE_TOOLCHAIN_FILE=[path_to_vcpkg]/scripts/buildsystems/vcpkg.cmake
```
