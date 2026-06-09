#ifndef WSB_HEAP_HPP
#define WSB_HEAP_HPP

#include <cstddef>
#include <iterator>
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>

namespace wsb {
namespace heap {

namespace detail {

template <typename Distance>
constexpr Distance left_child(Distance i) { return 2 * i + 1; }

template <typename Distance>
constexpr Distance right_child(Distance i) { return 2 * i + 2; }

template <typename Distance>
constexpr Distance parent(Distance i) { return (i - 1) / 2; }

template <typename Distance>
constexpr Distance last_parent(Distance len) { return len / 2 - 1; }

} // namespace detail

/*
 * 返回 [first, last) 中第一个破坏堆性质的位置。
 *
 * 比较规则：
 * - comp(parent, child) 为 true 表示 child 的优先级高于 parent，堆性质被破坏。
 * - 默认配合 ::std::less 使用时，检查的是大顶堆。
 *
 * 迭代器要求：
 * - Iter 必须是随机访问迭代器，因为堆使用数组下标表示父子关系。
 */
template <typename Iter, typename Comp>
Iter is_heap_until(Iter first, Iter last, Comp comp)
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    static_assert(::std::is_base_of<::std::random_access_iterator_tag, category>::value,
                 "is_heap_until require a random access iterator");

    auto len = last - first;
    decltype(len) parent = 0;

    for (decltype(len) child = 1; child < len; ++child) {
        if (comp(*(first + parent), *(first + child)))
            return first + child;
        if ((child % 2) == 0)
            ++parent;
    }

    return last;
}

/*
 * 判断 [first, last) 是否满足堆性质。
 */
template <typename Iter, typename Comp = ::std::less<typename ::std::iterator_traits<Iter>::value_type>>
bool is_heap(Iter first, Iter last, Comp comp = Comp{})
{
    return ::wsb::heap::is_heap_until(first, last, comp) == last;
}

/*
 * 从 parent_idx 开始向下调整，使 [first, first + len) 恢复堆性质。
 *
 * 该函数假设 parent_idx 的左右子树已经是堆，只需要修复 parent_idx
 * 这个位置可能破坏的父子关系。
 */
template <typename Iter, typename Distance, typename CompVal>
void _adjust_heap(Iter first, Distance parent_idx, Distance len, CompVal comp)
{
    while (::wsb::heap::detail::left_child(parent_idx) < len) {
        auto left = ::wsb::heap::detail::left_child(parent_idx);
        auto tmp_idx = parent_idx;

        if (comp(*(first + tmp_idx), *(first + left)))
            tmp_idx = left;

        auto right = ::wsb::heap::detail::right_child(parent_idx);
        if (right < len && comp(*(first + tmp_idx), *(first + right)))
            tmp_idx = right;

        if (tmp_idx == parent_idx) 
            break;

        ::std::swap(*(first + parent_idx), *(first + tmp_idx));
        parent_idx = tmp_idx;
    }
}

/*
 * 将 [first, last) 建成堆。
 *
 * 默认比较器为 ::std::less，此时构建大顶堆；传入 ::std::greater, 时可构建小顶堆。
 *
 * 复杂度：
 * - 时间 O(n)。
 * - 原地调整，额外空间 O(1)。
 */
template <typename Iter, typename CompVal = ::std::less<typename ::std::iterator_traits<Iter>::value_type>>
void make_heap(Iter first, Iter last, CompVal comp = CompVal{})
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    static_assert(::std::is_base_of<::std::random_access_iterator_tag, category>::value,
                 "make_heap require a random access iterator");

    auto len = last - first;
    if (len < 2) return;

    auto parent = ::wsb::heap::detail::last_parent(len);
    while (true) {
        ::wsb::heap::_adjust_heap(first, parent, len, comp);
        if (parent == 0)
            break;
        parent--;
    }
}

/*
 * 将尾部新元素加入堆。
 *
 * 调用前要求：
 * - [first, last - 1) 已经是堆。
 * - *(last - 1) 是刚插入的新元素。
 *
 * 调用后：
 * - [first, last) 恢复为堆。
 */
template <typename Iter, typename CompVal = ::std::less<typename ::std::iterator_traits<Iter>::value_type>>
void push_heap(Iter first, Iter last, CompVal comp = CompVal{})
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    static_assert(::std::is_base_of<::std::random_access_iterator_tag, category>::value,
                 "push_heap require a random access iterator");

    auto len = last - first;
    if (len < 2) return;

    auto hole = len - 1;
    auto value = ::std::move(*(first + hole));

    while (hole > 0) {
        auto parent = ::wsb::heap::detail::parent(hole);
        if (!comp(*(first + parent), value))
            break;
        *(first + hole) = ::std::move(*(first + parent));
        hole = parent;
    }

    *(first + hole) = ::std::move(value);
}

/*
 * 将堆顶元素移动到区间尾部，并重新调整剩余区间。
 *
 * 调用前要求：
 * - [first, last) 已经是堆。
 *
 * 调用后：
 * - *(last - 1) 保存原堆顶元素。
 * - [first, last - 1) 仍然是堆。
 *
 * 注意：
 * - 该函数不会缩小容器大小；如果要删除堆顶元素，需要调用方再执行 pop_back。
 */
template <typename Iter, typename CompVal = ::std::less<typename ::std::iterator_traits<Iter>::value_type>>
void pop_heap(Iter first, Iter last, CompVal comp = CompVal{})
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    static_assert(::std::is_base_of<::std::random_access_iterator_tag, category>::value,
                 "pop_heap require a random access iterator");

    auto len = last - first;
    if (len < 2) return;

    --last;
    ::std::iter_swap(first, last);
    ::wsb::heap::_adjust_heap(first, decltype(len)(0), len - 1, comp);
}

}
}


#endif
