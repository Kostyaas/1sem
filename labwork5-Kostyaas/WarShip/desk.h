#pragma once
#include "player.h"
#include "ship.h"



class Desk {
public:
    explicit Desk(uint64_t height, uint64_t width);

    void operator = (Player* player);
    bool shoot(uint64_t x, uint64_t y, Player* player);
    friend std::ostream& operator<<(std::ostream& os, const Desk& desk);
    Player* player_;
    uint64_t get_width() const;
    uint64_t get_height() const;
private:
    uint64_t width;
    uint64_t height;
    std::vector<std::vector<uint8_t>>board;
};
