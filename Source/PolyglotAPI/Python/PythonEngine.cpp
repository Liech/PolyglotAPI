#include "PythonEngine.h"

#include "FunctionRelay.h"
#include "Conversion.h"

#include "API/API.h"
#include "API/APIFunction.h"
#include "API/FunctionRelay.h"

#include <python3.10/Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <fstream>
#include <filesystem>


namespace PolyglotAPI {
  namespace Python {
    class PythonEngine::pimpl {
      public:
      pybind11::module_ mainModule;
      std::unique_ptr<Python::FunctionRelay> relay = nullptr;
      std::vector<std::shared_ptr<API>>   _apis;
      std::map<std::string, std::function<void(pybind11::module_&)>> customFunctions;
    };

    PythonEngine& PythonEngine::instance() {
      static PythonEngine engine;
      return engine;
    }

    PythonEngine::~PythonEngine() {
      if (_initialized)
        std::cout<<"Please dispose() the PythonEngine correctly"<<std::endl;
    }

    PythonEngine::PythonEngine() {
      _pimpl = std::make_unique<PythonEngine::pimpl>();
      _pimpl->relay = std::make_unique<Python::FunctionRelay>();

    }

    
    //pybind11::array_t<int> get_indices(int& g) {
    //  // an empty capsule
    //  return pybind11::array_t<int>{5, g.indices, pybind11::capsule{}};
    //}

    PYBIND11_EMBEDDED_MODULE(PolyglotModule, m) {
      PythonEngine& engine = PythonEngine::instance();

      for (size_t apiID = 0; apiID < engine.numberOfApis(); apiID++) {
        auto& api = engine.getAPI(apiID);
        for (size_t f = 0; f < api.numberOfFunctions(); f++) {
          auto& func = api.getFunction(f);
          m.def(func.getName().c_str(), [&func](const pybind11::object& input) {
            auto& r = (Python::FunctionRelay&)PythonEngine::instance().getRelay();
            return Conversion::j2py(func.call(Conversion::py2j(input, r)));
            }, pybind11::arg("input") = pybind11::none());
        }
      }
      for (auto& func : engine.getCustomFunctions()) {
        func.second(m);
      }
    }

    void PythonEngine::initialize() {
      assert(!_initialized);
      _initialized = true;
      if (!_pimpl) {
        _pimpl = std::make_unique<PythonEngine::pimpl>();
        _pimpl->relay = std::make_unique<Python::FunctionRelay>();
      }

      try {
        Py_SetPythonHome(L"Data/python");
        pybind11::initialize_interpreter();
        _pimpl->mainModule = pybind11::module_::import("PolyglotModule");
      } catch (pybind11::error_already_set& e) {
        std::cout << e.what() << std::endl;
        throw;
      }
    }

    void PythonEngine::execute(const std::string& pythonCode) {
      try {
        auto locals = pybind11::dict(**_pimpl->mainModule.attr("__dict__"));
        pybind11::exec(pythonCode, pybind11::globals(), locals);
      } catch (pybind11::error_already_set& e) {
        std::cout << e.what() << std::endl;
        throw;
      }
    }

    void PythonEngine::executeFile(const std::string& filename) {
      if (!std::filesystem::exists(filename))
        throw std::runtime_error("File not found!");

      std::ifstream t(filename);
      std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
      execute(str);
    }

    void PythonEngine::dispose() {
      assert(_initialized);
      _pimpl = nullptr;
      pybind11::finalize_interpreter();
      _initialized = false;
    }

    void PythonEngine::addAPI(std::shared_ptr<API> api) {
      assert(!_initialized);
      _pimpl->_apis.push_back(api);
    }

    size_t PythonEngine::numberOfApis() const {
      return _pimpl->_apis.size();
    }

    API& PythonEngine::getAPI(size_t number) {
      return *_pimpl->_apis[number];
    }

    PolyglotAPI::FunctionRelay& PythonEngine::getRelay() {
      return *_pimpl->relay;
    }

    void PythonEngine::addCustomFunction(const std::string& name, std::function<void(pybind11::module_&)> fun) {
      _pimpl->customFunctions[name] = fun;
    }

    const std::map<std::string, std::function<void(pybind11::module_&)>>& PythonEngine::getCustomFunctions() const {
      return _pimpl->customFunctions;
    }

  }
}