#include "logic_game/game.h"
#include "WarShip/ship.h"
#include "WarShip/player.h"
#include "WarShip/desk.h"

int main() {
    std::string human_name = "Human";
    std::string bot_name = "Bot";
    std::string dif = "custom";
    std::vector<uint16_t> ships{1,2};
    std::vector<Player*>players{new Player(human_name), new Bot(bot_name)};
    uint64_t N = 2;
    uint64_t width = 6, height = 6;
    std::string s;
    while(getline(std::cin,s)){
        if(s == "ping") std::cout <<"pong"<< std::endl;
        else if(s == "exit"){
            std::cout << "ok" << std::endl;
            break;
        }
        else if(s.find("create") != std::string::npos){
            extract_names(s, players);
            std::cout << "ok" << std::endl;
        }
        else if(s.find("set width") != std::string::npos){
            std::istringstream iss(s);
            std::string word;
            // Ищем слово "width" и извлекаем следующее число
            while (iss >> word) {
                if (word == "width") {
                    // Следующее слово должно быть числом
                    iss >> width;
                    break;
                }
            }
            std::cout << "ok" << std::endl;
        }
        else if(s.find("set height") != std::string::npos){
            std::istringstream iss(s);
            std::string word;
            // Ищем слово "width" и извлекаем следующее число
            while (iss >> word) {
                if (word == "height") {
                    // Следующее слово должно быть числом
                    iss >> height;
                    break;
                }
            }
            std::cout << "ok" << std::endl;
        }
        else if(s == "set strategy [custom]"){
            std::cout << "ok\n";
            dif = "custom";
        }
        else if(s.find("set count") != std::string::npos){
            extract_ships(s, ships, N);
            for(auto&& player : players) {
                *player << Rand_Pos_ships(ships, N, width, height);
            }
            std::cout << "ok" << std::endl;
        }
        else if(s == "start"){
            std::vector<Desk>desks;
            for (auto&& player : players) {
                desks.emplace_back(height, width);
                desks.back() = player;
            }
            start(desks,dif);
        }
        else if(s.find("load") != std::string::npos){
            load_path(s.substr(s.find(" ") + 1), players, width, height);
        }
    }
    return 0;
}
