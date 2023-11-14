#pragma once

#include <functional>
#include "PolyglotAPI/API/FunctionRelay.h"

namespace PolyglotAPI {
    class TestFunctionRelay : public PolyglotAPI::FunctionRelay {
    public:
      TestFunctionRelay(std::function<nlohmann::json(size_t,const nlohmann::json&)> callback);
      virtual ~TestFunctionRelay();

      virtual nlohmann::json call(size_t id, const nlohmann::json&) override;

    private:
      size_t     _counter = 1;
      std::function<nlohmann::json(size_t, const nlohmann::json&)> _callback;
    };
}