cmake_minimum_required(VERSION 3.24...4.0.2)

project(PolyglotAPI)

find_package(nlohmann_json REQUIRED)
find_package(Lua REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

############################################################################
############################################################################
############################################################################

set(Api_Files
    "Source/PolyglotAPI/API/API.h"
    "Source/PolyglotAPI/API/API.cpp"
    "Source/PolyglotAPI/API/APIFunction.h"
    "Source/PolyglotAPI/API/APIFunction.cpp"
    "Source/PolyglotAPI/API/FunctionRelay.h"
    "Source/PolyglotAPI/API/FunctionRelay.cpp"
    "Source/PolyglotAPI/API/TestFunctionRelay.cpp"
    "Source/PolyglotAPI/API/TestFunctionRelay.h"
)
source_group("Api" FILES ${Api_Files})

set(Lua_Files
    "Source/PolyglotAPI/Lua/LuaEngine.h"
    "Source/PolyglotAPI/Lua/LuaEngine.cpp"
    "Source/PolyglotAPI/Lua/Conversion.h"
    "Source/PolyglotAPI/Lua/Conversion.cpp"
)
source_group("Lua" FILES ${Lua_Files})

set(Python_Files
    "Source/PolyglotAPI/Python/PythonEngine.cpp"
    "Source/PolyglotAPI/Python/FunctionRelay.cpp"
    "Source/PolyglotAPI/Python/Conversion.cpp"
    "Source/PolyglotAPI/Python/PythonEngine.h"
    "Source/PolyglotAPI/Python/FunctionRelay.h"
    "Source/PolyglotAPI/Python/Conversion.h"
)
source_group("Python" FILES ${Python_Files})

set(main_Files
    "Source/PolyglotAPI/Node.cpp"
    "Source/PolyglotAPI/testmain.cpp"
    "Source/PolyglotAPI/Node.h"
)
source_group("" FILES ${main_Files})

set(ALL_FILES
    ${Lua_Files}
    ${Python_Files}
    ${Api_Files}
    ${main_Files}
)

############################################################################
############################################################################
############################################################################

add_library(PolyglotDependencies INTERFACE)

add_library(PolyglotAPI STATIC ${ALL_FILES})
add_executable(PolyglotAPITest ${ALL_FILES})

set_property(TARGET PolyglotAPI PROPERTY CXX_STANDARD 23)
set_property(TARGET PolyglotAPITest PROPERTY CXX_STANDARD 23)


target_link_libraries(PolyglotDependencies INTERFACE
    nlohmann_json::nlohmann_json
    ${LUA_LIBRARIES}
    Python3::Python
    pybind11::headers
)
target_link_libraries(PolyglotAPI PUBLIC PolyglotDependencies)
target_link_libraries(PolyglotAPITest PRIVATE PolyglotDependencies Catch2::Catch2WithMain)

target_include_directories(PolyglotDependencies INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    ${LUA_INCLUDE_DIR}
    ${Python3_INCLUDE_DIRS}
)

set_property( TARGET PolyglotAPITest PROPERTY COMPILE_DEFINITIONS ISTESTPROJECT=1 )
