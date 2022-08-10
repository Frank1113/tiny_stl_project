#ifndef MYTINYSTL_ALGOBASE_H_
#define MYTINYSTL_ALGOBASE_H_
#include <cstring>
#include "iterator.h"
#include "util.h"

namespace mystl
{

    /*******************************************************/
    // max
    // 取二者中的较大值
    /*******************************************************/
    template <class T>
    const T& max(const T& lhs, const T& rhs)
    {
        return lhs < rhs ？ rhs : lhs;
    }

    // 使用 comp
    template <class T, class Compare>
    const T& max(const T& lhs, const T& rhs, Compare comp)
    {
        return comp(lhs, rhs) ? rhs : lhs;
    }
    
    /********************************************/
    // copy
    /********************************************/
    
    //copy first last copy到 result的位置
    template <class InputIter, class OutputIter>
    OutputIter
    unchecked_copy_cat(InputIter first, InputIter last, OutputIter result,
                       mystl::input_iterator_tag)  
    {
        for(; first != last; ++first, ++result)
        {
            *result = *first;
        }

        return result;
    }

    //random_access_iterator_tag 版本
    template <class RandomIter, class OutputIter>
    OutputIter
    unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result,
                      mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++ result)
        {
            *result = *first;
        }
        return result;
    }

    template <class InputIter, class OutputIter>
    OutputIter
    unchecked_copy(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_copy_cat(first, last, result, iterator_category(first));
    }

    // trivially_copy_assgingble
    template <class Tp, class Up>
    typename std::enable_if<
      std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
      std::is_trivially_copy_assignable<Up>::value,
      Up*>::type
      unchecked_copy(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
            std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter copy(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_copy(first, last, result);
    }

    /****************************************************************/
    // copy_backward 
    // [first last) 拷贝到 [result - (last - first), result) 内
    /****************************************************************/
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                                BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while(first != last)
          *--result = * --last;
        return result;
    }

    
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result)
    {
      for(auto n = last - first; n > 0; --n)
        *--result = *--last;
      return result;
    }
    
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result)
    {
      return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
    }

    // trivally_copy_assignable 特化版本
    template <class Tp, class Up>
    typename std::enable_if<
      std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
      std::is_trivially_copy_assignable<Up>::value,
      Up*>::type
    unchecked_copy_backward(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template<class Bidirectionaliter1, class BidirectionalIter2>
    BidirectionalIter2
    copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_copy_backward(first, last, result);
    }
    
    // copy_n
    template <class InputIter, class Size, class OutputIter>
    mystl::pair<InputIter, OutputIter>
    unchecked_copy_n(InputIter first, Size n, OutputIter result, mystl::input_iterator_tag)
    {
        for(; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }
        return mystl::pair<InputIter, OutputIter>(first, result);
    }

    template <class RandomIter, class Size, class OutputIter>
    mystl::pair<RandomIter, OutputIter>
    unchecked_copy_n(RandomIter first, Size n, OutputIter result,
                     mystl::random_access_iterator_tag)
    {
        auto last = first + n;
        return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
    }

    template <class InputIter, class Size, class OutputIter>
    mystl::pair<InputIter, OutputIter>
    copy_n(InputIter first, Size n, OutputIter result)
    {
        return unchecked_copy_n(first, n, result, iterator_category(first));
    }

    /************************************************************************/
    //fill_n
    //从 first 开始填充n个值
    /************************************************************************/
    template <class OutputIter, class Size, class T>
    OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value)
    {
        for (; n > 0; --n, ++first)
        {
            *first = value;
        }
        return first;
    }

    // one-byte 类型的特化版本
    template <class Tp, class Size, class Up>
    typename std::enable_if<
      std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
      !std::is_same<Tp, bool>::value &&
      std::is_integral<Up>::value && sizeof(Up) == 1,
      Tp*>::type
      unchecked_fill_n(Tp* first, Size n, Up value)
      {
        if(n > 0)
        {
            std::memset(first, (unsigned char)value, (size_t)(n));
        }
        return first + n;
      }

    template <class OutputIter, class Size, class T>
    OutputIter fill_n(OutputIter first, Size n, const T& value)
    {
        return unchecked_fill_n(first, n, value);
    }

    /************************************************************************/
    // fill
    /************************************************************************/
    template <class ForwardIter, class T>
    void fill_cat(ForwardIter first, ForwardIter last, const T& value,
                 mystl::forward_iterator_tag)
    {
        for(; first != last; ++first)
        {
            *first = value;
        }
    }

    template <class RandomIter, class T>
    void fill_cat(RandomIter first, RandomIter last, const T& value,
                  mystl::random_access_iterator_tag)
    {
        fill_n(first, last - first, value);
    }

    template <class ForwardIter, class T>
    void fill(ForwardIter first, ForwardIter last, const T& value)
    {
        fill_cat(first, last, value, iterator_category(first));
    }

    /************************************************************************/
    //move
    /************************************************************************/

    template <class InputIter, class OutputIter>
    OutputIter
    unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
                      mystl::input_iterator_tag)
    {
        for(; first != last; ++first, ++result)
        {
            *result = mystl::move(*first);
        }
        return result;
    }

    // random_access_iterator_tag 版本
    template <class RandomIter, class OutputIter>
    OutputIter
    unchecked_move_cat(RandomIter first, RandomIter last, OutputIter rsult,
                       mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = mystl::move(*first);
        }

        return result;
    }

    // random_access_iterator_tag 版本
    template <class InputIter, class OutputIter>
    OutputIter
    unchecked_move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move_cat(first, last, result, iterator_category(first));
    }

    // 为 trivally_copy_assignable 提供特化版本
    template <class Tp, class Up>
    typename std::enable_if<
      std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
      std::is_trivially_move_assignable<Up>::value,
      Up*>::type
    unchecked_move(Tp* first, Tp* last, Up* result)
    {
        const size_t n = static_cast<size_t>(last - first);
        if(n != 0)
          std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move(first, last, result);
    }
    
}

#endif