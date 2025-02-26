#include "ArgParser.h"

using namespace ArgumentParser;
template <typename T>
Argument& ArgParser::AddIntArgument(const T short_name, const std::string& long_name, const std::string& diff) {
    std::string key;

    // Обработка для char
    if constexpr (std::is_same_v<T, char>) {
        key = std::string(1, short_name);  // Преобразуем char в std::string
    }
        // Обработка для std::string и const char*
    else if constexpr (std::is_same_v<T, std::string>) {
        key = short_name;  // Строки могут быть переданы как std::string или const char*
    }
    else if (std::is_same_v<T, const char*>){
        key = std::string(short_name);
    }

    bool is_single_char = key.length() == 1;  // Проверка на одиночный символ
    arguments[key] = Argument(ArgumentType::Int);

    if (!long_name.empty() && is_single_char) {
        long_to_short_name[long_name] = key;
    }
    return arguments[key];
}
template <typename T>
Argument& ArgParser::AddStringArgument(const T short_name, const std::string &long_name, const std::string &diff) {
    std::string key;

    // Обработка для char
    if constexpr (std::is_same_v<T, char>) {
        key = std::string(1, short_name);  // Преобразуем char в std::string
    }
        // Обработка для std::string и const char*
    else if constexpr (std::is_same_v<T, std::string>) {
        key = short_name;  // Строки могут быть переданы как std::string или const char*
    }
    else if (std::is_same_v<T, const char*>){
        key = std::string(short_name);
    }

    bool is_single_char = key.length() == 1;  // Проверка на одиночный символ
    arguments[key] = Argument(ArgumentType::String);

    if (!long_name.empty() && is_single_char) {
        long_to_short_name[long_name] = key;
    }
    return arguments[key];
}
template <typename T>
Argument& ArgParser::AddFlag(const T short_name, const std::string &long_name, const std::string &diff) {
    std::string key;

    // Обработка для char
    if constexpr (std::is_same_v<T, char>) {
        key = std::string(1, short_name);  // Преобразуем char в std::string
    }
        // Обработка для std::string и const char*
    else if constexpr (std::is_same_v<T, std::string>) {
        key = short_name;  // Строки могут быть переданы как std::string или const char*
    }
    else if (std::is_same_v<T, const char*>){
        key = std::string(short_name);
    }

    bool is_single_char = key.length() == 1;  // Проверка на одиночный символ
    arguments[key] = Argument(ArgumentType::Flag);

    if (!long_name.empty() && is_single_char) {
        long_to_short_name[long_name] = key;
    }
    return arguments[key];
}

Argument& ArgParser::AddHelp(const std::string &long_name, const std::string &diff) {
    arguments[long_name] = Argument(ArgumentType::Help);
    return arguments[long_name];
}

Argument& ArgParser::AddHelp(const char short_name, const std::string &long_name, const std::string &diff) {
    std::string key;

    key = std::string(1, short_name);  // Преобразуем char в std::string
    AddHelp(key, diff);
    if (!long_name.empty()) {
        long_to_short_name[long_name] = short_name;
    }
    return arguments[key];
}
template Argument& ArgParser::AddIntArgument<char>(const char short_name, const std::string& long_name, const std::string& diff);
template Argument& ArgParser::AddIntArgument<const char*>(const char* short_name, const std::string& long_name, const std::string& diff);
template Argument& ArgParser::AddStringArgument<char>(const char short_name, const std::string& long_name, const std::string& diff);
template Argument& ArgParser::AddStringArgument<const char*>(const char* short_name, const std::string& long_name, const std::string& diff);
template Argument& ArgParser::AddFlag<char>(const char short_name, const std::string& long_name, const std::string& diff);
template Argument& ArgParser::AddFlag<const char*>(const char* short_name, const std::string& long_name, const std::string& diff);
template Argument& ArgParser::AddHelp<char>(char short_name, const std::string &long_name, const std::string &diff);
template Argument& ArgParser::AddHelp<const char*>(const char* short_name, const std::string& long_name, const std::string& diff);

ArgParser::ArgParser(std::string name): name(std::move(name)){}



std::string ArgParser::GetStringValue(std::string name_argumnent){
    return arguments[(long_to_short_name.find(name_argumnent) != long_to_short_name.end()) ? long_to_short_name[name_argumnent] : name_argumnent].string_argument;
}

