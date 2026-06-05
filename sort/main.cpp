#include "sort.hpp"

#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <type_traits>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

#define N 100

/////////// 测试结构对象
struct Student {
    std::string name;
    int age;
    int score;

    operator int () const { return age; }    

    bool operator < (const Student &rhs) // std::less会优化使用此函数
    {
        if (this->age != rhs.age) return this->age < rhs.age;
        if (this->score != rhs.score) return this->score < rhs.score;
        return this->name < rhs.name;
    }
};

struct StudentLess {
    bool operator()(const Student &lhs, const Student &rhs) const
    {
        if (lhs.score != rhs.score)
            return lhs.score < rhs.score;
        if (lhs.age != rhs.age)
            return lhs.age < rhs.age;
        return lhs.name < rhs.name;
    }
};

std::ostream &operator<<(std::ostream &os, const Student &student)
{
    os << "{name:" << student.name
       << ", age:" << student.age
       << ", score:" << student.score << "}";
    return os;
}



template <typename Container, typename Iter, typename... Args>
void do_test_func(Container &arr, void (*sort_func)(Iter, Iter, Args...), Args... args)
{
    typedef decltype(arr.begin()) begin_iter;
    typedef decltype(arr.end()) end_iter;

    static_assert(std::is_convertible<begin_iter, Iter>::value, 
        "container begin() iterator does not match sort function iterator");
    static_assert(std::is_convertible<end_iter, Iter>::value, 
        "container end() iterator does not match sort function iterator");

    auto start = std::chrono::steady_clock::now();
    sort_func(arr.begin(), arr.end(), args...);
    auto end = std::chrono::steady_clock::now();
    auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (std::distance(arr.begin(), arr.end()) <= 100) {
        for (const auto &item : arr)
            std::cout << item << ' ';
        std::cout << '\n';
    }

    std::cout << "sort cost: " << cost.count() << " ms\n";
}

template <typename Container, typename Iter, typename... Args>
void test_func(Container &arr, void (*sort_func)(Iter, Iter, Args...))
{
    do_test_func(arr, sort_func, Args{}...);
}

template <typename Container, typename Iter, typename... Args>
void test_func(Container &arr, void (*sort_func)(Iter, Iter, Args...), Args... args)
{
    do_test_func(arr, sort_func, args...);
}



// 测试结构对象示例
void test_students(void)
{
    std::vector<Student> students = {
        {"Alice", 20, 86},
        {"Bob", 19, 92},
        {"Carol", 21, 86},
        {"Dave", 20, 78},
        {"Eve", 18, 92},
        {"Frank", 20, 86},
    };

    // 使用默认的<函数进行比较，比较顺序：age, score, name
    auto ss1 = students;
    test_func(ss1,
              wsb::sort::quick<std::vector<Student>::iterator>); 
    
    // 使用指定的比较函数，比较顺序：score, age, name
    auto ss2 = students;
    test_func(ss2, wsb::sort::quick<std::vector<Student>::iterator, StudentLess>, StudentLess{});

    auto ss3 = students;
    test_func(ss3, wsb::sort::quick_3way<std::vector<Student>::iterator>);


    
}


int main()
{
    test_students();


    // std::vector<int> a1(N);

    // std::srand(time(NULL));
    // std::generate(a1.begin(), a1.end(), [](){ return std::rand() % (N + 1); });

    // auto a2 = a1;
    // test_func(a2, wsb::sort::quick<std::vector<int>::iterator>);
    
    

    

    return 0;
}
