#pragma once

#include "PolyglotAPI/API/FunctionRelay.h"

struct lua_State;

namespace PolyglotAPI {
  namespace Lua {
    class FunctionRelay : public PolyglotAPI::FunctionRelay {
      public:
      FunctionRelay(lua_State*);
      virtual ~FunctionRelay() = default;

      virtual nlohmann::json call(size_t id, const nlohmann::json&) override;

      size_t addFunction();

      private:
      size_t     _counter = 1;
      lua_State* _state;
    };
  }
}