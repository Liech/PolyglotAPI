#pragma once

#include <nlohmann/json.hpp>

namespace PolyglotAPI {
  /// <summary>
  /// Deserializes numbers into callable functions. 
  /// E.g. a script gives an APIFunction a Function (disguised as number) in its parameters. To call it an Relay is needed
  /// </summary>
  class __declspec(dllexport) FunctionRelay {
    public:
      virtual nlohmann::json call(size_t id, const nlohmann::json&) = 0;
  };
}