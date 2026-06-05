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
| `counting` | 对象保留版计数排序 | 前向迭代器 | 元素可转换为 `int`，但需要保留原对象完整数据 |
| `countint_i` | 整数型计数排序 | 前向迭代器 | `int` 或能安全由 `int` 表示完整数据的类型 |

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

### 计数排序

`counting` 会先复制原对象，再按对象转换得到的整数键分桶排序，最后把完整原对象写回原区间。它适合类似结构体或类对象的场景，避免只写回整数值导致其他成员丢失。

```cpp
std::vector<int> data = {3, 1, 2, 1};

wsb::sort::counting(data.begin(), data.end(), 1, 3);
```

`countint_i` 是更轻量的整数型计数排序。它直接按计数结果把整数值写回原区间，因此要求元素既能转换成 `int`，也能由 `int` 转换回来：

```cpp
std::vector<int> data = {3, 1, 2, 1};

wsb::sort::countint_i(data.begin(), data.end(), 1, 3);
```

### 注意事项

- 所有排序函数都会直接修改传入的原容器。
- 计数排序要求所有元素的整数键都位于传入的 `[min, max]` 范围内。
- `quick` 和 `quick_3way` 内部使用 `std::rand()` 随机选择 pivot；如需不同随机序列，可以在调用前自行初始化 `std::srand()`。
- 当前代码主要面向算法学习和实验，不是标准库 `std::sort` 的替代品。
