#include <iostream>
#include <fstream>
#include <sstream>
#include "interpreter.h"

int main() {
    std::ifstream f("/Users/kostakazackov/Downloads/labwork10-kostyaas/examples/maximum.is");
    std::ostringstream out;
    bool ok = Interpreter(f, out);
    std::cout << "OK=" << ok << "\nOUT=" << out.str() << "\n";
    return 0;
}