cmake_minimum_required(VERSION 3.24...4.0.2)

project(PolyglotAPI)

find_package(nlohmann_json REQUIRED)
find_package(Lua REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
find_package(pybind11 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

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
    ${Lua_Files}
    ${Python_Files}
    ${main_Files}
)

############################################################################
############################################################################
############################################################################

add_library(PolyglotDependencies INTERFACE)

add_library(PolyglotAPI SHARED ${ALL_FILES})
add_executable(PolyglotAPITest ${ALL_FILES})

set_property(TARGET PolyglotAPI PROPERTY CXX_STANDARD 23)
set_property(TARGET PolyglotAPITest PROPERTY CXX_STANDARD 23)


target_link_libraries(PolyglotDependencies INTERFACE
    nlohmann_json::nlohmann_json
    ${LUA_LIBRARIES}
    Python3::Python
    pybind11::module
)
target_link_libraries(PolyglotAPI PUBLIC PolyglotDependencies)
target_link_libraries(PolyglotAPITest PRIVATE PolyglotDependencies Catch2::Catch2WithMain)

target_include_directories(PolyglotDependencies INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    ${LUA_INCLUDE_DIR}
    ${Python3_INCLUDE_DIRS}
)

set_property( TARGET PolyglotAPITest PROPERTY COMPILE_DEFINITIONS ISTESTPROJECT=1 )



############################################################################

get_filename_component(PYTHON_ROOT "${Python3_EXECUTABLE}" DIRECTORY)
set(PY_DLL_NAME "python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}.dll")

add_custom_command(
    TARGET PolyglotAPITest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:PolyglotAPITest>/Lib"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:PolyglotAPITest>/DLLs"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PYTHON_ROOT}/${PY_DLL_NAME}" "$<TARGET_FILE_DIR:PolyglotAPITest>/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PYTHON_ROOT}/Lib" "$<TARGET_FILE_DIR:PolyglotAPITest>/Lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PYTHON_ROOT}/DLLs" "$<TARGET_FILE_DIR:PolyglotAPITest>/DLLs"
    COMMENT "Synch Python Runtime for PolyglotAPITest..."
    VERBATIM
)
add_custom_command(
    TARGET PolyglotAPI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:PolyglotAPI>/Lib"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:PolyglotAPI>/DLLs"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PYTHON_ROOT}/${PY_DLL_NAME}" "$<TARGET_FILE_DIR:PolyglotAPI>/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PYTHON_ROOT}/Lib" "$<TARGET_FILE_DIR:PolyglotAPI>/Lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PYTHON_ROOT}/DLLs" "$<TARGET_FILE_DIR:PolyglotAPI>/DLLs"
    COMMENT "Synch Python Runtime for PolyglotAPI..."
    VERBATIM
)