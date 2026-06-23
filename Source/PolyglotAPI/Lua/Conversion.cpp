#include "Conversion.h"

#include "PolyglotAPI/Node.h"
#include <lua.hpp>

namespace PolyglotAPI::Lua
{
    Conversion::Conversion(lua_State* state)
    {
        _state = state;
    }

    Node Conversion::toNode()
    {
        int type = lua_type(_state, -1);

        switch (type)
        {
            case LUA_TNIL:
                return Node(nullptr);

            case LUA_TBOOLEAN:
                return Node((bool)lua_toboolean(_state, -1));

            case LUA_TNUMBER:
                return Node((double)lua_tonumber(_state, -1));

            case LUA_TSTRING:
                return Node(std::string(lua_tostring(_state, -1)));

            case LUA_TTABLE:
            {
                std::map<std::string, Node> table;
                lua_pushnil(_state);
                while (lua_next(_state, -2) != 0)
                {
                    std::string key = lua_tostring(_state, -2);
                    table[key]      = this->toNode();
                    lua_pop(_state, 1);
                }
                return Node(table);
            }

            case LUA_TFUNCTION:
            {
                lua_pushvalue(_state, -1);
                int  r   = luaL_ref(_state, LUA_REGISTRYINDEX);
                auto ref = std::shared_ptr<int>(new int(r),
                                                [this](int* ptr)
                                                {
                                                    luaL_unref(_state, LUA_REGISTRYINDEX, *ptr);
                                                    delete ptr;
                                                });

                return Node(
                  [this, ref](const Node& arg) -> Node
                  {
                      lua_rawgeti(_state, LUA_REGISTRYINDEX, *ref);
                      this->toTable(arg.toJson());
                      lua_pcall(_state, 1, 1, 0);
                      Node result = this->toNode();
                      lua_pop(_state, 1);
                      return result;
                  });
            }

            default:
                throw std::runtime_error("Unkown Lua-Type: " + std::string(lua_typename(_state, type)));
        }
    }

    void Conversion::toTable(const Node& node)
    {
        std::visit(
          [this](auto&& arg)
          {
              using T = std::decay_t<decltype(arg)>;

              if constexpr (std::is_same_v<T, std::nullptr_t>)
              {
                  lua_pushnil(_state);
              }
              else if constexpr (std::is_same_v<T, bool>)
              {
                  lua_pushboolean(_state, arg ? 1 : 0);
              }
              else if constexpr (std::is_same_v<T, double>)
              {
                  lua_pushnumber(_state, arg);
              }
              else if constexpr (std::is_same_v<T, std::string>)
              {
                  lua_pushstring(_state, arg.c_str());
              }
              else if constexpr (std::is_same_v<T, std::map<std::string, Node>>)
              {
                  lua_newtable(_state);
                  for (const auto& [key, value] : arg)
                  {
                      lua_pushstring(_state, key.c_str());
                      toTable(value); // Rekursiver Aufruf
                      lua_settable(_state, -3);
                  }
              }
              else if constexpr (std::is_same_v<T, std::vector<Node>>)
              {
                  lua_newtable(_state);
                  int index = 1; // Lua-Arrays sind 1-basiert
                  for (const auto& value : arg)
                  {
                      lua_pushnumber(_state, index++);
                      toTable(value); // Rekursiver Aufruf
                      lua_settable(_state, -3);
                  }
              }
              else if constexpr (std::is_same_v<T, std::function<Node(const Node&)>>)
              {
                  auto  func_ptr = std::make_shared<std::function<Node(const Node&)>>(arg);
                  auto* ud       = static_cast<std::shared_ptr<std::function<Node(const Node&)>>*>(lua_newuserdata(_state, sizeof(std::shared_ptr<std::function<Node(const Node&)>>)));
                  new (ud) std::shared_ptr<std::function<Node(const Node&)>>(func_ptr);
                  lua_newtable(_state);
                  lua_pushcfunction(_state,
                                    [](lua_State* L)
                                    {
                                        auto* ptr = static_cast<std::shared_ptr<std::function<Node(const Node&)>>*>(lua_touserdata(L, 1));
                                        ptr->~shared_ptr();
                                        return 0;
                                    });
                  lua_setfield(_state, -2, "__gc");
                  lua_setmetatable(_state, -2);

                  lua_pushcclosure(
                    _state,
                    [](lua_State* L) -> int
                    {
                        auto*      ptr = static_cast<std::shared_ptr<std::function<Node(const Node&)>>*>(lua_touserdata(L, lua_upvalueindex(1)));
                        Conversion conv(L);
                        int        num_args = lua_gettop(L);
                        Node       arg;

                        if (num_args == 0)
                        {
                            arg = Node(nullptr);
                        }
                        else if (num_args == 1)
                        {
                            arg = conv.toNode();
                        }
                        else
                        {
                            std::vector<Node> vec;
                            for (int i = 1; i <= num_args; ++i)
                            {
                                lua_pushvalue(L, i);
                                vec.push_back(conv.toNode());
                                lua_pop(L, 1);
                            }
                            arg = Node(vec);
                        }

                        Node       res  = (**ptr)(arg);
                        conv.toTable(res);
                        return 1;
                    },
                    1);
              }
          },
          node.value);
    }

