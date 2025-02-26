#include <functional>
#include <lib/ArgParser.h>
#include <numeric>
#include <sstream>
#include <iterator>
#include <string_view>
using namespace ArgumentParser;


std::vector<std::string> SplitString(const std::string& str) {
    std::istringstream iss(str);

    return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}

int main(int argc, char** argv) {
    ArgParser parser("My Parser");
    bool flag3 = false;
    parser.AddFlag('c', "flag3").StoreValue(flag3);
    std::string temp = "c";
    parser.Parse(SplitString("app -c"));
    std::cout << flag3;
}
