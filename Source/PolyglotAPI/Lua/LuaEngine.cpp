#include "LuaEngine.h"

#include <iostream>
#include <lua.hpp>
#include <sstream>
#include <stdexcept>

#include "PolyglotAPI/Node.h"

#include "Conversion.h"

namespace PolyglotAPI
{
    namespace Lua
    {
        LuaEngine::LuaEngine()
        {
            initialize();
        }

        LuaEngine::~LuaEngine()
        {
            lua_close(state);
        }

        void LuaEngine::initialize()
        {
            state = luaL_newstate();
            luaL_openlibs(state);
        }

        void LuaEngine::executeFile(const std::string& filename)
        {
            if (luaL_loadfile(state, filename.c_str()) == LUA_OK)
            {
                int returnCode = lua_pcall(state, 0, LUA_MULTRET, 0);
                if (returnCode != LUA_OK)
                {
                    std::cout << lua_tostring(state, -1) << std::endl;
                    throw std::runtime_error("Error in call");
                }
            }
            else
            {
                std::cout << lua_tostring(state, -1) << std::endl;
                throw std::runtime_error("File not found or Syntax Error");
            }
        }

        void LuaEngine::executeString(const std::string& str)
        {
            int returnCode = luaL_dostring(state, str.c_str());
            if (returnCode != LUA_OK)
            {
                std::string        lined   = "";
                std::string        current = "";
                std::istringstream f(str);
                size_t             lineNo = 0;
                while (std::getline(f, current, '\n'))
                {
                    lined += std::to_string(lineNo) + " " + current + "\n";
                    lineNo++;
                }

                std::cout << lined << std::endl;
                std::cout << lua_tostring(state, -1) << std::endl;
                throw std::runtime_error("Error in call");
            }
        }

        void LuaEngine::dumpGlobalVariables(bool fullPrint)
        {
            lua_pushglobaltable(state); // Get global table
            lua_pushnil(state);         // put a nil key on stack
            while (lua_next(state, -2) != 0)
            {                                                      // key(-1) is replaced by the next key(-1) in table(-2)
                std::cout << lua_tostring(state, -2) << std::endl; // Get key(-2) name
                if (fullPrint)
                    printTop(1);
                lua_pop(state, 1); // remove value(-1), now key on top at(-1)
            }
            lua_pop(state, 1); // remove global table(-1)
        }

        void LuaEngine::printTop(int indentation)
        {
            Conversion  conversion(state);
            std::string indent = "";
            for (int i = 0; i < indentation; i++)
                indent += " ";

            if (!lua_istable(state, -1))
            {
                if (lua_isstring(state, -1))
                {
                    std::cout << indent << conversion.popStr(-1) << std::endl;
                }
                else if (lua_isnumber(state, -1))
                {
                    std::cout << indent << lua_tonumber(state, -1) << std::endl;
                }
                else if (lua_isboolean(state, -1))
                {
                    std::cout << indent << lua_toboolean(state, -1) << std::endl;
                }
                else
                    std::cout << indent << "UNKOWN" << std::endl;
                return;
            }

            lua_pushnil(state); /* first key */
            int amount = 0;
            while (lua_next(state, -2) != 0)
            {
                if (lua_isstring(state, -1))
                {
                    std::cout << indent << conversion.popStr(-2) << ": " << conversion.popStr(-1) << std::endl;
                }
                else if (lua_isnumber(state, -1))
                {
                    std::cout << indent << conversion.popStr(-2) << ": " << lua_tonumber(state, -1) << std::endl;
                }
                else if (lua_isboolean(state, -1))
                {
                    std::cout << indent << conversion.popStr(-2) << ": " << lua_toboolean(state, -1) << std::endl;
                }
                else if (lua_istable(state, -1))
                {
                    printTop(indentation + 1);
                }
                else
                    std::cout << indent << "UNKOWN" << std::endl;
                lua_pop(state, 1);
            }
        }

        void LuaEngine::setVar(const std::string& name, const Node& value)
        {
            Conversion conversion(state);
            conversion.toTable(value);
            lua_setglobal(state, name.c_str());
        }

