#pragma once
#include <vector>
#include<iostream>
#include <sstream>
#include "ship.h"

class Player {
public:
    explicit Player(std::string& name);
    Player() {}
    virtual ~Player() = default;
    Player* operator<<(std::vector<Ship>&& arr);
    bool dropped_out_player();
    void shoot(std::string& s);

     std::vector<Ship>& Get_ship(){
        return ships_player_;
    }
    virtual std::string getType() const {
        return "Player";  // Тип по умолчанию
    }
    std::vector<uint16_t> Get_size_ship();
    std::map<std::pair<uint64_t,uint64_t>, uint8_t> hits;
    std::string name;                   //порядковый номер игрока
    uint64_t score ;                 //кол-во очков игрока
private:
    std::vector<Ship> ships_player_;    //массив кораблей игрока
};

class Bot : public Player{
public:
    Bot(std::string& name): Player(name){}
    std::pair<uint64_t ,uint64_t> bot_shoot_standart(uint64_t board_width, uint64_t board_height);
    std::pair<uint64_t ,uint64_t> bot_shoot_hard(std::vector<uint16_t>&ship, uint64_t board_width, uint64_t board_height);

    std::pair<uint64_t, uint64_t> bot_shoot(std::string& s,uint64_t size_x, uint64_t size_y,std::vector<uint16_t> ship);
     std::string getType() const override{
        return "bot";  // Тип по умолчанию
    }
private:
    std::pair<uint64_t, uint64_t> lastHit = {0, 0};
    std::vector<Ship> ships_player_;    //массив кораблей игрока
};