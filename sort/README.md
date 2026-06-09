# sort

`sort/` 目录用于实现和测试排序算法，核心头文件是 [`sort.hpp`](sort.hpp)。所有排序函数都定义在 `wsb::sort` 命名空间中，排序范围统一使用迭代器区间 `[s, e)`。

## sort.hpp

`sort.hpp` 当前实现了比较排序、计数排序和桶排序，适合作为《算法导论》中排序章节和 C++ 标准库算法接口风格的练习实现。

### 算法列表

| 函数 | 算法 | 迭代器要求 | 默认行为 | 复杂度 |
| --- | --- | --- | --- | --- |
| `insert` | 插入排序 | 双向迭代器 | 升序 | 最好 O(n)，平均/最坏 O(n^2) |
| `quick` | 快速排序 | 随机访问迭代器 | 升序 | 平均 O(n log n)，最坏 O(n^2) |
| `quick_3way` | 三路快速排序 | 随机访问迭代器 | 升序 | 平均 O(n log n)，最坏 O(n^2) |
| `counting` | 计数排序 | 前向迭代器 | 按整数键升序 | O(n + k) |
| `countint_i` | 整数型计数排序 | 前向迭代器 | 按整数值升序 | O(n + k) |
| `bucket` | 桶排序 | 前向迭代器 | 按整数键升序 | O(n + k) |

其中 `k = max - min + 1`。

## 比较排序

### insert

```cpp
template <typename Iter, typename Comp = std::less<value_type>>
void insert(Iter s, Iter e, Comp comp = Comp{});
```

插入排序适合小规模数据，或者输入数据已经基本有序的场景。算法会从第二个元素开始，把当前元素不断向前交换到合适位置。

```cpp
#include "sort.hpp"

#include <vector>

std::vector<int> data = {3, 1, 2};
wsb::sort::insert(data.begin(), data.end());
```

自定义比较器：

```cpp
wsb::sort::insert(data.begin(), data.end(), std::greater<int>{});
```

### quick

```cpp
template <typename Iter, typename Comp = std::less_equal<value_type>>
void quick(Iter s, Iter e, Comp comp = Comp{});
```

快速排序要求随机访问迭代器。当前实现会随机选择一个元素作为 pivot，并把满足 `comp(*j, pivot)` 的元素放到左侧。

```cpp
std::vector<int> data = {5, 1, 4, 2, 3};
wsb::sort::quick(data.begin(), data.end());
```

注意：当前实现使用 `std::rand()` 选择 pivot。如需不同随机序列，可以在调用前执行 `std::srand()`。

### quick_3way

```cpp
template <typename Iter, typename Comp = std::less<value_type>>
void quick_3way(Iter s, Iter e, Comp comp = Comp{});
```

三路快速排序会把区间划分为“小于 pivot、等于 pivot、大于 pivot”三段，只递归处理小于和大于两段。当数据中重复元素较多时，它通常比普通快速排序更合适。

```cpp
std::vector<int> data = {2, 3, 2, 1, 3, 2};
wsb::sort::quick_3way(data.begin(), data.end());
```

## 计数排序与桶排序

`counting`、`countint_i` 和 `bucket` 都要求元素的整数键位于 `[min, max]` 范围内。

### counting

```cpp
template <typename Iter>
void counting(Iter s, Iter e, int min, int max);
```

`counting` 使用 `static_cast<int>(item)` 作为排序键。它会创建计数数组和临时输出数组，最终把完整对象写回原区间。

这个版本适合“对象有多个成员，但排序键可以转换为 int”的场景。例如：

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

当前 `counting` 不是稳定排序：相同整数键的元素相对顺序可能反转。

### countint_i

```cpp
template <typename Iter>
void countint_i(Iter s, Iter e, int min, int max);
```

`countint_i` 是更轻量的整数型计数排序。它直接根据计数结果把整数值写回原区间，因此要求元素类型既能转换为 `int`，也能由 `int` 转换回来。

```cpp
std::vector<int> data = {3, 1, 2, 1};
wsb::sort::countint_i(data.begin(), data.end(), 1, 3);
```

如果元素是包含多个成员的类对象，不应使用 `countint_i`，否则写回时只会保留整数值所能构造出的数据。

### bucket

```cpp
template <typename Iter>
void bucket(Iter s, Iter e, int min, int max);
```

`bucket` 会为 `[min, max]` 中的每个整数键创建一个桶，把完整对象放入对应桶，再按桶顺序写回。

```cpp
std::vector<int> data = {3, 1, 2, 1};
wsb::sort::bucket(data.begin(), data.end(), 1, 3);
```

`bucket` 是稳定排序：相同整数键的元素会保持原来的相对顺序。

## 注意事项

- 所有函数都会直接修改原区间。
- `insert` 至少需要双向迭代器。
- `quick` 和 `quick_3way` 需要随机访问迭代器。
- `counting`、`countint_i` 和 `bucket` 的整数键必须位于 `[min, max]`。
- `counting` 和 `bucket` 遇到越界键时会抛出 `std::out_of_range`。
- 当前实现用于学习和实验，不是 `std::sort`、`std::stable_sort` 等标准库算法的替代品。
