# ifndef MYTINYSTL_MEMORY_H
# define MYTINYSTL_MEMORY_H

namespace mystl
{
    // 获取对象地址
    template <class T>
    constexpr Tp* address_of(Tp& value) noexcept
    {
        return &value;
    }
}


# endif