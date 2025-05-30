#include <iostream>
#include <unrolled_list.h>
#include <list.h>
#include <vector>
#include <iostream>
#include <vector>
#include <chrono>
#include "unrolled_list.h"
#include <list>
template <size_t NodeMaxSize>
void test_ull(int n, const std::string &label)
{
    unrolled_list<int, NodeMaxSize> uul;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i)
    {
        uul.push_back(i); // Тест вставки в конец
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << label << " (NodeMaxSize=" << NodeMaxSize << "): "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us\n";
}
void test_list(int n)
{
    std::list<int> vec;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i)
    {
        vec.push_back(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "list: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us\n";
}
void test_vector(int n)
{
    std::vector<int> vec;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; ++i)
    {
        vec.push_back(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Vector: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us\n";
}
// #include <unrolled_list.h>

// #include <gtest/gtest.h>
// #include <gmock/gmock.h>

struct NoDefaultConstructible {
    int x = 5;
    NoDefaultConstructible() = delete;

    NoDefaultConstructible(int) {}
    void add(int y){
        x = y;
    }
    int get(){
        return x;
    }
};



int main()
{
    int n = 1000000; // 1 миллион элементов
    test_vector(n);
    test_list(n);
    test_ull<262144>(n, "ULL-8388608");
    // 8388608
    // std::vector<int> vec;
    // size_t static_size = sizeof(vec);
    // size_t data_size = vec.capacity() * sizeof(int);
    // size_t total_size = static_size + data_size;
    // std::cout << "vector size - " << total_size << std::endl;
    // std::cout <<"UUL size - " << ul.calculate_memory() << std::endl
}