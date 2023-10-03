#pragma once

#include "API/FunctionRelay.h"

namespace pybind11 {
  class object;
}

namespace PolyglotAPI {
  namespace Python {
    class FunctionRelay : public PolyglotAPI::FunctionRelay {
      public:
      FunctionRelay();
      virtual ~FunctionRelay() = default;

      virtual nlohmann::json call(size_t id, const nlohmann::json&) override;

      size_t addFunction(pybind11::object);

      private:
      size_t counter = 1;

      class pimpl;
      std::unique_ptr<pimpl> _pimpl;
    };
  }
}