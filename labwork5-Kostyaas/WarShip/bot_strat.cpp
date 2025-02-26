#include "player.h"


void calculateProbabilities(const std::vector<uint16_t>& ships,const std::map<std::pair<uint64_t, uint64_t>, uint8_t>& hits,std::vector<std::vector<double>>& probabilities,uint64_t board_width, uint64_t board_height) {
    // Обнуление вероятностй

    // Увеличиваем вероятность для всех клеток, куда может поместиться корабль
    for (auto&& ship : ships) {
        for (uint64_t x = 0; x < board_width; ++x) {
            for (uint64_t y = 0; y < board_height; ++y) {
                double k = 1;
                // Проверяем, может ли корабль поместиться по горизонтали
                if (x + ship <= board_width) {
                    bool can_place = true;
                    for (uint64_t i = 0; i < ship; ++i) {
                        if(hits.find({x + i, y}) != hits.end() && hits.at({x + i, y}) == 2) k+=0.25;
                        else if (hits.find({x + i, y}) != hits.end() && hits.at({x + i, y}) >= 3) { // Проверяем промахи
                            can_place = false;
                            break;
                        }
                    }
                    if (can_place) {
                        for (uint64_t i = 0; i < ship; ++i) {
                            probabilities[y][x + i] += (1.0*k);
                        }
                    }
                }

                // Проверяем, может ли корабль поместиться по вертикали
                if (y + ship <= board_height) {
                    bool can_place = true;
                    for (uint64_t i = 0; i < ship; ++i) {
                        if(hits.find({x + i, y}) != hits.end() && hits.at({x + i, y}) == 2) k+=0.25;
                        else if (hits.find({x, y + i}) != hits.end() && hits.at({x, y + i}) >= 3) { // Проверяем промахи
                            can_place = false;
                            break;
                        }
                    }
                    if (can_place) {
                        for (uint64_t i = 0; i < ship; ++i) {
                            probabilities[y + i][x] += (1.0*k);
                        }
                    }
                }
            }
        }
    }
}


// Функция для выбора следующей цели по направлению корабля
std::pair<uint64_t, uint64_t> chooseNextTarget(const std::pair<uint64_t, uint64_t>& lastHit,
                                               const Direction dir) {
    if (dir == Direction::up) {
        return {lastHit.first, lastHit.second + 1}; // Стрельба вверх
    } else if (dir == Direction::right) {
        return {lastHit.first + 1, lastHit.second}; // Стрельба вправо
    }
    return lastHit; // Если направление не определено, ничего не меняем
}

// Стратегия стрельбы
std::pair<uint64_t, uint64_t> chooseTarget(const std::vector<std::vector<double>>& probabilities,const std::map<std::pair<uint64_t, uint64_t>, uint8_t>& hits,const std::pair<uint64_t, uint64_t>& lastHit,const Direction dir) {
    // Если был предыдущий выстрел, стреляем в соседние клетки по направлению
    if (lastHit != std::pair<uint64_t, uint64_t>{0, 0}) {
        std::pair<uint64_t, uint64_t> nextTarget = chooseNextTarget(lastHit, dir);
        if (hits.find(nextTarget) == hits.end()) {
            return nextTarget; // Возвращаем следующую клетку, если она еще не была стреляна
        }
    }

    // Если нет попаданий, выбираем клетку с максимальной вероятностью
    std::pair<uint64_t, uint64_t> target = {0, 0};
    double max_probability = -1.0;
    uint64_t x_max = 0,y_max = 0;
    // Найти клетку с максимальной вероятностью
    for (uint64_t y = 0; y < probabilities.size(); ++y) {
        for (uint64_t x = 0; x < probabilities[y].size(); ++x) {
            if (hits.find({x, y}) == hits.end() && probabilities[y][x] > max_probability) {
                x_max = x;
                y_max = y;
                max_probability = probabilities[y][x];
            }
        }
    }
    target = {x_max, y_max};
    return target;
}

std::pair<uint64_t ,uint64_t> Bot::bot_shoot_hard(std::vector<uint16_t>&ship, uint64_t board_width, uint64_t board_height) {
    std::vector<std::vector<double>> probabilities(board_height, std::vector<double>(board_width, 0.0));
    calculateProbabilities(ship, hits, probabilities, board_width, board_height);

    std::pair<uint64_t, uint64_t> target = chooseTarget(probabilities, hits, lastHit, Direction::right);

    lastHit = target;

    return target;
}

std::pair<uint64_t, uint64_t> Bot::bot_shoot_standart(uint64_t size_x, uint64_t size_y) {
    // Переменные для отслеживания текущей позиции выстрела
    static uint64_t x = lastHit.first;
    static uint64_t y = lastHit.second;

    std::pair<uint64_t, uint64_t> target = {x, y};
    ++x;


    if (x >= size_x) {
        x = 0;
        y++;
    }
    if (y >= size_y) {
        y = 0;
    }
    lastHit = {x,y};
    return target;
}


std::pair<uint64_t, uint64_t> Bot::bot_shoot(std::string& s,uint64_t size_x, uint64_t size_y,std::vector<uint16_t> ship){
    if(s == "ordered"){
        std::cout << "1";
        return bot_shoot_standart(size_x,size_y);
    }
    else{
        std::cout << "2!!!!!!!!!!";
        return bot_shoot_hard(ship,size_x,size_y);
    }
}
