
        if(NOT EXISTS "${SENTINEL}")
            file(MAKE_DIRECTORY "")
            execute_process(COMMAND C:/Program Files/CMake/bin/cmake.exe -E copy_directory "C:/Users/nicol/Documents/GitHub/PolyglotAPI/out/vcpkg_installed/x64-windows/tools/python3" "")
            execute_process(COMMAND "/python.exe" -m ensurepip --default-pip)
            execute_process(COMMAND "/python.exe" -m pip install --upgrade pip)
            execute_process(COMMAND "/python.exe" -m pip install numpy)
            file(TOUCH "${SENTINEL}")
        endif()
    