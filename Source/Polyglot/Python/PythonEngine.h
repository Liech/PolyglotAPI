#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <vector>
#include "Polyglot/Node.h"

namespace pybind11
{
    class object;
    class module_;
}

namespace PolyglotAPI
{
    namespace Python
    {
        class __declspec(dllexport) PythonEngine
        {
          public:
            PythonEngine();
            virtual ~PythonEngine();

            void executeString(const std::string& str);
            void executeFile(const std::string& filename);

            void setVar(const std::string& name, const Node& value);
            Node getVar(const std::string& name);
            
            bool addExtension(const std::string& extensionName); // installs single extension with pip

          private:
            class pimpl;
            std::unique_ptr<pimpl> p = nullptr;
        };
    }
}
