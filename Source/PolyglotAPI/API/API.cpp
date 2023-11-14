#include "API.h"

namespace PolyglotAPI {
  API::API(const std::string& name) {
    _name = name;
  }

  void API::addFunction(std::unique_ptr<APIFunction> func) {
    std::string name = func->getName();
    _functions.push_back(std::move(func));
    _name2func[name] = _functions.size() - 1;
  }

  void API::setDescription(const std::string& desc) {
    _description = desc;
  }

  size_t API::numberOfFunctions() const {
    return _functions.size();
  }

  APIFunction& API::getFunction(size_t number) {
    return *_functions[number];
  }

  APIFunction& API::getFunction(const std::string& name) {
    return getFunction(_name2func[name]);
  }
}