#include "ship.h"

 Ship::Ship(uint16_t size, uint64_t x0, uint64_t y0, Direction dir)
    : size(size), x0(x0), y0(y0), dir(dir), ship(size, 1) // Initialize ship with 'size' elements, default value 0
{}

bool Ship::isOutOfBounds(uint64_t board_width, uint64_t board_height) const {

    if (dir == Direction::up) return y0 + size >= board_height;  // Проверка вертикально
    // Direction::right
    return x0 + size > board_width;  // Проверка горизонтально
}
bool Ship::overlapsWith(const Ship& other) const {
    // Определяем границы текущего корабля
    uint64_t this_x_start = x0;
    uint64_t this_x_end = (dir == Direction::right) ? (x0 + size - 1) : x0;
    uint64_t this_y_start = y0;
    uint64_t this_y_end = (dir == Direction::up) ? (y0 + size - 1) : y0;

    // Определяем границы другого корабля
    uint64_t other_x_start = other.x0;
    uint64_t other_x_end = (other.dir == Direction::right) ? (other.x0 + other.size - 1) : other.x0;
    uint64_t other_y_start = other.y0;
    uint64_t other_y_end = (other.dir == Direction::up) ? (other.y0 + other.size - 1) : other.y0;

    // Проверка на перекрытие или касание по обеим координатам
    bool overlap_x = (this_x_start <= other_x_end + 1 && this_x_end + 1 >= other_x_start);
    bool overlap_y = (this_y_start <= other_y_end + 1 && this_y_end + 1 >= other_y_start);

    return overlap_x && overlap_y;
}
bool Ship::isSunk(){
    for (auto&& part : ship) {
        if (part == 1) {  // Если хотя бы одна часть не повреждена, корабль не потоплен
            return false;
        }
    }
    return true;  // Все части повреждены, корабль потоплен
}

bool Ship::damage(uint64_t x, uint64_t y) {
    // Вычисляем индекс части корабля на основе координат
    if (dir == Direction::up) {
        if (y >= y0 && y < y0 + size && x == x0) {
            std::cout << "ship:" << y - y0 << std::endl;
            ship[y - y0] = 2;  // Повреждаем соответствующую часть
            return true;
        }
    } else if (dir == Direction::right) {
        if (x >= x0 && x < x0 + size && y == y0) {
            std::cout << "ship:" << x - x0 << std::endl;
            ship[x - x0] = 2;  // Повреждаем соответствующую часть
            return true;
        }
    }
    return false;
}