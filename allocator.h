#ifndef MYSTL_ALLOCATOR_H
#define MYSTL_ALLOCATOR_H

template<class T>
class allocator
{
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    //...

    //对operator new进行封装
public:
    static T* allocate();
    static T* allocate(size_type n);

    nonon;



}




#endif //mystl_allocator_h