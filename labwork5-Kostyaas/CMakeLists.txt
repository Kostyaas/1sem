cmake_minimum_required(VERSION 3.29)
project(labwork5_Kostyaas)

set(CMAKE_CXX_STANDARD 23_)

add_executable(labwork5_Kostyaas
        main.cpp
        WarShip/ship.cpp
        WarShip/ship.h
        WarShip/player.cpp
        WarShip/player.h
        WarShip/desk.cpp
        WarShip/desk.h
        logic_game/game.cpp
        logic_game/game.h
        logic_game/utilits.cpp
        WarShip/bot_strat.cpp)