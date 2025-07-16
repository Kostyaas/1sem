cmake_minimum_required(VERSION 3.14)

add_library(lexical_analyser STATIC
        Token.h
        LexicalAnalyser.h
        LexicalAnalyser.cpp
)

target_include_directories(lexical_analyser PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)