#pragma once

#include <string>
#include <vector>
#include <json/json.h>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#include "../utitlits/utilits.h"
#include "../helpers/constants.h"

class Transport {
public:
    Transport() = default;

    Transport(const Json::Value &json) {
        if (json.isMember("is_transfer") && json["is_transfer"].asBool()) {
            from_city_name = json["transfer_from"]["title"].asString();
            to_city_name = json["transfer_to"]["title"].asString();
            type = "Пересадка";
            uid = "";
        } else {
            time_departue = json["departure"].asString();
            time_arrival = json["arrival"].asString();
            from_city_name = json["from"]["title"].asString();
            to_city_name = json["to"]["title"].asString();
            type = json["thread"]["transport_type"].asString();
            uid = json["thread"]["number"].asString();
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const Transport &transport) {
        os << transport.from_city_name << " -- "
           << transport.type << " "
           << transport.uid << " --> "
           << transport.to_city_name
           << " (" << transport.time_arrival << " : " << transport.time_departue << ")";
        return os;
    }

    operator Json::Value() const {
        Json::Value result;
        result["time_arrival"] = time_arrival;
        result["time_departue"] = time_departue;
        result["type"] = type;
        result["uid"] = uid;
        result["from_city_name"] = from_city_name;
        result["to_city_name"] = to_city_name;
        return result;
    }

    std::string time_arrival;
    std::string time_departue;
    std::string type;
    std::string uid;
    std::string from_city_name;
    std::string to_city_name;
};

template<typename T>
class BaseParser {
public:
    BaseParser(const std::string &type = "") : type(type) {}

    virtual ~BaseParser() = default;

    const std::vector<T> &getData() const {
        return _data;
    }

    operator Json::Value() const {
        Json::Value result;

        // Обработка поля Threads
        Json::Value threadsArray;
        for (const auto &subobject: _data) {
            threadsArray.append(subobject);
        }
        if (!type.empty()) result[type] = threadsArray;
        else result = threadsArray;
        return result;
    }

    friend std::ostream &operator<<(std::ostream &os, const BaseParser<T> &object) {
        int index = 1;
        for (auto it = object.getData().begin(); it != object.getData().end(); ++it) {
            if (!object.type.empty()) {
                os << "[" << object.type << "]\t№" << index << ":\n";
            }

            os << "\t" << *it << "\n";

            ++index;
        }
        return os;
    }

    std::string type;
protected:
    std::vector<T> _data;

    virtual void parse(const Json::Value &jsonArray) {
        for (const auto &item: jsonArray) {
            _data.emplace_back(item);
        }
    }
};


class Thread : public BaseParser<Transport> {
public:
    Thread(const Json::Value &json) : BaseParser<Transport>(){
        parse(prepareJson(json));
    }

protected:
    void parse(const Json::Value &jsonArray) override {
        if (has_transe) {
            for (const auto &item: jsonArray) {
                _data.emplace_back(item);
                if (_data.back().type == "Пересадка") ++stops;
            }
        } else {
            _data.emplace_back(jsonArray);
        }
        if (stops > 2) _data.clear();
    }
private:
    bool has_transe = false;
    int stops = 0;
    Json::Value prepareJson(const Json::Value &json) {
        if (json["has_transfers"].asBool()) {
            has_transe = true;
            return json["details"];
        } else {
            has_transe = false;
            return json;
        }
    }
};

class Threads : public BaseParser<Thread> {
public:
    Threads(const Json::Value &json) : BaseParser<Thread>("Thread"), json(json) {
        parse(Combine_Json(json["segments"], json["interval_segments"]));
        to = json["search"]["to"]["code"].asString();
        date = json["search"]["date"].asString();
    }

    bool Save_File() {
        std::string name_file = myconst::directory + to + "_" + date + ".json"; // Полный путь к файлу
        if (std::filesystem::exists(name_file)) {
            return true;
        }
        std::ofstream file(name_file, std::ofstream::out);
        if (!file.is_open()) {
            return false;
        }
        file << json;
        file.close();
        std::cout << "Данные успешно записаны" << std::endl;
        return true;
    }

    std::string Get_Name() const {
        return to + "_" + date;
    }

private:
    Json::Value json;
    std::string to;
    std::string date;
};

class Run : public BaseParser<Threads> {
public:

    Run() : BaseParser<Threads>("Threads") {
        std::string out;
        while (std::getline(std::cin, out) && out != "z") {
            std::string to, date, lim;

            std::istringstream iss(out);
            std::string arg;
            while (iss >> arg) {
                if (arg == "-d") {
                    iss >> date;
                } else if (arg == "-t") {
                    iss >> to;
                } else if (arg == "l") {
                    iss >> lim;
                } else if (arg == "add") {
                    Add_base(to, date, lim);
                } else if (arg == "file") Add_file();
                else if (arg == "print") {
                    if (!_data.empty()) {
                        std::cout << *this;
                    } else {
                        std::cerr << "Невозможно вывести пустой массив" << std::endl;
                    }
                } else {
                    for (auto &arr: _data) {
                        arr.Save_File();
                    }
                }
            }
        }
    }


private:
    void Add_base(std::string &to, std::string &date, std::string lim = "7") {
        if (to.empty() || date.empty()) {
            std::cerr << "Ошибка: to или date пустые!" << std::endl;
            return;
        }

        to = Get_Json_Url(Parametr("https://suggests.rasp.yandex.net/all_suggests", {{"format", "old"},
                                                                                     {"part",   to}}))[1][0][0].asString();
        if (CheckRepeat(to + "_" + date)) {
            Parametr url("https://api.rasp.yandex.net/v3.0/search/",
                         {{"page",      "1"},
                          {"format",    myconst::format},
                          {"to",        to},
                          {"from",      myconst::from},
                          {"date",      date},
                          {"lang",      myconst::lang},
                          {"limit",     lim},
                          {"transfers", "true"},
                          {"apikey",    myconst::api_key}});
            Json::Value json = Get_Json_Url(url);
            _data.emplace_back(json);

            std::cout << "Маршрут успешно добавлен!" << std::endl;
        }
    }

    void Add_file() {
        if (!_data.empty()) {
            _data.back().Save_File();
        }
    }

    void readJsonFile(const std::string &fileName) {
        std::ifstream file(myconst::directory + fileName, std::ifstream::binary);
        Json::Value root;
        Json::CharReaderBuilder readerBuilder;
        std::string errs;

        if (!file.is_open()) {
            std::cerr << "Ошибка открытия файла!" << std::endl;
            return;
        }

        if (!Json::parseFromStream(readerBuilder, file, &root, &errs)) {
            std::cerr << "Ошибка при парсинге JSON: " << errs << std::endl;
        }
        _data.emplace_back(root);
    }

    bool CheckRepeat(const std::string &file) {
        if (std::filesystem::exists(myconst::directory + file + ".json")) {
            readJsonFile(file + ".json");
            return false;
        }
        for (auto &arr: _data) {
            if (arr.Get_Name() == file) {
                return false;
            }
        }
        return true;
    }

};
