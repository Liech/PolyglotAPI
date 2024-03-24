#pragma once

#include <nlohmann/json.hpp>

struct lua_State;

namespace PolyglotAPI {
  namespace Lua {
    class FunctionRelay;

    class __declspec(dllexport) Conversion {
      public:
      Conversion(lua_State*, Lua::FunctionRelay&);

      void toTable(const nlohmann::json& json);
      void toJson(nlohmann::json& json);
      std::string popStr(int pos);

      private:
        void dumpstack(lua_State* L);

      Lua::FunctionRelay& _relay;
      lua_State*          _state;
    };
  }
}