#pragma once

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

namespace PolyglotAPI {
  class API;
  class APIFunction;
  class FunctionRelay;
}
namespace pybind11 {
  class object;
  class module_;
}

namespace PolyglotAPI {
  namespace Python {
    class PythonEngine {
      public:
      static PythonEngine& instance();
      virtual ~PythonEngine();

      void addAPI(std::shared_ptr<API>);
      void initialize();
      void dispose();

      void execute(const std::string& pythonCode);
      void executeFile(const std::string& filename);

      size_t numberOfApis() const;
      API& getAPI(size_t number);
      PolyglotAPI::FunctionRelay& getRelay();

      void addCustomFunction(const std::string& name, std::function<void(pybind11::module_&)> fun);
      const std::map<std::string,std::function<void(pybind11::module_&)>>& getCustomFunctions() const;

    private:
      PythonEngine();

      bool _initialized = false;

      class pimpl;
      std::unique_ptr<pimpl> _pimpl;
    };
  }
}