#pragma once

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "Polyglot/Engine.h"

struct lua_State;

namespace Polyglot
{
    namespace Lua
    {
        class __declspec(dllexport) LuaEngine : public Polyglot::Engine
        {
          public:
            LuaEngine();
            virtual ~LuaEngine();

            virtual void executeFile(const std::string& filename) override;
            virtual void executeString(const std::string& str) override;

            virtual Node getVar(const std::string& name) const override;
            virtual void setVar(const std::string& name, const Node& value) override;

            virtual bool addExtension(const std::string& extensionName) override
            {
                throw std::runtime_error("Not yet implemented!");
            }; // installs single extension with luarocks

          private:
            void initialize();
            void printTop(int indentation = 0);
            void dumpGlobalVariables(bool fullPrint);

            lua_State* state = nullptr;
        };
    }
}