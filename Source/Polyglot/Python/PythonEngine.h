#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <vector>
#include "Polyglot/Node.h"
#include "Polyglot/Engine.h"

namespace pybind11
{
    class object;
    class module_;
}

namespace Polyglot
{
    namespace Python
    {
        class __declspec(dllexport) PythonEngine : public Polyglot::Engine
        {
          public:
            PythonEngine();
            virtual ~PythonEngine();

            virtual void executeString(const std::string& str) override;
            virtual void executeFile(const std::string& filename) override ;

            virtual void setVar(const std::string& name, const Node& value) override;
            virtual Node getVar(const std::string& name) const override;
            
            virtual bool addExtension(const std::string& extensionName) override; // installs single extension with pip

          private:
            class pimpl;
            std::unique_ptr<pimpl> p = nullptr;
        };
    }
}
