#include "APIFunction.h"

#include <iostream>

namespace PolyglotAPI {
  APIFunction::APIFunction(const std::string& name, std::function<nlohmann::json(const nlohmann::json&)> func) {
    _name = name;
    _func = func;
  }

  APIFunction::~APIFunction() {

  }

  void APIFunction::setDescription(const std::string& desc) {
    _description = desc;
  }

  nlohmann::json APIFunction::call(const nlohmann::json& input) const {
    try {
      auto result = _func(input);
      return result;
    }
    catch (const std::exception& e) {
      std::cout << " --------error message ------- " << std::endl;
      std::cout << e.what() << std::endl;
      std::cout << " --------error message end ---------- " << std::endl;
      throw e;
    }
  }

  std::string APIFunction::getDescription() const {
    return _description;
  }

  std::string APIFunction::getName() const {
    return _name;
  }
}