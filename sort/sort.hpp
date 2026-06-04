
#include <iterator>
#include <type_traits>
#include <random>

namespace wsb {
namespace sort {

/* 插入排序:
排序元素[s, e) 
伪代码：
insert_sort: A
    for j = 2 to A.length
        key = A[j]
        i = j - 1
        while i > 0 and A[i] > key
            A[i+1] = A[i]
            i = i - 1
        A[i+1] = key
*/
template <
    typename Iter, 
    typename Comp = std::less<typename std::iterator_traits<Iter>::value_type>
>
void insert(Iter s, Iter e, Comp comp = Comp{})
{
    using category = typename std::iterator_traits<Iter>::iterator_category;
    static_assert(std::is_base_of<std::bidirectional_iterator_tag, category>::value,
                 "insert_sort require a bidirectional iterator");

    if (s == e)
        return;

    for (auto j = std::next(s); j != e; ++j) {
        for (auto i = j; i != s && comp(*i, *std::prev(i)); --i)
            std::iter_swap(std::prev(i), i);
    }
}

/* 快速排序，排序元素[s, e)
伪代码：
quick_sort(A, s, e):
    if (e - s <= 1)
        return
    
    r = random(s, e - 1)
    swap(A[e-1], A[r]) 
    key = A[e-1]
    
    i = s
    for j = s to e - 2
        if A[j] <= key
            swap(A[i], A[j])
            i = i + 1
    swap(A[e-1], A[i])

    quick(A, s, i) // 排序[s, i)
    quick(A, i + 1, e)
*/
template <
    typename Iter,
    typename Comp = std::less_equal<typename std::iterator_traits<Iter>::value_type>
>
void quick(Iter s, Iter e, Comp comp = Comp{})
{
    using category = typename std::iterator_traits<Iter>::iterator_category;
    static_assert(std::is_base_of<std::random_access_iterator_tag, category>::value,
                 "quick_sort require a random access iterator");

    if (e - s <= 1)
        return;
    
    auto r = std::rand() % (e-s); // 最好在调用此函数前调用一次 srand
    std::iter_swap(std::prev(e), std::next(s, r));

    auto pivot = *std::prev(e);

    auto i = s;
    for (auto j=s; j != std::prev(e); ++j) {
        if (comp(*j, pivot))
            std::iter_swap(j, i++);
    }
    std::iter_swap(i, std::prev(e));

    quick(s, i, comp);
    quick(std::next(i), e, comp);
}

/* 3路快排：每次将数据分为3块，即<, == >。排序[s, e)
伪代码：
quick3(A, s, e):
    if (e - s <= 1) return

    r = random(s, e-1)
    swap(A[s], A[r])

    key = A[s]
    lt = s
    gt = e - 1
    i = s + 1

    while (i <= gt)
        if (A[i] < key)
            swap(A[i], A[lt])
            i = i + 1
            lt = lt + 1
        else if (A[i] > key)
            swap(A[i], A[gt])
            gt = gt - 1
        else
            i = i + 1
    // 此时
    // A[s .. lt - 1] 均 < key
    // A[gt + 1, .. e) 均 > key
    // A[lt .. gt] 均 == key

    quick3(A, s, lt) // [s, lt)
    quick3(A, gt + 1, e)

*/
template <
    typename Iter,
    typename Comp = std::less<typename std::iterator_traits<Iter>::value_type>
>
void quick_3way(Iter s, Iter e, Comp comp = Comp{})
{
    using category = typename std::iterator_traits<Iter>::iterator_category;
    static_assert(std::is_base_of<std::random_access_iterator_tag, category>::value,
                 "quick_sort_3way require a random access iterator");

    if (e - s <= 1)
        return;
    
    auto r = std::rand() % (e - s);
    std::iter_swap(s, std::next(s, r));
    
    auto key = *s;
    auto lt = s;
    auto gt = std::prev(e);
    auto i = std::next(s);
    while (i <= gt) {
        if (comp(*i, key)) {
            std::iter_swap(lt, i);
            lt++;
            i++;
        } else if (comp(key, *i)) {
            std::iter_swap(gt, i);
            gt--;
        } else
            i++;
    }

    quick_sort_3way(s, lt, comp);
    quick_sort_3way(std::next(gt), e, comp);
}

/////////// 计数排序，输入为[s, e]
template <typename T>
void counting_sort(T *d, T max, int s, int e)
{
    std::vector<T> c(max+1, 0); // d[x] 在[0, max]区间中的整数
    for (int i = s; i <= e; i++)
        c[d[i]]++; // 初始化计数

    int pos = s;
    for (T i = 0; i <= max; i++) {
        while (c[i]-- > 0)
            d[pos++] = i;
    }
}


} // namespace sort
} // namespace wsb
