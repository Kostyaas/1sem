cmake_minimum_required(VERSION 3.29)
project(labwork6_Kostyaas)

set(CMAKE_CXX_STANDARD 26)

add_executable(labwork6_Kostyaas
        src/main.cpp
        helpers/constants.h
        src/thread.h
        src/thread.cpp
        utitlits/utilits.h
        utitlits/utilits.cpp)

find_package(jsoncpp CONFIG REQUIRED)
find_package(cpr REQUIRED)

target_link_libraries(labwork6_Kostyaas
        PRIVATE JsonCpp::JsonCpp
        PRIVATE cpr::cpr
)

