#pragma once

#include <string>

struct lua_State;

namespace Polyglot
{
    class Node;

    namespace Lua
    {
        class __declspec(dllexport) Conversion
        {
          public:
            Conversion(lua_State*);

            void        toTable(const Node& json);
            Node        toNode();
            std::string popStr(int pos);

          private:
            void dumpstack(lua_State* L);

            lua_State*          _state;
        };
    }
}