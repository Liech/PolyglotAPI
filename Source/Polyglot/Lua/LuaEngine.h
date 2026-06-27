#pragma once

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

struct lua_State;

namespace PolyglotAPI
{
    class Node;

    namespace Lua
    {
        class __declspec(dllexport) LuaEngine
        {
          public:
            LuaEngine();
            virtual ~LuaEngine();

            void executeFile(const std::string& filename);
            void executeString(const std::string& str);

            Node getVar(const std::string& name);
            void setVar(const std::string& name, const Node& value);

          private:
            void initialize();
            void printTop(int indentation = 0);
            void dumpGlobalVariables(bool fullPrint);

            lua_State* state = nullptr;
        };
    }
}