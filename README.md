# PolyglotAPI
Single API for Python &amp; Lua scripting

# CMAKE lines

Put these lines in your main project.

Add/Remove packages as you like.

```
target_link_libraries(ProjectName PolyglotAPI)

get_filename_component(pyDir "${Python3_EXECUTABLE}" DIRECTORY )

# .\python -m ensurepip
execute_process(COMMAND ${Python3_EXECUTABLE} -m ensurepip)
#.\scripts\pip install scipy
execute_process(COMMAND "${pyDir}/scripts/pip3" install scipy)

add_custom_command(TARGET ProjectName POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_directory 
               ${pyDir}              
                $<TARGET_FILE_DIR:ProjectName>/Data/python)
```

Example how to do it: https://github.com/Liech/Farfalle
