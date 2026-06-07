#ifndef WSB_HEAP_HPP
#define WSB_HEAP_HPP

#include <cstddef>
#include <vector>
#include <functional>

namespace wsb {
namespace heap {

/* 打造一个通用的堆容器，底层默认用vector保存数据
*/
template <
    typename T, 
    typename Container = ::std::vector<T>,
    typename Comp = ::std::less<typename Container::value_type> // 控制是大堆，还是小堆
>
class heap 
{
public:
    heap(const Comp& comp, const Container &cont) : comp_(comp), cont_(cont)
    {
    }

    heap() : heap(Comp{}, Container{}) {}

    template <typename InputIt>
    heap(InputIt first, InputIt last, const Comp& comp = Comp())
    {
        using value_type = ::std::iterator_traits<InputIt>::value_type
        cont_ = Container<value_type>(::std::distance(first, last));
        
        // todo: 用[first, last)填充 cont_

        // todo: 建零
    }

    enum class type { max, min };
    explicit heap()

private:

    static constexpr int _parent(int i) { return (i - 1) / 2; }
    static constexpr int _left(int i) { return 2 * i + 1; }
    static constexpr int _right(int i) { return 2 * i + 2; }

    /* 维护堆
    */
    void _heapify_down(int i)
    {
        if (i < 0) return;

        if (type_ == type::max) { // 大堆，data_[i]元素变小了，需要向下调整
            while (i < size_) {
                auto largest = i;
                auto left = _left(i);
                auto right = _right(i);
                if (left < size_ && comp_(data_[largest], data_[left])) 
                    largest = left;
                if (right < size_ && comp_(data_[largest], data_[right])) 
                    largest = right;
                if (largest == i) break;
                ::std::swap(data_[largest], data_[i]);
                i = largest;    
            }
        } else { // 小堆，data_[i]元素变大了，需要向下调整
            while (i < size_) {
                auto smallest = i; 
                auto left = _left(i);
                auto right = _right(i);
                if (left < size_ && comp_(data_[left], data_[smallest]))
                    smallest = left;
                if (right < size_ && comp_([data_[right], data_[smallest]]))
                    smallest = right;
                if (smallest == i) break;
                ::std::swap(data_[smallest], data_[i]);
                i = smallest;
            }
        }
    }

    void _heapify_up(int i)
    {
        if (type_ == type::max) { // 大堆，data_[i]变大 且 > parent 时，需要向向上调整
            while (i >= 0) {
                auto parent = _parent(i);
                if (parent < 0 || !comp_(data_[parent], data_[i]))
                    break;
                ::std::swap(data_[parent], data_[i]);
                i = parent;
            }
        } else { // 小堆，data_[i]变小 且 < parent 时，需要向上调整
            while (i >= 0) {
                auto parent = _parent(i);
                if (parent < 0 || !comp_(data_[i], data_[parent]))
                    break;
                ::std::swap(data_[parent], data_[i]);
                i = parent;
            }
        }

    }

    // 建堆: 从最后非叶子结点开始，
    void _build()
    {
        if (size_ == 0) 
            return;
        auto last_parent = size_ / 2 - 1;
        for (auto i = last_parent; i >= 0; --i) {
            _heapify_down(i);
        }
    }

    
    ::std::size_t size_;
    type type_;

    Container cont_;
    Comp comp_;
};

}
}


#endif
