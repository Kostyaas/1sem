#pragma once
#include <iostream>
#include <vector>
#include <map>

enum class Direction {
    up,
    right
};

class Ship {
public:
    // Constructor initializes all members
    explicit Ship(uint16_t size, uint64_t x0, uint64_t y0, Direction dir);
    ~Ship() = default;
    bool damage(uint64_t x, uint64_t y) ;
    bool isSunk();

    bool isOutOfBounds(uint64_t board_width, uint64_t board_height) const ;

    // Проверка на пересечение с другим кораблем
    bool overlapsWith(const Ship& other) const ;
    std::vector<uint16_t> ship;   // Size of the ship (e.g., number of cells)
    uint16_t size;                // Vector representing the ship
    uint64_t x0;                  // Starting x-coordinate
    uint64_t y0;                  // Starting y-coordinate
    Direction dir;                // Direction of the ship (0 for horizontal, 1 for vertical, etc.)
};