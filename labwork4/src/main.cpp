#include <iostream>
#include <vector>
int main() {
    std::vector<int> v = {1,2,3,4,5,6};

    auto removed = ([](int i){return i % 2;});
    auto result = removed | transform([]
            (int i){return i * i;});

    for(int i : result)
        std::cout << i << " ";
}
