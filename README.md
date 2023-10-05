# PolyglotAPI
Single API for Python &amp; Lua scripting

# CMAKE lines

get_filename_component(pyDir "${Python3_EXECUTABLE}" DIRECTORY )

add_custom_command(TARGET ProjectName POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_directory 
               ${pyDir}              
                $<TARGET_FILE_DIR:ProjectName>/Data/python)
