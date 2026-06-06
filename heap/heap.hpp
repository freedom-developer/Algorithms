#ifndef WSB_HEAP_HPP
#define WSB_HEAP_HPP

#include <cstddef>
#include <vector>

namespace wsb {
namespace heap {

/* 打造一个通用的堆容器，底层用vector保存数据
*/
template <typename T>
class heap 
{
public:
    heap() {}

    size_t size() { return size_; }
    bool empty() { return size_ == 0; }
    

private:


    ::std::vector<T> data_;
    std::size_t size_;
};

}
}


#endif