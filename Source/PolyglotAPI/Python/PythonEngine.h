#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

namespace pybind11
{
    class object;
    class module_;
}

namespace PolyglotAPI
{
    class Node;

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

          private:
            inline static std::atomic<int> instanceCount = 0;
        };
    }
}
