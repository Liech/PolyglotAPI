#pragma once

#include "Polyglot/Node.h"
#include <nlohmann/json.hpp>

namespace Polyglot
{
    class __declspec(dllexport) Engine
    {
      public:
        virtual void executeString(const std::string& str)              = 0;
        virtual void executeFile(const std::string& filename)           = 0;
        virtual void setVar(const std::string& name, const Node& value) = 0;
        virtual Node getVar(const std::string& name) const              = 0;
        virtual bool addExtension(const std::string& extensionName)     = 0; // installs single extension with pip/luarocks/...
    };
}
