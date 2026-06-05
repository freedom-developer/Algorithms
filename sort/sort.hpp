
#include <iterator>
#include <type_traits>
#include <random>
#include <functional>
#include <cstdlib>
#include <vector>

namespace wsb {
namespace sort {

/*
 * 插入排序，排序范围为 [s, e)。
 *
 * 迭代器要求：
 * - Iter 至少是双向迭代器，因为算法需要通过 prev 向前比较相邻元素。
 *
 * 比较规则：
 * - comp(a, b) 为 true 表示 a 应排在 b 前面。
 * - 默认使用 ::std::less，按升序排序。
 *
 * 算法思路：
 * - 从第二个元素开始，将当前元素不断与前一个元素比较。
 * - 如果当前元素应排在前一个元素之前，则交换二者。
 * - 每轮结束后，[s, j] 区间保持有序。
 *
 * 复杂度：
 * - 最好 O(n)，平均和最坏 O(n^2)。
 * - 原地排序，额外空间 O(1)。
 */
template <
    typename Iter, 
    typename Comp = ::std::less<typename ::std::iterator_traits<Iter>::value_type>
>
void insert(Iter s, Iter e, Comp comp = Comp{})
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    static_assert(::std::is_base_of<::std::bidirectional_iterator_tag, category>::value,
                 "insert_sort require a bidirectional iterator");

    if (s == e)
        return;

    for (auto j = ::std::next(s); j != e; ++j) {
        for (auto i = j; i != s && comp(*i, *::std::prev(i)); --i)
            ::std::iter_swap(::std::prev(i), i);
    }
}

/*
 * 快速排序，排序范围为 [s, e)。
 *
 * 迭代器要求：
 * - Iter 必须是随机访问迭代器，因为算法需要计算区间长度和随机下标。
 *
 * 比较规则：
 * - comp(a, pivot) 为 true 表示 a 应放入 pivot 左侧。
 * - 默认使用 ::std::less_equal，按升序排序，并把等于 pivot 的元素放到左侧分区。
 *
 * 算法思路：
 * - 随机选择一个元素作为 pivot，并先交换到区间末尾。
 * - 扫描 [s, e - 1)，把满足 comp(*j, pivot) 的元素交换到左侧。
 * - 将 pivot 放回分区中点，然后递归排序左右两个子区间。
 *
 * 复杂度：
 * - 平均 O(n log n)，最坏 O(n^2)。
 * - 原地排序，递归栈平均 O(log n)，最坏 O(n)。
 */
template <
    typename Iter,
    typename Comp = ::std::less_equal<typename ::std::iterator_traits<Iter>::value_type>
>
void quick(Iter s, Iter e, Comp comp = Comp{})
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    static_assert(::std::is_base_of<::std::random_access_iterator_tag, category>::value,
                 "quick require a random access iterator");

    if (e - s <= 1)
        return;
    
    auto r = ::std::rand() % (e-s); 
    ::std::iter_swap(::std::prev(e), ::std::next(s, r));

    auto pivot = *::std::prev(e);

    auto i = s;
    for (auto j=s; j != ::std::prev(e); ++j) {
        if (comp(*j, pivot))
            ::std::iter_swap(j, i++);
    }
    ::std::iter_swap(i, ::std::prev(e));

    quick(s, i, comp);
    quick(::std::next(i), e, comp);
}

/*
 * 三路快速排序，排序范围为 [s, e)。
 *
 * 迭代器要求：
 * - Iter 必须是随机访问迭代器，因为算法需要计算区间长度和随机下标。
 *
 * 比较规则：
 * - comp(a, b) 为 true 表示 a 应排在 b 前面。
 * - 默认使用 ::std::less，按升序排序。
 *
 * 算法思路：
 * - 随机选择 pivot，并交换到区间起点。
 * - 将区间划分为三个连续部分：小于 pivot、等于 pivot、大于 pivot。
 * - 只递归排序“小于”和“大于”两个子区间，跳过中间相等区间。
 *
 * 适用场景：
 * - 当输入中存在大量重复元素时，通常比普通快速排序更高效。
 *
 * 复杂度：
 * - 平均 O(n log n)，最坏 O(n^2)。
 * - 原地排序，递归栈平均 O(log n)，最坏 O(n)。
 */
template <
    typename Iter,
    typename Comp = ::std::less<typename ::std::iterator_traits<Iter>::value_type>
