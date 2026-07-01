cmake_minimum_required(VERSION 3.16)
project(Assignment4 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cipher SHARED dll/cipher_api.h dll/cipher.cpp)
set_target_properties(cipher PROPERTIES PREFIX "")

add_executable(editor
    main.cpp
    Text.h Text.cpp
    Cipher.h Cipher.cpp
    CommandLineInterface.h CommandLineInterface.cpp
)

add_dependencies(editor cipher)


add_custom_command(TARGET editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:cipher>
        $<TARGET_FILE_DIR:editor>
)
