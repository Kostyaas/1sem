#include "utilits.h"

std::string Parametr::Get_Url() const {
    if (parametrs.empty()) return http_req;
    std::string url = http_req + "?";
    for (size_t i = 0; i < parametrs.size(); ++i) {
        url += parametrs[i].first + "=" + parametrs[i].second;
        if (i != parametrs.size() - 1) {
            url += "&";
        }
    }
    return url;
}

Json::Value Get_Json_Url(const Parametr& parametr) {
    std::string full_url =  parametr.Get_Url();
    std::cout << "Запрос: " << full_url << std::endl; // Для отладки
    cpr::Response response = cpr::Get(cpr::Url{full_url});
    if (response.status_code == 200) {
        Json::CharReaderBuilder reader;
        Json::Value jsonData;
        std::string errs;
        std::istringstream s(response.text);
        if (Json::parseFromStream(reader, s, &jsonData, &errs)) {
            return jsonData;
        } else {
            std::cerr << "Ошибка парсинга JSON: " << errs << std::endl;
        }
    } else {
        std::cerr << "Ошибка запроса. Код HTTP: " << response.status_code
                  << " Ответ: " << response.text << std::endl;
    }

    return Json::Value();
}


Json::Value Combine_Json(const Json::Value& json1,const Json::Value& json2){
    Json::Value res;
    for(const auto& item : json1){
        res.append(item);
    }

    for(const auto& item : json2){
        res.append(item);
    }

    return res;
}




