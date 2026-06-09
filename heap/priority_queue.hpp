#ifndef WSB_HEAP_PRIORITY_QUEUE
#define WSB_HEAP_PRIORITY_QUEUE

#include "heap.hpp"

#include <cassert>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace wsb {
namespace heap {


/*
 * 基于 heap.hpp 的优先队列容器适配器。
 *
 * 模板参数：
 * - T: 元素类型。
 * - Container: 底层顺序容器，默认使用 ::std::vector<T>。
 * - Compare: 优先级比较器，默认使用 ::std::less，表现为大顶堆。
 *
 * 类型要求：
 * - Container::value_type 必须与 T 一致。
 * - Container 的迭代器必须是随机访问迭代器，因为堆算法依赖下标访问。
 * - Compare(a, b) 的结果必须能转换为 bool。
 *
 * 语义：
 * - 默认情况下，top() 返回当前最大元素。
 * - 若 Compare 使用 ::std::greater<T>，top() 返回当前最小元素。
 */
template <
    typename T,
    typename Container = ::std::vector<T>,
    typename Compare = ::std::less<typename Container::value_type>
>
class priority_queue 
{
public:
    using container_type = Container;
    using value_compare = Compare;
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

private:
    using iterator_category = typename ::std::iterator_traits<typename Container::iterator>::iterator_category;
    using compare_result = decltype(::std::declval<Compare&>()(
        ::std::declval<const value_type&>(),
        ::std::declval<const value_type&>()));

    static_assert(::std::is_same<T, value_type>::value,
                 "priority_queue require T same as Container::value_type");
    static_assert(::std::is_base_of<::std::random_access_iterator_tag, iterator_category>::value,
                 "priority_queue require Container random access iterator");
    static_assert(::std::is_convertible<compare_result, bool>::value,
                 "priority_queue require Compare result convertible to bool");

public:
    /*
     * 构造空优先队列。
     */
    priority_queue() : priority_queue(Compare{}, Container{}) {}

    /*
     * 使用指定比较器和底层容器构造优先队列。
     *
     * 构造后会对容器执行 make_heap，确保内部数据满足堆性质。
     */
    priority_queue(const Compare& comp, const Container &cont) : c_(cont), comp_(comp)
    {
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    }

    /*
     * 使用指定比较器构造空优先队列。
     */
    explicit priority_queue(const Compare &comp) : priority_queue(comp, Container{}) {}

    /*
     * 移动底层容器构造优先队列。
     */
    priority_queue(const Compare &comp, Container &&cont) : c_(::std::move(cont)), comp_(comp)
    {
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    }

    /*
     * 拷贝和移动构造。
     */
    priority_queue(const priority_queue &other) : priority_queue(other.comp_, other.c_) {}
    priority_queue(priority_queue &&other) : c_(::std::move(other.c_)), comp_(::std::move(other.comp_)) {}

    /*
     * 拷贝赋值。
     */
    priority_queue &operator=(const priority_queue &other)
    {
        c_ = other.c_;
        comp_ = other.comp_;
        return *this;
    }

    /*
     * 移动赋值。
     */
    priority_queue &operator=(priority_queue &&other)
    {
        c_ = ::std::move(other.c_);
        comp_ = ::std::move(other.comp_);
        return *this;
    }

    /*
     * 使用输入区间 [first, last) 构造优先队列。
     *
     * Iter 至少需要是输入迭代器，且 Iter::value_type 需要能转换为 value_type。
     * 构造时会先把区间数据插入底层容器，再执行 make_heap。
     */
    template <typename Iter>
    priority_queue(Iter first, Iter last, const Compare &comp = Compare{}) : comp_(comp)
    {
        using iter_category = typename ::std::iterator_traits<Iter>::iterator_category;
        using iter_value_type = typename ::std::iterator_traits<Iter>::value_type;
        static_assert(::std::is_base_of<::std::input_iterator_tag, iter_category>::value,
                     "priority_queue range constructor require input iterator");
        static_assert(::std::is_convertible<iter_value_type, value_type>::value,
                     "priority_queue range constructor require convertible value_type");

        c_.insert(c_.end(), first, last);
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    } 

    /*
     * 返回堆顶元素。
     *
     * 调用前要求队列非空。
     */
    const_reference top() const
    {
        assert(!c_.empty());
        return c_.front();
    }

    /*
     * 判断队列是否为空。
     */
    bool empty() const { return c_.empty(); }

    /*
     * 返回队列中的元素数量。
     */
    size_type size() const { return c_.size(); }

    /*
     * 插入一个元素，并通过 push_heap 恢复堆性质。
     */
    void push(const value_type &value)
    {
        c_.push_back(value);
        ::wsb::heap::push_heap(c_.begin(), c_.end(), comp_);
    }

    /*
     * 移动插入一个元素，并通过 push_heap 恢复堆性质。
     */
    void push(value_type &&value)
    {
        c_.push_back(::std::move(value));
        ::wsb::heap::push_heap(c_.begin(), c_.end(), comp_);
    }

    /*
     * 批量插入区间 [first, last)，然后重新建堆。
     *
     * 当一次性插入多个元素时，重新 make_heap 通常比逐个 push_heap 更直接。
     */
    template <typename Iter>
    void push_range(Iter first, Iter last)
    {
        using iter_category = typename ::std::iterator_traits<Iter>::iterator_category;
        using iter_value_type = typename ::std::iterator_traits<Iter>::value_type;
        static_assert(::std::is_base_of<::std::input_iterator_tag, iter_category>::value,
                     "priority_queue push_range require input iterator");
        static_assert(::std::is_convertible<iter_value_type, value_type>::value,
                     "priority_queue push_range require convertible value_type");

        c_.insert(c_.end(), first, last);
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    }

    /*
     * 在底层容器末尾原地构造元素，并通过 push_heap 恢复堆性质。
     */
    template <typename... Args>
    void emplace(Args&&... args)
    {
        c_.emplace_back(::std::forward<Args>(args)...);
        ::wsb::heap::push_heap(c_.begin(), c_.end(), comp_);
    }

    /*
     * 删除堆顶元素。
     *
     * 调用前要求队列非空。内部先执行 pop_heap，把堆顶移动到尾部，
     * 再从底层容器中移除尾部元素。
     */
    void pop()
    {
        assert(!c_.empty());
        ::wsb::heap::pop_heap(c_.begin(), c_.end(), comp_);
        c_.pop_back();
    }

private:
    container_type c_;
    value_compare comp_;

};

} // heap
} // wsb

#endif
