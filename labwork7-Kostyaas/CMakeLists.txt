cmake_minimum_required(VERSION 3.12)

project(
    unrolled_list
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

include_directories(lib)

add_subdirectory(bin)


add_executable(L bin/main.cpp lib/unrolled_list.h lib/IteratorsULL.h lib/list.h lib/VectorIterators.h lib/IteratorHelp.h) 
set(CMAKE_TYPE_BUILD Debug)
# enable_testing()
# add_subdirectory(tests)
