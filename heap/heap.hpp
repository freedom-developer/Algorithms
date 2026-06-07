#ifndef WSB_HEAP_HPP
#define WSB_HEAP_HPP

#include <cstddef>
#include <vector>
#include <functional>

namespace wsb {
namespace heap {

/* 打造一个通用的堆容器，底层用vector保存数据
*/
template <typename T, typename Comp = ::std::less<T>>
class heap 
{
public:
    enum class type { max, min };

    explicit heap(type heap_type = type::max, Comp comp = Comp{}) : type_(heap_type), comp_(comp) {}

    size_t size() { return size_; }
    bool empty() { return size_ == 0; }
    

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
        
    }

    ::std::vector<T> data_;
    ::std::size_t size_;
    type type_;
    Comp comp_;
};

}
}


#endif
