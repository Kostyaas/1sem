cmake_minimum_required(VERSION 3.14)

add_library(interpreter STATIC
        interpreter.h
        interpreter.cpp
)

target_link_libraries(interpreter PUBLIC
        semantic_analyser
        syntactic_analyser
        utils
)

target_include_directories(interpreter PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
