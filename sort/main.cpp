#include "sort.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <vector>
#include <algorithm>
#include <chrono>

#define N 100



int main(int argc, char **argv)
{
    std::vector<int> a1(N);

    std::srand(time(NULL));
    std::generate(a1.begin(), a1.end(), [](){ return std::rand() % (N + 1); });

    auto test_func = [](auto &arr, auto sort_func) {
        auto start = std::chrono::steady_clock::now()
        sort_func(arr.begin(), arr.end());
        auto end = std::chrono::steady_clock::now();
        auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (arr.size <= 100) {
            std::for_each(arr.cbegin(), arr.cend(), [](const int &a) { printf("%d ", a); };
            printf("\n");
        }
        printf("sort cost: %lld ms\n", cost);
    };

    auto a2 = a1;


    return 0;
}