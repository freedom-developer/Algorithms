#include "heap.hpp"
#include "priority_queue.hpp"

#include <vector>
#include <random>
#include <ctime>
#include <cstdio>

#include <queue>
#include <algorithm>

int main(int argc, char **argv)
{
    srand(time(NULL));

    std::priority_queue<int> q1;
    q1.push(1);
    // auto a2 = std::make_heap<int>();

    std::vector<int> a1(20);
    std::generate(a1.begin(), a1.end(), []() { return rand() % 101; });
    std::for_each(a1.begin(), a1.end(), [](const int &a) { printf("%d ", a); });
    printf("\n");

    wsb::heap::make_heap(a1.begin(), a1.end());
    if (wsb::heap::is_heap(a1.begin(), a1.end()))
        printf("make heap success\n");
    else
        printf("make heap failed\n");

    auto size = a1.size();
    for (size_t i = 0; i < size; ++i) {
        printf("%d ", a1[0]);
        wsb::heap::pop_heap(a1.begin(), a1.end());
        a1.pop_back();
    }
    printf("\n");

    std::vector<int> a2(30);
    std::generate(a2.begin(), a2.end(), []() { return rand() % 101; });
    std::for_each(a2.begin(), a2.end(), [](const int &a) { printf("%d ", a); });
    printf("\n");

    wsb::heap::priority_queue<int, std::vector<int>, ::std::greater<int>> q2(a2.begin(), a2.end(), std::greater<int>());
    size = q2.size();
    for (decltype(size) i = 0; i < size; i++) {
        printf("%d ", q2.top());
        q2.pop();
    }
    printf("\n");


    return 0;
}
