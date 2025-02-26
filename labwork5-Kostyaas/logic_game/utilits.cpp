#include "game.h"
template<typename t>
t getRandomNumber(t min, t max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<t> dist(min, max);
    return dist(gen);
}

std::vector<Ship> Rand_Pos_ships(std::vector<uint16_t>& arr, uint64_t n, uint64_t board_width, uint64_t board_height) {
    std::vector<Ship> ships;

    for (uint64_t i = 0; i < n; ++i) {
        uint16_t size = arr[i];
        Direction dir = (getRandomNumber(0, 1) == 0) ? Direction::up : Direction::right;
        uint64_t x0 = getRandomNumber<uint64_t>(0, board_width - 1);
        uint64_t y0 = getRandomNumber<uint64_t>(0, board_height - 1);

        Ship new_ship(size, x0, y0, dir);

        // Проверяем валидность нового корабля
        bool valid = true;

        if (new_ship.isOutOfBounds(board_width, board_height)) {
            valid = false; // Корабль выходит за границы
        } else {
            for (const auto& ship : ships) {
                if (new_ship.overlapsWith(ship)) {
                    valid = false; // Корабль пересекается с другим
                    break;
                }
            }
        }

        if (valid) {
            ships.push_back(new_ship);
        } else {
            --i; // Повторяем попытку для текущего корабля
        }
    }

    return ships;
}


void extract_names(const std::string& query, std::vector<Player*>& result) {
    // Ищем начало и конец скобок
    size_t start = query.find('[');
    size_t end = query.find(']');

    if (start != std::string::npos && end != std::string::npos) {
        // Извлекаем подстроку между скобками
        std::string names = query.substr(start + 1, end - start - 1);

        // Разбиваем на части по запятой
        std::stringstream ss(names);
        std::string name;
        uint8_t temp = 0;
        while (std::getline(ss, name, ',')) {
            if(temp == 1)   result.push_back(new Bot(name));  // Добавляем каждое имя в вектор
            if(temp == 0)   result.push_back(new Player(name));  // Добавляем каждое имя в вектор
            ++temp;
        }
    }
}
void extract_ships(const std::string& query, std::vector<uint16_t>& result, uint64_t& N) {
    // Ищем начало и конец скобок для извлечения кораблей
    size_t start = query.find('[');
    size_t end = query.find(']');

    if (start != std::string::npos && end != std::string::npos) {
        // Извлекаем подстроку между скобками
        std::string sizes = query.substr(start + 1, end - start - 1);

        // Разбиваем на части по запятой
        std::stringstream ss(sizes);
        uint64_t size;
        while (ss >> size) {  // Извлекаем размеры кораблей
            result.push_back(size);
            if (ss.peek() == ',') ss.ignore();  // Пропускаем запятую
        }
    }

    // Извлекаем значение N, которое должно быть после скобок
    size_t pos = query.find("]") + 1;
    while (pos < query.size() && std::isspace(query[pos])) {
        ++pos;  // Пропускаем пробелы после скобки
    }

    // Читаем число после пробела, которое и есть N
    if (pos < query.size()) {
        std::string N_str;
        while (pos < query.size() && std::isdigit(query[pos])) {
            N_str.push_back(query[pos]);
            ++pos;
        }
        if (!N_str.empty()) {
            N = std::stoull(N_str);  // Преобразуем строку в число
        }
    }
}
void dump_path(const std::string& s, Desk& desk){
    std::ofstream file(s);
    if(!file){
        std::cerr << "File dont open\n";
    }
    file << desk.get_width() << " " << desk.get_height() << "\n";
    for(auto&& ship : desk.player_->Get_ship()){
        std::string vec = ship.dir == Direction::up ? "h" : "v";
        file << ship.size << " " << vec << " " << ship.x0 << " " << ship.y0 << "\n";
    }
    file.close();
}
void load_path(std::string&& s, std::vector<Player*>& players, uint64_t& x, uint64_t& y){
    std::ifstream file(s);
    if(!file) {
        std::cerr << "File dont open\n";
        return;
    }
    file >> x >> y;
    uint16_t size;                // Vector representing the ship
    uint64_t x0;                  // Starting x-coordinate
    uint64_t y0;                  // Starting y-coordinate
    std::string dir_str;
    std::vector<Ship>ships;
    // Чтение данных о кораблях
    while (file >> size >> dir_str >> x0 >> y0) {
        // Создаем объект Ship и добавляем его в вектор
        ships.push_back(Ship(size,x0, y0, dir_str == "h" ? Direction::up : Direction::right));
    }
    for (auto&& player : players) {
        *player << std::move(ships);
    }
    file.close();
}
