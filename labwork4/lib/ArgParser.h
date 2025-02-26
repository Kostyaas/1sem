#pragma once
#include <string>
#include <map>
#include<vector>
#include <utility>
#include <iostream>
namespace ArgumentParser {
    enum class ArgumentType {
        String,
        Int,
        Flag,
        Help
    };
    class Argument {
    public:
        ArgumentType type;
        bool is_multi_value = false;
        int multi_value = 0;
        bool positional = false;

        std::vector<int>* save_is_arr = nullptr;
        std::string* save_is_string = nullptr;
        bool* save_is_flag = nullptr;

        std::vector<int>arr_int;
        std::string string_argument = "";

        bool flag = false;
        bool help = false;

        Argument(ArgumentType t) : type(t) {}
        Argument() {}

        Argument &MultiValue(int value = 0);
        template <typename T>
        Argument& Default(T a);
        Argument& Positional();
        Argument& StoreValues(std::vector<int> &arr);
        Argument& StoreValue(std::string& save_in_string);
        Argument& StoreValue(bool& flag_value);

    };
class ArgParser {
public:
    ArgParser(std::string name);
    template <typename T>
    Argument& AddIntArgument(const T short_name, const std::string& long_name = "", const std::string& diff = "");
    template <typename T>
    Argument& AddStringArgument(const T short_name ,const std::string& long_name = "",const std::string& diff = "");
    template <typename T>
    Argument& AddFlag(const T short_name ,const std::string& long_name = "",const std::string& diff = "");
    template <typename T>
    Argument& AddHelp(const T short_name ,const std::string& long_name = "",const std::string& diff = "");

    std::string GetStringValue(std::string name_argumnent);
    int GetIntValue(std::string name_argumnent, int index = 0);
    bool GetFlag(std::string name_argumnent);
    bool Help();

    bool Parse(const std::vector<std::string>& args);

    void add_int_argument(Argument& arg,const std::string& value);
    void add_string_argument(Argument& arg,std::string& value);
    void add_flag_argument(Argument& arg);
    void add_help_argument(Argument& arg);

    bool IsNormal();
    Argument& name_arg_pos();
private:
    std::string name;
    std::map<std::string, Argument> arguments;
    std::map<std::string,std::string> long_to_short_name;
};


}