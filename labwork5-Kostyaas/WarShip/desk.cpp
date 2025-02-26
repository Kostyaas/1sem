#include "desk.h"


char cellRepresentation(uint8_t cell) {
    switch (cell) {
        case 0: return '~'; // Вода
        case 2: return 'x'; // Попадание
        case 3: return 'o'; // Промах
        case 4: return '#'; // Заблокированная зона
        default: return '?'; // возможно кораблик
    }
}
Desk::Desk(uint64_t height, uint64_t width) : height(height),width(width) , board(height, std::vector<uint8_t>(width, 0)) {}

void app_block_sunkship(std::map<std::pair<uint64_t, uint64_t>, uint8_t>& hit, Ship& ship) {
    // Начальные координаты с учетом возможного уменьшения
    int64_t x0 = static_cast<int64_t>(ship.x0) - 1;
    int64_t y0 = static_cast<int64_t>(ship.y0) - 1;

    // Конечные координаты в зависимости от направления корабля
    int64_t size_x = (ship.dir == Direction::right) ? ship.x0 + ship.size : ship.x0 + 1;
    int64_t size_y = (ship.dir == Direction::up) ? ship.y0 + ship.size : ship.y0 + 1;


    // Блокируем клетки в hits
    for (int64_t x = x0; x <= size_x; ++x) {
        for (int64_t y = y0; y <= size_y; ++y) {
            auto it = hit.find({static_cast<uint64_t>(x), static_cast<uint64_t>(y)});
            if (x >= 0 && y >= 0 && it->second != 2) {
                hit[{x,y}] = 4;
            }
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Desk& desk) {
    for (size_t y = 0; y < desk.height; ++y) {
        for (size_t x = 0; x < desk.width; ++x) {
            os << cellRepresentation(desk.board[y][x]) << " ";
        }
        os << '\t';
        for (size_t x = 0; x < desk.width; ++x) {
            auto it = desk.player_->hits.find({x, y});
            os << cellRepresentation(it != desk.player_->hits.end() ? it->second : 0) << " ";
        }
        os << "\n";
    }
    return os;
}


bool Desk::shoot(uint64_t x, uint64_t y, Player* player) {
    if(player->hits.find({x,y}) != player->hits.end()){
        std::cout << "you've already shot here \n try again \n";
        return true;
    }
    if (x >= width || y >= height) {
        std::cerr << "Invalid coordinates" << x << " " << y << std::endl;
        return true;
    } else if (board[y][x] == 0) {
        std::cout << "Player:"<< player->name << " - Miss" << std::endl;
        player->hits[{x,y}] = 3;
        return false;
    }
    else {
        player->hits[{x,y}] = 2;
        for (auto it = player_->Get_ship().begin(); it != player_->Get_ship().end(); ++it) {
            if(it->damage(x, y)) {  // Повреждаем корабль
                if (it->isSunk()) {
                    std::cout << "Player " << player_->name << " - Ship sunk!" << std::endl;
                    app_block_sunkship(player->hits, *it);
                    player_->Get_ship().erase(it);
                    return true;
                }
                break;
            }
        }
        ++player->score;     // Увеличиваем счет игрока
        std::cout << "Player " << player->name << " - Hit" << std::endl;
        return true;
    }
}

void Desk::operator = (Player* player){  //добавление игроков
    player_ = player;
    for(auto& a : player->Get_ship() ){
        if(a.dir == Direction::up){
            for(int i = 0;i < a.size;++i){
                uint64_t y = a.y0 + i;
                uint64_t x = a.x0;
                board[y][x] = a.ship[i];
            }
        }
        else{
            for(int i = 0;i < a.size;++i){
                uint64_t y = a.y0;
                uint64_t x = a.x0 + i;
                board[y][x] = a.ship[i];
            }
        }
    }
}

uint64_t Desk::get_width() const {
    return width;
}

// Метод для получения высоты доски
uint64_t Desk::get_height() const {
    return height;
}