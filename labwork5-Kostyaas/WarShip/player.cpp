#include "player.h"
#include "../logic_game/game.h"

Player::Player(std::string& name): name(name), score(0){}

void Player::shoot(std::string& s){
    std::istringstream iss(s);
    std::string word;
    // Ищем слово "width" и извлекаем следующее число
    while (iss >> word) {
        if (word == "height") {
            // Следующее слово должно быть числом
            break;
        }
    }
}
std::vector<uint16_t> Player::Get_size_ship(){
    std::vector<uint16_t> res;
    for(auto&& a : ships_player_){
        res.push_back(a.size);
    }
    return res;
}

bool Player::dropped_out_player(){
    if(ships_player_.size() > 0)    return false;
    return true;
}

Player* Player::operator<<(std::vector<Ship>&& arr){      //добавление корабля
    ships_player_ = std::move(arr);
    return this;
}



