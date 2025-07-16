cmake_minimum_required(VERSION 3.14)

add_library(semantic_analyser STATIC
        SemanticAnalyser.h
        SemanticAnalyser.cpp
)

target_link_libraries(semantic_analyser PUBLIC
        syntactic_analyser
)

target_include_directories(semantic_analyser PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)