>
void quick_3way(Iter s, Iter e, Comp comp = Comp{})
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    static_assert(::std::is_base_of<::std::random_access_iterator_tag, category>::value,
                 "quick_3way require a random access iterator");

    if (e - s <= 1)
        return;
    
    auto r = ::std::rand() % (e - s);
    ::std::iter_swap(s, ::std::next(s, r));
    
    auto key = *s;
    auto lt = s;
    auto gt = ::std::prev(e);
    auto i = ::std::next(s);
    while (i <= gt) {
        if (comp(*i, key)) {
            ::std::iter_swap(lt, i);
            lt++;
            i++;
        } else if (comp(key, *i)) {
            ::std::iter_swap(gt, i);
            gt--;
        } else
            i++;
    }

    quick_3way(s, lt, comp);
    quick_3way(::std::next(gt), e, comp);
}

/*
 * 计数排序，排序范围为 [s, e)，元素值必须位于 [min, max]。
 *
 * 迭代器要求：
 * - Iter 至少是前向迭代器，因为算法需要先统计，再从头写回结果。
 *
 * 元素要求：
 * - 元素需要能转换为 int，用于计算计数数组下标。
 * - 元素需要可复制赋值，用于把排序后的原对象写回原区间。
 *
 * 算法思路：
 * - 先把 [s, e) 中的原对象复制到临时容器，避免写回时丢失对象其他成员。
 * - 按对象转换得到的整数键分桶，bucket[i] 保存键为 min + i 的原对象。
 * - 按键从小到大遍历桶，并把桶中的原对象写回原区间。
 *
 * 复杂度：
 * - 时间 O(n + k)，其中 n 为元素数量，k = max - min + 1。
 * - 额外空间 O(n + k)。
 */
template <typename Iter>
void counting(Iter s, Iter e, int min, int max)
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    using value_type = typename ::std::iterator_traits<Iter>::value_type;
    static_assert(::std::is_base_of<::std::forward_iterator_tag, category>::value,
                 "counting require a forward iterator");

    if (s == e || max < min)
        return;

    ::std::vector<value_type> items(s, e); // 复制原数据
    ::std::vector<::std::vector<value_type>> buckets(max - min + 1);

    for (const auto &item : items) {
        int key = static_cast<int>(item);
        buckets[key - min].push_back(item); // 值与下标关联，值相同的元素在同一个桶中
    }

    auto out = s;
    for (const auto &bucket : buckets) { // 遍历下标中的元素列表，将数据按顺序输出
        for (const auto &item : bucket) {
            *out = item;
            ++out;
        }
    }
}

/*
 * 整数型计数排序，排序范围为 [s, e)，元素值必须位于 [min, max]。
 *
 * 迭代器要求：
 * - Iter 至少是前向迭代器，因为算法需要先统计，再从头写回结果。
 *
 * 元素要求：
 * - 元素需要能转换为 int，用于计算计数数组下标。
 * - int 也需要能转换为元素类型，因为写回时会用整数值重新构造/赋值元素。
 *
 * 算法思路：
 * - 使用 tmp[i] 统计值 min + i 出现的次数。
 * - 按值从小到大遍历计数数组，并把对应整数值写回原区间。
 *
 * 适用场景：
 * - 适合 int 或能安全由 int 表示完整数据的类型。
 * - 如果元素是包含多个成员的对象，应使用 counting，避免只按整数键写回而丢失原对象数据。
 *
 * 复杂度：
 * - 时间 O(n + k)，其中 n 为元素数量，k = max - min + 1。
 * - 额外空间 O(k)。
 */
template <typename Iter>
void countint_i(Iter s, Iter e, int min, int max)
{
    using category = typename ::std::iterator_traits<Iter>::iterator_category;
    using value_type = typename ::std::iterator_traits<Iter>::value_type;
    static_assert(::std::is_base_of<::std::forward_iterator_tag, category>::value,
                 "countint_i require a forward iterator");
    static_assert(::std::is_convertible<value_type, int>::value,
                 "countint_i require value_type convertible to int");
    static_assert(::std::is_convertible<int, value_type>::value,
                 "countint_i require int convertible to value_type");

    auto numbers = max - min + 1;
    if (s == e || numbers <= 0)
        return;

    int *tmp = new int[numbers]();

    for (auto i = s; i != e; ++i) {
        auto idx = static_cast<int>(*i) - min;
        tmp[idx]++;
    }

    auto out = s;
    for (int i = 0; i < numbers; i++) {
        auto val = i + min;
        while (tmp[i]-- > 0) {
            *out = static_cast<value_type>(val);
            out++;
        }
    }

    delete[] tmp;
}


} // namespace sort
} // namespace wsb
