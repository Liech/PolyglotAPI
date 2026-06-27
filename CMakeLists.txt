cmake_minimum_required(VERSION 3.24...4.0.2)

project(PolyglotAPI)

option(POLYGLOT_ENABLE_PYTHON "Polyglot Python" ON)
option(POLYGLOT_ENABLE_LUA "Polyglot Lua" ON)
set(POLYGLOT_PYTHON_EXTENSIONS "numpy" CACHE STRING "Space-separated list of pip packages")
set(POLYGLOT_LUA_EXTENSIONS "LuaFileSystem" CACHE STRING "Space-separated list of luarocks packages")

############################################################################

find_package(nlohmann_json REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

if(POLYGLOT_ENABLE_LUA)
  find_package(Lua REQUIRED)
endif()

if(POLYGLOT_ENABLE_PYTHON)
  find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
  find_package(pybind11 CONFIG REQUIRED)
endif()

############################################################################
############################################################################
############################################################################

set(Lua_Files
    "Source/PolyglotAPI/Lua/LuaEngine.h"
    "Source/PolyglotAPI/Lua/LuaEngine.cpp"
    "Source/PolyglotAPI/Lua/Conversion.h"
    "Source/PolyglotAPI/Lua/Conversion.cpp"
)
source_group("Lua" FILES ${Lua_Files})

set(Python_Files
    "Source/PolyglotAPI/Python/PythonEngine.h"
    "Source/PolyglotAPI/Python/PythonEngine.cpp"
    "Source/PolyglotAPI/Python/Conversion.h"
    "Source/PolyglotAPI/Python/Conversion.cpp"
)
source_group("Python" FILES ${Python_Files})

set(main_Files
    "Source/PolyglotAPI/Node.cpp"
    "Source/PolyglotAPI/testmain.cpp"
    "Source/PolyglotAPI/Node.h"
)
source_group("" FILES ${main_Files})

set(ALL_FILES
    ${main_Files}
)

if(POLYGLOT_ENABLE_LUA)
    list(APPEND ALL_FILES ${Lua_Files})
endif()

if(POLYGLOT_ENABLE_PYTHON)
    list(APPEND ALL_FILES ${Python_Files})
endif()

############################################################################
############################################################################
############################################################################

add_library(PolyglotDependencies INTERFACE)

add_library(PolyglotAPI SHARED ${ALL_FILES})
add_executable(PolyglotAPITest ${ALL_FILES})
target_compile_definitions(PolyglotAPITest PRIVATE -DISTESTPROJECT=1)

set_property(TARGET PolyglotAPI PROPERTY CXX_STANDARD 23)
set_property(TARGET PolyglotAPITest PROPERTY CXX_STANDARD 23)

target_link_libraries(PolyglotDependencies INTERFACE
    nlohmann_json::nlohmann_json
)
target_include_directories(PolyglotDependencies INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)
if(POLYGLOT_ENABLE_LUA)
    target_link_libraries(PolyglotDependencies INTERFACE ${LUA_LIBRARIES})
    target_include_directories(PolyglotDependencies INTERFACE ${LUA_INCLUDE_DIR})
endif()
if(POLYGLOT_ENABLE_PYTHON)
    target_link_libraries(PolyglotDependencies INTERFACE
        Python3::Python
        pybind11::module
    )
    target_include_directories(PolyglotDependencies INTERFACE ${Python3_INCLUDE_DIRS})
endif()

target_link_libraries(PolyglotAPI PUBLIC PolyglotDependencies)
target_link_libraries(PolyglotAPITest PRIVATE PolyglotDependencies Catch2::Catch2WithMain)

set_target_properties(PolyglotAPITest PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Test/$<CONFIG>"
)
set_target_properties(PolyglotAPI PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)
target_compile_definitions(PolyglotAPITest PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)
target_compile_definitions(PolyglotAPI PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)
############################################################################
if(POLYGLOT_ENABLE_PYTHON)
    set(PYTHON_EXT_ARGS ${POLYGLOT_PYTHON_EXTENSIONS})
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    get_filename_component(PYTHON_ROOT "${Python3_EXECUTABLE}" DIRECTORY)

    add_custom_command(
        TARGET PolyglotAPI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env PYTHON_ROOT="${PYTHON_ROOT}" 
                ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sync_python.py" 
                "${PYTHON_ROOT}" "$<TARGET_FILE_DIR:PolyglotAPI>/python" ${PYTHON_EXT_ARGS}
        COMMENT "Syncing Python environment and installing extensions"
        VERBATIM
    )
    add_custom_command(
        TARGET PolyglotAPITest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env PYTHON_ROOT="${PYTHON_ROOT}" 
                ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sync_python.py" 
                "${PYTHON_ROOT}" "$<TARGET_FILE_DIR:PolyglotAPITest>/python" ${PYTHON_EXT_ARGS}
        COMMENT "Syncing Python environment and installing extensions"
        VERBATIM
    )
endif()