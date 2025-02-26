#include "game.h"

void print_desk(Desk& d){
    system("clear");
    std::cout << d.player_->name << " - " << d.player_->getType() << "score:" <<d.player_->score << std::endl;
    std::cout << d;
}

void process_shot(std::vector<Desk>& desks, uint64_t i, uint64_t size, uint64_t x, uint64_t y) {
    bool shot_successful = desks[(i + 1) % size].shoot(x, y, desks[i].player_);

    while (shot_successful) {
        if (desks[(i + 1) % size].player_->dropped_out_player()) {
            std::cout << "Player " << desks[(i + 1) % size].player_->name << " dropped out!" << std::endl;
            desks.erase(desks.begin() + (i + 1) % size);
            --size;
            break;
        }
        std::string s;
        std::getline(std::cin, s);
        if (s.find("shot") != std::string::npos) {
            std::stringstream ss(s);
            std::string temp;
            ss >> temp >> x >> y;
            shot_successful = desks[(i + 1) % size].shoot(x, y, desks[i].player_);  // Пробуем снова
        } else {
            std::cout << "Invalid command, please enter 'shot' followed by coordinates." << std::endl;
        }
    }
}

void process_bot_shot(std::vector<Desk>& desks, uint64_t i, uint64_t size,std::string& s) {
    Bot* bot = dynamic_cast<Bot*>(desks[i].player_);
    std::pair<uint64_t,uint64_t>shot = bot->bot_shoot(s, desks[0].get_width(), desks[0].get_height(), desks[(i + 1) % size].player_->Get_size_ship());
    bool shot_successful = desks[(i + 1) % size].shoot(shot.first, shot.second, desks[i].player_);
    while(shot_successful){
        if (desks[(i + 1) % size].player_->dropped_out_player()) {
            std::cout << "Player " << desks[(i + 1) % size].player_->name << " dropped out!" << std::endl;
            desks.erase(desks.begin() + (i + 1) % size);
            --size;
            break;
        }
        shot = bot->bot_shoot(s, desks[0].get_width(), desks[0].get_height(), desks[(i + 1) % size].player_->Get_size_ship());
        shot_successful = desks[(i + 1) % size].shoot(shot.first, shot.second, desks[i].player_);
    }
    std::string s1;
    std::getline(std::cin, s1);  // Вводим команду
    while(s1.find("contine") == std::string::npos){
        std::getline(std::cin, s1);
    }
}


void start(std::vector<Desk>& desks,std::string& s) {
    uint64_t size = desks.size();
    uint64_t i = 0;
    while (desks.size() > 1) {
        uint64_t x,y;
        if (desks[i].player_->getType() == "Player") {
            print_desk(desks[i]);
            std::string s;
            std::getline(std::cin, s);  // Вводим команду
             if(s.find("dump") != std::string::npos){
                 dump_path(s.substr(s.find(" ") + 1), desks[i]);
            }
            else if (s.find("shot") != std::string::npos) {
                std::stringstream ss(s);
                std::string temp;
                ss >> temp >> x >> y;
                process_shot(desks, i, size, x, y);
                ++i;
            }
        }
        else if (desks[i].player_->getType() == "bot") {
            process_bot_shot(desks, i, size,s);
            ++i;
        }

        i %= size;
    }

    // Конец игры
    std::cout << "Игра закончена, победил - " << desks.begin()->player_->name << std::endl;
}

