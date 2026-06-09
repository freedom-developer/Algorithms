#ifndef WSB_HEAP_PRIORITY_QUEUE
#define WSB_HEAP_PRIORITY_QUEUE

#include "heap.hpp"

#include <cassert>
#include <functional>
#include <utility>
#include <vector>

namespace wsb {
namespace heap {


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

    priority_queue() : priority_queue(Compare{}, Container{}) {}

    priority_queue(const Compare& comp, const Container &cont) : c_(cont), comp_(comp)
    {
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    }

    explicit priority_queue(const Compare &comp) : priority_queue(comp, Container{}) {}
    priority_queue(const Compare &comp, Container &&cont) : c_(::std::move(cont)), comp_(comp)
    {
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(const priority_queue &other) : priority_queue(other.comp_, other.c_) {}
    priority_queue(priority_queue &&other) : c_(::std::move(other.c_)), comp_(::std::move(other.comp_)) {}

    priority_queue &operator=(const priority_queue &other)
    {
        c_ = other.c_;
        comp_ = other.comp_;
        return *this;
    }

    priority_queue &operator=(priority_queue &&other)
    {
        c_ = ::std::move(other.c_);
        comp_ = ::std::move(other.comp_);
        return *this;
    }

    template <typename Iter>
    priority_queue(Iter first, Iter last, const Compare &comp = Compare{}) : comp_(comp)
    {
        c_.insert(c_.end(), first, last);
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    } 

    const_reference top() const
    {
        assert(!c_.empty());
        return c_.front();
    }

    bool empty() const { return c_.empty(); }

    size_type size() const { return c_.size(); }

    void push(const value_type &value)
    {
        c_.push_back(value);
        ::wsb::heap::push_heap(c_.begin(), c_.end(), comp_);
    }

    void push(value_type &&value)
    {
        c_.push_back(::std::move(value));
        ::wsb::heap::push_heap(c_.begin(), c_.end(), comp_);
    }

    template <typename Iter>
    void push_range(Iter first, Iter last)
    {
        c_.insert(c_.end(), first, last);
        ::wsb::heap::make_heap(c_.begin(), c_.end(), comp_);
    }

    template <typename... Args>
    void emplace(Args&&... args)
    {
        c_.emplace_back(::std::forward<Args>(args)...);
        ::wsb::heap::push_heap(c_.begin(), c_.end(), comp_);
    }

    void pop()
    {
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
