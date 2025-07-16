cmake_minimum_required(VERSION 3.14)

add_library(syntactic_analyser STATIC
        SyntacticAnalyser.h
        SyntacticAnalyser.cpp
)

target_link_libraries(syntactic_analyser PUBLIC
        lexical_analyser
)

target_include_directories(syntactic_analyser PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)