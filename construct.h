#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H
#include "type_traits.h"
#include <new>
#include <utility>
//用于构造和析构的模板类
//construct 构造
//destroy 析构

namespace mystl{
    
    template <class Ty>
    void construct(Ty* ptr)
    {
        ::new ((void*) ptr) Ty(); //new 传入地址的分配
    }

    template <class Ty1, class Ty2>
    void construct(Ty1* ptr, Ty2& value)
    {
        ::new ((void*) ptr) Ty1(value);
    }

    template <class Ty, typename ...Args>//可变参数模板
    void construct(Ty* ptr, Args&& ...args)
    {
        ::new ((void*) ptr) Ty(std::forward<Args> args...);//通过forward实现完美转发
    }

    //destroy析构对象
    //析构分为单一对象析构和多对象析构 使用true_type和false_type判断对象的析构函数是默认合成版本与否
    template <class Ty>
    void destroy_one(Ty*, std::true_type){}
    //有合成的默认析构函数 无需手动调用

    //false_type 需要调用其析构函数
    template <class Ty>
    void destroy_one(Ty* pointer, std::false_type)
    {
        if(pointer != nullptr){
            pointer->~Ty();
        }
    }
    
    //无需做任何事
    template <class ForwardIter>
    void destroy_cat(ForwardIter, ForwardIter, std::true_type){}

    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
    {
        for (; first != last; ++first)
            destroy(&*first);
    }

    template <class Ty>
    void destroy(Ty* pointer)
    {
        destroy_one(pointer, std::is_trivially_destructible<Ty>{});
    }

    template <class ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        destroy_cat(first, last, std::is_Trivially_destructible<typename iterator_traits<ForwardIter>::value_type>{});
    }
    
}//namespace mystl


#endif //constructor_h