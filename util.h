#ifndef TINY_STL_UTIL_H
#define TINY_STL_UTIL_H
#include <cstddef>
#include "type_traits.h"

namespace mystl
{
    //move
    template <typename T>
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept
    {
        return static_cast<typename remove_reference<T>::type&&> (arg);
    }

    template <typename T>
    T&& forward(typename std::remove_reference<T>::type && arg)
    {
        return static_cast<T &&> (arg);
    }

    template <typename T>
    T&& forward(typename std::remove_reference<T>::type & arg)
    {
        return static_cast<T &&> (arg);
    }

    template <typename T>
    class remove_reference
    {
    public:
        using type = T;
    };

    template <typename T>
    class remove_reference<T &>
    {
    public:
        using type = T;
    };

    template <typename T>
    class remove_reference<T &&>
    {
    public:
        using type = T;
    }


}//namespace mystl



#endif //tiny_stl_util_h