# Algorithms

这是一个依据《算法导论》和 C++ 标准库接口风格编写的算法练习项目。

项目中的实现主要用于学习算法思想、熟悉标准库常见接口，并对比教材伪代码与 C++ 模板/迭代器写法之间的差异。当前代码不是标准库实现的替代品，更适合作为实验、笔记和练习代码。

## 目录说明

### sort/

[`sort/`](sort/) 目录实现排序相关算法，核心说明见 [`sort/README.md`](sort/README.md)。

该目录中的 [`sort/sort.hpp`](sort/sort.hpp) 提供：

- 插入排序：`wsb::sort::insert`
- 快速排序：`wsb::sort::quick`
- 三路快速排序：`wsb::sort::quick_3way`
- 计数排序：`wsb::sort::counting`
- 整数型计数排序：`wsb::sort::countint_i`
- 桶排序：`wsb::sort::bucket`

`sort/README.md` 详细说明了每个排序函数的接口、迭代器要求、适用场景、复杂度和使用示例。

### heap/

[`heap/`](heap/) 目录实现堆算法和基于堆的优先队列，核心说明见 [`heap/README.md`](heap/README.md)。

该目录中的 [`heap/heap.hpp`](heap/heap.hpp) 提供：

- 堆检查：`wsb::heap::is_heap`、`wsb::heap::is_heap_until`
- 建堆：`wsb::heap::make_heap`
- 插入后调整：`wsb::heap::push_heap`
- 弹出堆顶后调整：`wsb::heap::pop_heap`

[`heap/priority_queue.hpp`](heap/priority_queue.hpp) 提供：

- 基于 `heap.hpp` 的 `wsb::heap::priority_queue`

`heap/README.md` 详细说明了堆算法的调用前提、调用后结果、复杂度、默认大顶堆语义、小顶堆写法，以及 `priority_queue` 的基本接口。

## 编译示例

```bash
g++ -std=c++11 sort/main.cpp -o /tmp/sort_check.out
g++ -std=c++11 heap/main.cpp -o /tmp/heap_check.out
```