    std::string Conversion::popStr(int pos)
    {
        if (lua_isnumber(_state, pos))
            return std::to_string(lua_tonumber(_state, pos));
        else if (lua_isstring(_state, pos))
            return lua_tostring(_state, pos);

        throw std::runtime_error("Unkown");
    }

    void Conversion::dumpstack(lua_State* L)
    {
        int top = lua_gettop(L);
        for (int i = 1; i <= top; i++)
        {
            printf("%d\t%s\t", i, luaL_typename(L, i));
            switch (lua_type(L, i))
            {
                case LUA_TNUMBER:
                    printf("%g\n", lua_tonumber(L, i));
                    break;
                case LUA_TSTRING:
                    printf("%s\n", lua_tostring(L, i));
                    break;
                case LUA_TBOOLEAN:
                    printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
                    break;
                case LUA_TNIL:
                    printf("%s\n", "nil");
                    break;
                default:
                    printf("%p\n", lua_topointer(L, i));
                    break;
            }
        }
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

using namespace PolyglotAPI;

struct LuaStateGuard
{
    lua_State* L;
    LuaStateGuard()
    {
        L = luaL_newstate();
        luaL_openlibs(L);
    }
    ~LuaStateGuard()
    {
        lua_close(L);
    }
};

TEST_CASE("Conversion: Lua -> Node -> Lua", "[Conversion]")
{
    LuaStateGuard                guard;
    PolyglotAPI::Lua::Conversion conv(guard.L);

    lua_pushnumber(guard.L, 42.5);
    Node n_num = conv.toNode();
    REQUIRE(std::get<double>(n_num.value) == 42.5);

    lua_pushboolean(guard.L, 1);
    Node n_bool = conv.toNode();
    REQUIRE(std::get<bool>(n_bool.value) == true);

    lua_pushstring(guard.L, "TestString");
    Node n_str = conv.toNode();
    REQUIRE(std::get<std::string>(n_str.value) == "TestString");
}

TEST_CASE("Conversion: Table (Map) Lua -> Node", "[Conversion]")
{
    LuaStateGuard                guard;
    PolyglotAPI::Lua::Conversion conv(guard.L);

    lua_newtable(guard.L);
    lua_pushstring(guard.L, "key");
    lua_pushnumber(guard.L, 100);
    lua_settable(guard.L, -3);

    Node  n = conv.toNode();
    auto& m = std::get<std::map<std::string, Node>>(n.value);
    REQUIRE(std::get<double>(m["key"].value) == 100.0);
}

TEST_CASE("Conversion: Node -> Lua Table", "[Conversion]")
{
    LuaStateGuard                guard;
    PolyglotAPI::Lua::Conversion conv(guard.L);

    Node n;
    n["a"] = 1.0;
    n["b"] = "hello";

    conv.toTable(n);

    REQUIRE(lua_istable(guard.L, -1));

    lua_pushstring(guard.L, "a");
    lua_gettable(guard.L, -2);
    REQUIRE(lua_tonumber(guard.L, -1) == 1.0);
    lua_pop(guard.L, 1);

    lua_pushstring(guard.L, "b");
    lua_gettable(guard.L, -2);
    REQUIRE(std::string(lua_tostring(guard.L, -1)) == "hello");
}

TEST_CASE("Conversion: Lua Callback -> Node Call", "[Conversion]")
{
    LuaStateGuard                guard;
    PolyglotAPI::Lua::Conversion conv(guard.L);

    luaL_dostring(guard.L, "my_func = function(x) return x * 2 end");
    lua_getglobal(guard.L, "my_func");

    Node n = conv.toNode();

    Node result = n(21.0);
    REQUIRE(std::get<double>(result.value) == 42.0);
}

#endif