        Node LuaEngine::getVar(const std::string& name)
        {
            Conversion conversion(state);
            lua_getglobal(state, name.c_str());
            Node result = conversion.toNode();
            lua_pop(state, 1);
            return result;
        }
    }
}



#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

using namespace PolyglotAPI;

TEST_CASE("LuaEngine: Set/Get var", "[LuaEngine]")
{
    PolyglotAPI::Lua::LuaEngine engine;

    Node n_num = 42.0;
    engine.setVar("myNum", n_num);
    REQUIRE((double)engine.getVar("myNum") == 42.0);

    Node n_str = "HelloLua";
    engine.setVar("myStr", n_str);
    REQUIRE(static_cast<std::string>(engine.getVar("myStr")) == "HelloLua");
}

TEST_CASE("LuaEngine: Manipulate via Lua", "[LuaEngine]")
{
    PolyglotAPI::Lua::LuaEngine engine;
    engine.setVar("x", 10.0);
    engine.executeString("x = x + 5");
    REQUIRE((double)engine.getVar("x") == 15.0);
}

TEST_CASE("LuaEngine: Table Handling", "[LuaEngine]")
{
    PolyglotAPI::Lua::LuaEngine engine;

    Node table;
    table["a"] = 1.0;
    table["b"] = 2.0;

    engine.setVar("myTable", table);
    engine.executeString("sum = myTable.a + myTable.b");
    Node sum = engine.getVar("sum");
    REQUIRE(static_cast<double>(sum) == 3.0);
}

TEST_CASE("LuaEngine: Callback Test", "[LuaEngine]")
{
    PolyglotAPI::Lua::LuaEngine engine;

    std::function<Node(const Node& arg)> func = [](const Node& arg) -> Node { return static_cast<double>(arg) * 2.0; };
    Node                                 cb   = func;

    engine.setVar("myCallback", cb);
    engine.executeString("result = myCallback(21)");
    Node res = engine.getVar("result");

    REQUIRE(static_cast<double>(res) == 42.0);
}

TEST_CASE("LuaEngine: Dynamische Parameter-Übergabe", "[LuaEngine]")
{
    PolyglotAPI::Lua::LuaEngine engine;

    std::function<Node(const Node& arg)> func = [](const Node& arg) -> Node
    {
        if (std::holds_alternative<std::vector<Node>>(arg.value))
        {
            const auto& vec = std::get<std::vector<Node>>(arg.value);
            return static_cast<double>(vec.size());
        }
        return 1.0;
    };

    engine.setVar("myFunc", func);

    engine.executeString("res1 = myFunc(10)");
    Node res1 = engine.getVar("res1");
    REQUIRE(static_cast<double>(res1) == 1.0);

    engine.executeString("res2 = myFunc(10, 20, 30)");
    Node res2 = engine.getVar("res2");
    REQUIRE(static_cast<double>(res2) == 3.0);
}

TEST_CASE("LuaEngine: Summen-Callback mit Vektor-Argumenten", "[LuaEngine]")
{
    PolyglotAPI::Lua::LuaEngine engine;

    std::function<Node(const Node& arg)> func = [](const Node& arg) -> Node
    {
        double sum = 0;
        if (std::holds_alternative<std::vector<Node>>(arg.value))
        {
            for (const auto& n : std::get<std::vector<Node>>(arg.value))
            {
                sum += static_cast<double>(n);
            }
        }
        else
        {
            sum = static_cast<double>(arg);
        }
        return sum;
    };

    engine.setVar("sumFunc", func);

    engine.executeString("res = sumFunc(10, 20, 5)");
    Node res = engine.getVar("res");

    REQUIRE(static_cast<double>(res) == 35.0);
}

TEST_CASE("LuaEngine: Lua Callback -> Node Call", "[LuaEngine]")
{
    PolyglotAPI::Lua::LuaEngine engine;
    engine.executeString("my_func = function(x) return x * 2 end");
    auto fun = engine.getVar("my_func");

    Node result = fun(21.0);
    REQUIRE(std::get<double>(result.value) == 42.0);
}

#endif