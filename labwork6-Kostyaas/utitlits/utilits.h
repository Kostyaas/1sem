#pragma once

#include <iostream>
#include <json/json.h>
#include <cpr/cpr.h>
#include "../helpers/constants.h"
#include <vector>

struct Parametr {
    Parametr();

    Parametr(const std::string &http_req, const std::vector<std::pair<std::string,std::string>> &parametrs) : http_req(http_req),
                                                                                 parametrs(parametrs) {}

    std::string Get_Url() const;

    std::string http_req;
    std::vector<std::pair<std::string,std::string>> parametrs;
};
Json::Value Get_Json_Url(const Parametr &parametr);

Json::Value Combine_Json(const Json::Value& json1,const Json::Value& json2);



