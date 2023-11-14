#pragma once

#include <memory>
#include <map>
#include <string>
#include "APIFunction.h"

namespace PolyglotAPI {
  /// <summary>
  /// defined in libraries to make global functions avaiable to scripting
  /// input may contain script functions (disguised numbers). To call them a FunctionRelay must be known.
  /// </summary>
  class API {
    public:
      API(const std::string& name);
      void addFunction(std::unique_ptr<APIFunction> func);
      void setDescription(const std::string& desc);

      size_t numberOfFunctions() const;
      APIFunction& getFunction(size_t number);
      APIFunction& getFunction(const std::string& name);
    private:
      std::vector<std::unique_ptr<APIFunction>> _functions;
      std::map<std::string, size_t>             _name2func;
      std::string                               _name;
      std::string                               _description;
  };
}