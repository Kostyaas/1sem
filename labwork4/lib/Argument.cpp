#include "ArgParser.h"
using namespace ArgumentParser;
Argument& Argument::StoreValue(std::string& save_in_string){
    save_is_string = &save_in_string;
    return *this;
}
Argument& Argument::StoreValue(bool& flag_value){
    save_is_flag = &flag_value;
    return *this;
}
Argument& Argument::MultiValue(int value) {
    multi_value = value;
    is_multi_value = (multi_value > 0);
    return *this;
}

Argument& Argument::Positional(){
    positional = true;
    return *this;
}

Argument& Argument::StoreValues(std::vector<int>& arr){
    save_is_arr = &arr;
    return *this;
}
template <typename T>
Argument& Argument::Default(T a){
    if constexpr (std::is_same_v<std::decay_t<T>, const char *>){
        string_argument = a;
    } else if constexpr (std::is_same_v<std::decay_t<T>, int>) {
        arr_int.push_back(a);
    } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
        flag = a;
    }
    return *this;
}
template Argument& Argument::Default<int>(int a);
template Argument& Argument::Default<const char *>(const char* a);
template Argument& Argument::Default<bool>(bool a);
