cmake_minimum_required(VERSION 2.8...3.23)
project(PolyglotAPI)

find_package(nlohmann_json REQUIRED)
find_package(lua REQUIRED)

############################################################################
############################################################################
############################################################################
set(Header_FilesLua
    "Source/PolyglotAPI/Lua/LuaEngine.h"
    "Source/PolyglotAPI/Lua/Conversion.h"
    "Source/PolyglotAPI/Lua/FunctionRelay.h"
)
source_group("Header Files/Lua" FILES ${Header_FilesLua})

set(Source_FilesLua
    "Source/PolyglotAPI/Lua/LuaEngine.cpp"
    "Source/PolyglotAPI/Lua/Conversion.cpp"
    "Source/PolyglotAPI/Lua/FunctionRelay.cpp"
)
source_group("Source Files/Lua" FILES ${Source_FilesLua})

set(Header_FilesPy
    "Source/PolyglotAPI/Python/PythonEngine.h"
    "Source/PolyglotAPI/Python/FunctionRelay.h"
    "Source/PolyglotAPI/Python/Conversion.h"
)
source_group("Header Files/Python" FILES ${Header_FilesPy})

set(Source_FilesPy
    "Source/PolyglotAPI/Python/PythonEngine.cpp"
    "Source/PolyglotAPI/Python/FunctionRelay.cpp"
    "Source/PolyglotAPI/Python/Conversion.cpp"
)
source_group("Source Files/Python" FILES ${Source_FilesPy})

set(Header_FilesAPI
    "Source/PolyglotAPI/API/API.h"
    "Source/PolyglotAPI/API/APIFunction.h"
    "Source/PolyglotAPI/API/FunctionRelay.h"
    "Source/PolyglotAPI/API/TestFunctionRelay.h"
)
source_group("Header Files/API" FILES ${Header_FilesAPI})

set(Source_FilesAPI
    "Source/PolyglotAPI/API/API.cpp"
    "Source/PolyglotAPI/API/APIFunction.cpp"
    "Source/PolyglotAPI/API/FunctionRelay.cpp"
    "Source/PolyglotAPI/API/TestFunctionRelay.cpp"
)
source_group("Source Files/API" FILES ${Source_FilesAPI})


set(ALL_FILES
    ${Header_FilesPy}
    ${Source_FilesPy}
    ${Header_FilesLua}
    ${Source_FilesLua}
    ${Header_FilesAPI}
    ${Source_FilesAPI}
    ${lib}
)

############################################################################
############################################################################
############################################################################


add_library(PolyglotAPI SHARED ${ALL_FILES})
set_property(TARGET PolyglotAPI PROPERTY CXX_STANDARD 20)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

find_library(pythonlibrary
    NAMES python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}
    HINTS "${CMAKE_PREFIX_PATH}/lib"
    REQUIRED
)

find_library(pythonlibraryDebug
    NAMES python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}_d
    HINTS "${CMAKE_PREFIX_PATH}/lib"
    REQUIRED
)
target_include_directories(PolyglotAPI SYSTEM PUBLIC 
${Python3_INCLUDE_DIRS}
)
target_include_directories(PolyglotAPI SYSTEM PUBLIC 
"Source/PolyglotAPI"
)

target_link_libraries(PolyglotAPI PUBLIC
debug ${pythonlibraryDebug}
optimized ${pythonlibrary}
)
target_include_directories(PolyglotAPI SYSTEM PUBLIC 
"${CMAKE_CURRENT_LIST_DIR}/Source/"
)

find_package(pybind11 CONFIG REQUIRED)

target_link_libraries(PolyglotAPI PRIVATE
nlohmann_json::nlohmann_json 
lua
)
target_link_libraries(PolyglotAPI PUBLIC
pybind11::headers
)

set_target_properties(PolyglotAPI PROPERTIES FOLDER "PolyglotAPI")
set_target_properties(PolyglotAPI
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/$<CONFIG>
)