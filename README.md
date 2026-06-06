# Algorithms

这是一个用于学习和实现常见算法的数据结构/算法练习项目。

## sort/sort.hpp

`sort/sort.hpp` 提供了一组位于 `wsb::sort` 命名空间下的排序函数。它们都使用迭代器表示排序范围，范围格式统一为 `[s, e)`。

### 已实现的排序算法

| 函数 | 算法 | 迭代器要求 | 适用场景 |
| --- | --- | --- | --- |
| `insert` | 插入排序 | 双向迭代器 | 小规模数据，或基本有序的数据 |
| `quick` | 快速排序 | 随机访问迭代器 | 通用排序场景 |
| `quick_3way` | 三路快速排序 | 随机访问迭代器 | 重复元素较多的数据 |
| `counting` | 计数排序 | 前向迭代器 | 整数键范围较小，且需要保留原对象完整数据 |
| `countint_i` | 整数型计数排序 | 前向迭代器 | `int` 或能安全由 `int` 表示完整数据的类型 |
| `bucket` | 桶排序 | 前向迭代器 | 整数键范围较小，希望按键分桶再写回 |

### 比较排序

`insert`、`quick` 和 `quick_3way` 支持自定义比较器：

```cpp
std::vector<int> data = {3, 1, 2};

wsb::sort::insert(data.begin(), data.end());
wsb::sort::quick(data.begin(), data.end());
wsb::sort::quick_3way(data.begin(), data.end());
```

如果需要自定义排序规则，可以传入比较函数对象：

```cpp
wsb::sort::quick(data.begin(), data.end(), std::greater<int>{});
```

### 计数排序与桶排序

`counting` 使用 `static_cast<int>(item)` 作为排序键，先统计每个整数键出现的次数，再把计数数组原地转换为前缀计数，最后把完整原对象移动到临时数组并写回原区间。

它适合类似结构体或类对象的场景：排序依据是一个整数键，但写回时不能丢失对象的其他成员。当前实现不是稳定排序；相同整数键的元素在输出中可能反转相对顺序。

```cpp
std::vector<int> data = {3, 1, 2, 1};

wsb::sort::counting(data.begin(), data.end(), 1, 3);
```

例如对象可以通过转换函数提供整数键：

```cpp
struct Student {
    std::string name;
    int age;
    int score;

    operator int() const
    {
        return age;
    }
};

std::vector<Student> students = {
    {"Alice", 20, 86},
    {"Bob", 19, 92},
    {"Eve", 18, 92},
};

wsb::sort::counting(students.begin(), students.end(), 18, 20);
```

`countint_i` 是更轻量的整数型计数排序。它直接按计数结果把整数值写回原区间，因此要求元素既能转换成 `int`，也能由 `int` 转换回来：

```cpp
std::vector<int> data = {3, 1, 2, 1};

wsb::sort::countint_i(data.begin(), data.end(), 1, 3);
```

`bucket` 会为 `[min, max]` 中的每个整数键创建一个桶，把原对象按输入顺序放入对应桶中，再按桶下标写回。它同样保留原对象完整数据，并保持相同整数键元素的相对顺序。

```cpp
std::vector<int> data = {3, 1, 2, 1};

wsb::sort::bucket(data.begin(), data.end(), 1, 3);
```

`counting` 和 `bucket` 的时间复杂度都是 `O(n + k)`，额外空间都是 `O(n + k)`，其中 `k = max - min + 1`。通常 `counting` 使用连续辅助数组，桶容器开销更少；`bucket` 的分桶过程更直观，并且当前实现保持相同整数键元素的原相对顺序。

### 注意事项

- 所有排序函数都会直接修改传入的原容器。
- `counting`、`countint_i` 和 `bucket` 要求所有元素的整数键都位于传入的 `[min, max]` 范围内。
- `counting` 和 `bucket` 在发现整数键越界时会抛出 `std::out_of_range`。
- `quick` 和 `quick_3way` 内部使用 `std::rand()` 随机选择 pivot；如需不同随机序列，可以在调用前自行初始化 `std::srand()`。
- 当前代码主要面向算法学习和实验，不是标准库 `std::sort` 的替代品。但在整数键范围很小的大规模数据上，计数排序类算法可能明显快于比较排序。
