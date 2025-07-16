cmake_minimum_required(VERSION 3.14)

add_library(utils STATIC
        ParseExpression.h
)

target_link_libraries(utils PUBLIC
        interpreter
)

target_include_directories(utils PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)