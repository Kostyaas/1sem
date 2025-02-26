#pragma once
#include<vector>
#include<iostream>
#include <random>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include "../WarShip/desk.h"

void load_path(std::string&& s, std::vector<Player*>& players, uint64_t& x, uint64_t& y);

template<typename t>
t getRandomNumber(t min, t max);

void dump_path(const std::string& s, Desk& desk);

std::vector<Ship> Rand_Pos_ships(std::vector<uint16_t>& arr, uint64_t n, uint64_t board_width, uint64_t board_height);

void extract_names(const std::string& query, std::vector<Player*>& result);

void extract_ships(const std::string& query, std::vector<uint16_t>& result, uint64_t& N);

void start(std::vector<Desk>&desks,std::string& s);