int ArgParser::GetIntValue(std::string name_argumnent, int index){
    return arguments[(long_to_short_name.find(name_argumnent) != long_to_short_name.end()) ? long_to_short_name[name_argumnent] : name_argumnent].arr_int[index];
}

bool ArgParser::GetFlag(std::string name_argumnent){
    return arguments[(long_to_short_name.find(name_argumnent) != long_to_short_name.end()) ? long_to_short_name[name_argumnent] : name_argumnent].flag;
}

bool ArgParser::Help(){
    for(auto arg : arguments){
        if(arg.second.type == ArgumentType::Help && arg.second.help == true){
            return true;
        }
    }
    return false;
}

void ArgParser::add_int_argument(Argument& arg, const std::string& value) {
    int val = std::stoi(value);
    arg.arr_int.push_back(val);

    if (arg.save_is_arr != nullptr) {
        arg.save_is_arr->push_back(val);
    }
}

void ArgParser::add_string_argument(Argument& arg, std::string &value) {
    arg.string_argument = value;
    if(arg.save_is_string != nullptr) *arg.save_is_string = value;
}

void ArgParser::add_flag_argument(Argument& arg) {
    arg.flag = true;

    if (arg.save_is_flag) {
        *arg.save_is_flag = true;
    }

}

void ArgParser::add_help_argument(Argument& arg) {
    arg.help = true;
}

bool ArgParser::IsNormal(){
    for(auto& pair : arguments){
        Argument& arg = pair.second;
        if(arg.type == ArgumentType::Int && arg.is_multi_value && arg.arr_int.size() < arg.multi_value){
            return false;
        }
        else if(arg.type == ArgumentType::String && arg.string_argument == ""){
            return false;
        }
    }
    return true;
}

Argument& ArgParser::name_arg_pos(){
    for(auto& pair : arguments){
        Argument& arg = pair.second;
        if(arg.type == ArgumentType::Int && arg.positional){
            return arg;
        }
    }
}

 bool ArgParser::Parse(const std::vector<std::string>& args) {
    for (int i = 1; i < args.size();++i){
        auto & arg = args[i];
        std::string arg_name;
        bool is_long = arg[1] == '-';
        size_t eq_pos = arg.find('=');
        if (arg[0] == '-') {
            size_t start_pos = is_long ? 2 : 1;  // Для длинных аргументов начинаем с 2, для коротких с 1
            arg_name = arg.substr(start_pos, (eq_pos != std::string::npos) ? eq_pos - start_pos : std::string::npos);
        }
        // когда -ac
        if(!is_long && arg_name.length() > 1){
            for(auto & short_arg : arg_name){
                std::string temp = std::string(1, short_arg);
                Argument& argument = arguments[(long_to_short_name.find(temp) != long_to_short_name.end()) ? long_to_short_name[temp] : temp];
                if(argument.type == ArgumentType::Flag){
                    add_flag_argument(argument);
                }
                else if(argument.type == ArgumentType::Help){
                    add_help_argument(argument);
                    return true;
                }

            }
        }
        else if(arg[0] == '-'){
            Argument& argument = arguments[(long_to_short_name.find(arg_name) != long_to_short_name.end()) ? long_to_short_name[arg_name] : arg_name];
            if(argument.type == ArgumentType::Flag)  add_flag_argument(argument);
            else if(argument.type == ArgumentType::Help) {
                add_help_argument(argument);
                return true;
            }
            else if(argument.type == ArgumentType::String){
                std::string arg_value = (eq_pos != std::string::npos) ? arg.substr(eq_pos + 1) :  args[++i];
                add_string_argument(argument, arg_value);
            }
             else if (argument.type == ArgumentType::Int) {
                 std::string arg_value = (eq_pos != std::string::npos) ? arg.substr(eq_pos + 1) :  args[++i];
                add_int_argument(argument, arg_value);

            }
        }
        else{
                Argument& argument = name_arg_pos();
                while (i< args.size() && args[i][0] != '-' ) {  // Убедитесь, что i + 1 существует и не за пределами массива
                    add_int_argument(argument, args[i++]);
                }
                --i;
            }
    }
    return IsNormal();
}











