#include "TestFunctionRelay.h"

namespace PolyglotAPI {
  TestFunctionRelay::TestFunctionRelay(std::function<nlohmann::json(size_t, const nlohmann::json&)> callback) {
    _callback = callback;
  }

  TestFunctionRelay::~TestFunctionRelay() {

  }

  nlohmann::json TestFunctionRelay::call(size_t id, const nlohmann::json& parameters) {
    return _callback(id, parameters);
  }
}