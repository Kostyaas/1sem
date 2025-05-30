#include <iostream>
#include <processing.h>
#include <algorithm>
#include <format>
/*
Программа считает частоту слов, встречающихся во всех файлах в директории
и выводит итоговую статистику в консоль
*/

int main(int argc, char **argv)
{
  Dir("/Users/kostakazackov/labs/labwork8-Kostyaas/work_temp", false) | Filter([](const std::filesystem::path &p){ return p.extension() == ".txt"; }) | OpenFiles() | Split("\n ,.;") | Transform(
        []( std::string token) {
            std::transform(token.begin(), token.end(), token.begin(), [](char c){return std::tolower(c);});
            return token;
        }) | AggregateByKey(
        [](size_t& count) { ++count;},
        [](const std::string& token) { return token;}) | Transform([](const std::pair<std::string, size_t>& stat) { return std::format("{} - {}", stat.first, stat.second);})| Out(std::cout);

  return 0;
}

