#pragma once

#include <string>
#include <functional>
#include <nlohmann/json.hpp>

namespace PolyglotAPI {
  /// <summary>
  /// std::function with a name and description strapped on
  /// </summary>
  class __declspec(dllexport) APIFunction { 
    public:
      APIFunction(const std::string& name, std::function<nlohmann::json(const nlohmann::json&)>);
      virtual ~APIFunction();

      void setDescription(const std::string&);

      nlohmann::json call(const nlohmann::json&) const;
      std::string    getDescription() const;
      std::string    getName() const;
    private:
      std::function<nlohmann::json(const nlohmann::json&)> _func       ;
      std::string                                          _name       ;
      std::string                                          _description;
  };
}