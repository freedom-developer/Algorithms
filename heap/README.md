# heap

`heap/` 目录用于实现和测试堆相关算法，主要参考《算法导论》中完全二叉树数组表示的堆结构，同时模仿 C++ 标准库的堆算法和 `std::priority_queue` 接口。

核心文件：

- [`heap.hpp`](heap.hpp)：提供堆算法。
- [`priority_queue.hpp`](priority_queue.hpp)：基于 `heap.hpp` 实现优先队列适配器。

## heap.hpp

`heap.hpp` 中的函数都定义在 `wsb::heap` 命名空间下，使用迭代器区间 `[first, last)` 表示堆所在范围。

默认比较器是 `std::less<T>`，因此默认构建和维护的是大顶堆。传入 `std::greater<T>` 时，可以得到小顶堆语义。

### 函数列表

| 函数 | 作用 | 调用前要求 | 调用后结果 | 复杂度 |
| --- | --- | --- | --- | --- |
| `is_heap_until` | 找到第一个破坏堆性质的位置 | 随机访问区间 | 返回迭代器 | O(n) |
| `is_heap` | 判断区间是否是堆 | 随机访问区间 | 返回 `bool` | O(n) |
| `make_heap` | 原地建堆 | 随机访问区间 | `[first, last)` 成为堆 | O(n) |
| `push_heap` | 把尾部新元素加入堆 | `[first, last - 1)` 已是堆 | `[first, last)` 成为堆 | O(log n) |
| `pop_heap` | 把堆顶移动到尾部 | `[first, last)` 已是堆 | `[first, last - 1)` 成为堆 | O(log n) |

### make_heap

```cpp
template <typename Iter, typename Comp = std::less<value_type>>
void make_heap(Iter first, Iter last, Comp comp = Comp{});
```

`make_heap` 使用自底向上的方式建堆，从最后一个非叶子节点开始执行下沉调整。虽然单次下沉最多是 `O(log n)`，但大部分节点位于树的底层，实际可下沉高度很小，所以整体复杂度是 `O(n)`。

```cpp
#include "heap.hpp"

#include <vector>

std::vector<int> data = {3, 1, 5, 2, 4};
wsb::heap::make_heap(data.begin(), data.end());
```

默认是大顶堆，堆顶位于 `data.front()`。

小顶堆：

```cpp
wsb::heap::make_heap(data.begin(), data.end(), std::greater<int>{});
```

### is_heap 和 is_heap_until

```cpp
bool ok = wsb::heap::is_heap(data.begin(), data.end());
auto pos = wsb::heap::is_heap_until(data.begin(), data.end(), std::less<int>{});
```

`is_heap` 用于判断整个区间是否满足堆性质。`is_heap_until` 返回第一个破坏堆性质的位置；如果整个区间都是堆，则返回 `last`。

### push_heap

```cpp
data.push_back(6);
wsb::heap::push_heap(data.begin(), data.end());
```

调用 `push_heap` 前，必须保证 `[first, last - 1)` 已经是堆，且 `*(last - 1)` 是刚追加的新元素。函数会把新元素向上调整到合适位置。

### pop_heap

```cpp
wsb::heap::pop_heap(data.begin(), data.end());
int top = data.back();
data.pop_back();
```

`pop_heap` 会把堆顶元素交换到 `last - 1`，然后调整 `[first, last - 1)`。它不会缩小容器大小；如果要删除堆顶，需要调用方自己执行 `pop_back()`。

## priority_queue.hpp

`priority_queue.hpp` 提供 `wsb::heap::priority_queue`，它是基于 `heap.hpp` 的容器适配器，接口风格接近 `std::priority_queue`。

```cpp
template <
    typename T,
    typename Container = std::vector<T>,
    typename Compare = std::less<typename Container::value_type>
>
class priority_queue;
```

### 基本用法

```cpp
#include "priority_queue.hpp"

wsb::heap::priority_queue<int> q;
q.push(3);
q.push(1);
q.push(5);

int top = q.top(); // 5
q.pop();
```

默认比较器是 `std::less<int>`，因此默认是大顶优先队列。

### 从区间构造

```cpp
std::vector<int> data = {3, 1, 5, 2, 4};
wsb::heap::priority_queue<int> q(data.begin(), data.end());
```

构造函数会把输入区间复制到底层容器中，然后调用 `wsb::heap::make_heap` 建堆。

### 小顶优先队列

```cpp
wsb::heap::priority_queue<
    int,
    std::vector<int>,
    std::greater<int>
> q;
```

此时 `top()` 返回当前最小元素。

### 成员函数

| 函数 | 作用 |
| --- | --- |
| `top()` | 返回堆顶元素引用 |
| `empty()` | 判断是否为空 |
| `size()` | 返回元素数量 |
| `push(value)` | 插入元素并维护堆 |
| `emplace(args...)` | 原地构造元素并维护堆 |
| `push_range(first, last)` | 批量插入后重新建堆 |
| `pop()` | 删除堆顶元素 |

## 注意事项

- `heap.hpp` 的堆算法要求随机访问迭代器。
- `pop_heap` 不会删除元素，只会把堆顶移动到尾部。
- `priority_queue::top()` 要求队列非空。
- 当前实现用于学习和实验，接口和异常安全还没有完全覆盖标准库 `std::priority_queue` 的全部细节。
