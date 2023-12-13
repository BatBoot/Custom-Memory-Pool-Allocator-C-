#include <iostream>
#include <chrono>
#include <forward_list>
#include "custom_alloc_lib/pool_alloc.h"

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::forward_list<std::string, pool_alloc<std::string>> pool_vec(5);

    for(auto i = 0u; i < 20; ++i) pool_vec.push_front("29");
    pool_vec.push_front("a");
    auto end_time = std::chrono::high_resolution_clock::now();


    for(const auto& elem : pool_vec)
        std::cout << elem << " ";
    std::cout << std::endl;

    pool_vec = {"a", "b", "c"};
    for(const auto& elem : pool_vec)
        std::cout << elem << " ";
    std::cout << std::endl;

    auto alloc_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << alloc_time << "\n";
}
