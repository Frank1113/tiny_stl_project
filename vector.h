#ifndef MYTINYSTL_VECTOR_H_
#define MYTINYSTL_VECTOR_H_

#include <initializer_list>

#include "iterator.h"
#include "util.h"
#include "algo.h"
#include "allocator.h"
#include "exceptdef.h"

namespace mystl
{

    // 模板类 vector
    template <class T>
    class vector
    {
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandonded in mystl");
    public:
        
        //vector 嵌套型别定义
        typedef mystl::allocator<T>                      allocator_type;
        typedef mystl::allocator<T>                      data_allocator;


        typedef typename allocator_type::value_type       value_type;
        typedef typename allocator_type::pointer          pointer;
        typedef typename allocator_type::const_pointer    const_pointer;
        typedef typename allocator_type::reference        reference;
        typedef typename allocator_type::const_reference  const_reference;
        typedef typename allocator_type::size_type        size_type;
        typedef typename allocator_type::difference_type  difference_type;

        typedef value_type*                               iterator;
        typedef const value_type*                         const_iterator;
        typedef mystl::reverse_iterator<iterator>         reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator>   const_reverse_iterator;

        allocator_type get_allocator() { return data_allocator()};

    private:
      iterator begin_;  // 表示目前使用空间的头部
      iterator end_;    // 表示目前使用空间的尾部
      iterator cap_;    // 表示目前储存空间的尾部    
    
    public:
      
      //构造  复制 移动 析构函数
      vector() noexcept
      {try_init()};
      

      explicit vector(size_type n)
      { fill_init(n, value_type());}

      vector(size_type n, const value_type& value)
      { fill_init(n, value)};

      template<class Iter, typename std::enable_if<
        mystl::is_input_iterator<Iter>::value, int>::type = 0>
      vector(Iter first, Iter fast)
      {
        MYSTL_DEBUG(!(last < first));
        range_init(first, last);
      }

      vector( const vector& rhs )
      {
        range_init(rhs.begin_, rhs.end_);
      }

      vector(vector&& rhs) noexcept
        :begin_(rhs.begin_),
        end_(rhs.end_),
        cap(rhs.cap_)
      {
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
      }

      vector(std::initializer_list<value_type> ilist)
      {
        range_init(ilist.begin(), ilist.end());
      }

      vector& operator=(const vector& rhs);
      vector& operator=(vector&& rhs) noexcept;

      vector& operator=(std::initializer_list<value_type> ilist)
      {
        vector tmp(ilist.begin(), ilist.end());
        swap(tmp);
        return *this;
      }

      //迭代器相关操作
      iterator                  begin()        noexcept
      { return begin_; }

      const iterator            begin()  const noexcept
      { return begin_; }

      iterator                  end()          noexcept
      { return end_; }

      const_iterator            end()    const noexcept
      { return end_; }

      reverse_iterator          rbegin()       noexcept
      { return reverse_iterator(end()); }

      const_reverse_iterator    rbegin() const noexcept
      { return reverse_iterator(begin());}

      const_iterator            cbegin() const noexcept
      { return begin(); }

      const_iterator            cend()   const noexcept
      { return end(); }

      const_reverse_iterator    crbegin() const noexcept
      { return rbegin(); }

      const_reverse_iterator    crend()   const noexcept
      { return rend(); }


      //容量相关操作
      bool     empty()  const noexcept
      { return begin_ == end_; }

      size_type size()  const noexcept
      { return static_cast<size_type>(end_ - begin_);}

      size_type max_size() const noexcept
      { return static_cast<size_type>(-1) / sizeod(T);}

      size_type capacity() const noexcept
      { return static_cast<size_type>(cap_ - begin_);}

      void reserve(size_type n);
      void shrink_to_fit();


      // 访问元素相关操作
      reference operator[](size_type n)
      {
        MYSTL_DEBUG(n < size())
        return *(begin_ + n);
      }

      const_reference operator[](size_type n) const
      {
        MYSTL_DEBUG(n < size());
        return *(begin_ + n);
      }

      reference at(size_type n)
      {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscrpt out of range");
        return (*this)[n];
      }

      const_reference at(size_type n) const
      {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
        return (*this)[n];
      }

      reference front()
      {
        MYSTL_DEBUG(!empty());
        return *begin_;
      }

      const_reference front() const
      {
        MYSTL_DEBUG(!empty());
        return *begin_;
      }

      reference back()
      {
        MYSTL_DEBUG(!empty());
        return *begin_;
      }

      const_reference back() const
      {
        MYSTL_DEBUG(!empty());
        return *(end_ - 1);
      }

      pointer        data()          noexcept {return begin_};
      const_pointer  data()   const  noexcept {return begin_};
      
      ~vector()
      {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
      }

      // 修改容器的相关操作

      

      // erase
      iterator erase(const_iterator pos);
      iterator erase(const_iterator first, const_iterator last);
      void clear() {erase(begin(), end())};
      
      // emplace / emplace_back

      template <class... Args>
      iterator emplace(const_iterator pos, Args&& ...args);

      template <class... Args>
      void emplace_back(Args&& ...args);

      // push_back / pop_back

      void push_back(const value_type& value);
      void push_back(value_type&& value)
      { emplace_back(mystl::move(value))};

      // swap
      void     swap(vector& rhs) noexcept;

    private:
      
      //helper function
      //initialize destroy
      void try_init() noexcept;
      void fill_init(size_type n, const value_type& value);
      void init_space(size_type size, size_type cap);
      
      template <class Iter>
      void range_init(Iter first, Iter last);

      void destroy_and_recover(iterator first, iterator last, size_type n);


      // calculate the growth size
      size_type get_new_cap(size_type add_size);

      // assign

      void fill_assign(size_type n, const value_type& value);

      template <class IIter>
      void copy_assign(IIter first, IIter last, input_iterator_tag);

      template <class FIter>
      void copy_assign(FIter first, FIter last, forward_iterator_tag);

      // reallocate_emplace
      template <class ...Args>
      void reallocate_emplace(iterator pos, Args&& ...args);
      void reallocate_insert(iterator pos, const value_type& value);
      
      
      // insert

      iterator fill_insert(iterator pos, size_type n, const value_type& value);
      template <class IIter>
      void     copy_insert(iterator pos, IIter first, IIter last);

      // shrink_to_fit
      void reinsert(size_type size);
    };

    //构造 拷贝 赋值
    template <class T>
    vector<T>& vector<T>::operator=(const vector& rhs)
    {
      if(this != &rhs)
      {
        const auto len = rhs.size();
        if(len > capacity())
        {
          vector temp(rhs.begin(), rhs.end())
          swap(temp);
        }
        else if(size() >= len)
        {
          auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
          data_allocator::destroy(i, end_);
          end_ = begin_ + len;
        }
        else
        {
          mystl::copy(rhs.begin(), rhs.begin() + size(), begin_);
          mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
          cap_ = end_ = begin_ + len;
          data_allocator::deallocate(end_);
        } 
      }

      return *this;
    }

    // 移动赋值

    template <class T>
    vector<T>& vector<T>::operator=(vector&& rhs) noexcept
    {
      destroy_and_recover(begin_, end_, cap_ - begin_);
      begin_ = rhs.begin_;
      end_ = rhs.end_;
      cap_ = rhs.cap_;
      rhs.begin_ = nullptr;
      rhs.end_ = nullptr;
      rhs.cap_ = nullptr;
      return *this;
    }


    // 预留空间大小 当原容量小于要求大小时 重新分配

    template <class T>
    void vector<T>::reserve(size_type n)
    {
      if(capacity() < n)
      {
        THROW_LENGTH_ERROR_IF(n > max_size(),
                              "n cannot be larger than max_size() in vector<t>::reserve" );
        const auto old_size = size();
        auto tmp = data_allocator::allocate(n);
        mystl::uninitialized_move(begin_, end_, tmp);
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = tmp;
        end_ = tmp + old_size;
        cap_ = begin_ + n;
      }
    }

    // 放弃多余的容量
    template <class T>
    void vector<T>::shrink_to_fit()
    {
      if(end_ < cap_)
      {
        reinsert(size());
      }
    }


    // 在pos位置就地构造元素 避免额外的复制或移动开销

    template <class T>
    template <class ...Args>
    typename vector<T>::iterator
    vector<T>::emplace(const_iterator pos, Args&& ...args)
    {
      MYSTL_DEBUG(pos >= begin() && pos <= end());
      iterator xpos = const_cast<iterator>(pos);
      const size_type n = xpos - begin_;
      if (end_ != cap_ && xpos == end_)
      {
        data_allocator::construct(mystl::address_of(*end), mystl::forward<Args>(args)...);
        ++end_;
      }
      else if (end_ != cap_)
      {
        data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
        mystl::copy_backward(xpos, end_-1, end_);
        *xpos = value_type(mystl::forward<Args>(args)...);
        ++end_;
      }
      else
      {
        reallocate_emplace(xpos, mystl::forward<Args>(args)...);
      }
      return begin() + n;
    }
    
    // 在尾部就地构造元素
    template <class T>
    template <class ...Args>
    void vector<T>::emplace_back(Args&& ...args)
    {
      if (end_ < cap_)
      {
        data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
        ++end_;
      }
      else
      {
        reallocate_emplace(end_, mystl::forward<Args>(args)...);
      }
    }

    // 在尾部插入元素
    template <class T>
    void vector<T>::push_back(const value_type& value)
    {
      if(end_ != cap_)
      {
        data_allocator::construct(mystl::address_of(*end_), value);
        ++end_;
      }
      else
      {
        reallocate_insert(end_, value);
      }
    }

    // reallocate_emplace
    template <class T>
    template <class ...Args>
    void vector<T>::reallocate_emplace(iterator pos, Args&& ...args)
    {
      const auto new_size = get_new_cap(1);
      auto new_begin = data_allocator::allocate(new_size);
      auto new_end = new_begin;

      try
      {
        new_end = mystl::uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(mystl::address_of(*new_end), mystl::forward<Args>(args)...);
        ++new_end;
        new_end = mystl::uninitialized_move(pos, end_, new_end);
      }
      catch( ... )
      {
        data_allocator::deallocate(new_begin, new_size);
        throw;
      }

      destroy_and_recover(begin_, end_, cap_ - begin_);
      begin_ = new_begin;
      end_ = new_end;
      cap_ = new_begin + new_size;
      
    }

    //reallocate_insert
    template <class T>
    void vector<T>::reallocate_insert(iterator pos, const value_type& value)
    {
      const auto new_size = get_new_cap(1);
      auto new_begin = data_allocator::allocate(new_size);
      auto new_end = new_begin;

      try
      {
        new_end = mystl::uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(mystl::address_of(*new_end), value);
        ++new_end;
        new_end = mystl::uninitialized_move(pos, end_, new_end);
      }
      catch( ... )
      {
        data_allocator::deallocate(new_begin, new_size);
        throw;
      }

      destroy_and_recover(begin_, end_, cap_ - begin_);
      begin_ = new_begin;
      end_ = new_end;
      cap_ = new_begin + new_size;
    }

    // erase pos
    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator pos)
    {
      MYSTL_DEBUG(pos >= begin() && pos < end());
      iterator xpos = begin_ + (pos - begin());
      mystl::move(xpos + 1, end_, xpos);
      data_allocator::destroy(end_ - 1);
      --end_;
      return xpos;
    }

    //删除[first, last)上的元素

    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator first, const_iterator last)
    {
      MYSTL_DEBUG(fierst >= begin() && last <= end() && !(last < first));
      const auto n = first - begin();
      iterator r = begin_ + (first - begin());
      data_allocator::destroy(mystl::move(r + (last - first), end_, r), end_);
      end_ = end_ - (last - first);
      return begin_ + n;
    }

    //swap
    template <class T>
    void vector<T>::swap(vector<T>& rhs) noexcept
    {
      mystl::swap(*this, rhs);
    }



    // helper function

    template <class T>
    void vector<T>::try_init() noexcept
    {
        try
        {
            begin_ = data_allocator::allocate(16);
            end_ = begin_;
            cap_ = begin_ + 16;
        }
        catch(...)
        {
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
            throw;
        }
        
    }
    

    // init_space 
    template <class T>
    void vector<T>::init_space(size_type size, size_type cap)
    {
      try
      {
        begin_ = data_allocator::allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
      }
      catch( ... )
      {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
        throw;
      }
    }
      
      // fill_init 函数
      template <class T>
      void vector<T>::
      fill_init(size_type n, const value_type& value)
      {
        const size_type init_size = mystl::max(static_cast<size_type>(16), 16);
        init_space(n, init_size);
        mystl::uninitialized_fill_n(begin_, n, value);
      }

      // range_init
      template <class T>
      template <class Iter>
      void vector<T>::
      range_init(Iter first, Iter last)
      {
        const size_type init_size = mystl::max(static_cast<size_type>(last - first),
                                               static_cast<size_type>(16));
        init_space(static_cast<size_type>(last - first), init_size);
        mystl_uninitialized_copy(first, last, begin_);
      }

      // destroy and recover
      template <class T>
      void vector<T>::
      destroy_and_recover(iterator first, iterator last, size_type n)
      {
        data_allocator::destroy(first, last);
        data_allocator::deallocate(first, n);
      }


      // reinsert 函数
      template <class T>
      void vector<T>::reinsert(size_type size)
      {
        auto new_begin = data_allocator::allocate(size);
        try
        {
          mystl::uninitialized_move(begin_, end_, new_begin);
        }
        catch(...)
        {
          data_allocator::deallocate(begin_, cap_ - begin_);
          begin_ = new_begin;
          end_ = begin_ + size;
          cap_ = begin_ + size;
        }
        
      }


      // get_new_cap
      template <class T>
      typename vector<T>::size_type
      vector<T>::
      get_new_cap(size_type add_size)
      {
        const auto old_size = capacity();
        THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,
                              "vector<T>'s size too big");
        if (old_size > max_size() - old_size / 2)
        {
          return old_size + add_size > max_size() - 16
            ? old_size + add_size : old_size + add_size + 16;
        }
        const size_type new_size = old_size == 0
          ? mystl::max(add_size, static_cast<size_type>(16))
          : mystl::max(old_size + old_size / 2, old_size + add_size);
        return new_size;
      }

      // fill_assign
      template <class T>
      void vector<T>::
      fill_assign(size_type n, const value_type& value)
      {
        if (n > capacity())
        {
          vector tmp(n, value);
          swap(tmp);
        }
        else if (n > size())
        {
          mystl::fill(begin(), end(), value);
          end_ = mystl::uninitialized_fill_n(end_, n - size(), value);
        }
        else
        {
          erase(mystl::fill_n(begin_, n, value), end_);
        }
      }

      // copy_assign
      template <class T>
      template <class IIter>
      void vector<T>::
      copy_assign(IIter first, IIter last, input_iterator_tag)
      {
        auto cur = begin_;
        for (; first != last && cur != end_; ++first)
        {
          *cur = *first;
        }

        if (first == last)
        {
          erase(cur, end_);
        }
        else
        {
          insert(end_, first, last);
        }
      }

      // copy_insert函数
      template <class T>
      template <class IIter>
      void vector<T>::
      copy_insert(iterator pos, IIter first, IIter last)
      {
        if(first == last)
        {
          return;
        }

        const auto n = mystl::distance(first, last);
        if ((cap_ - end_) >= n)
        {
          const auto after_elems = end_ = pos;
          auto old_end = end_;
          if (after_elems > n)
          {
            end_ = mystl::unintialized_copy(end_ - n, end_, end_);
            mystl::move_backward(pos, old_end - n, old_end);
            mystl::uninitialized_copy(first, last, pos);
          }
          else
          {
            auto mid = first;
            mystl::advance(mid, after_elems);
            end_ = mystl::uninitialized_copy(mid, last, end_);
            end_ = mystl::uninitialized_move(pos, old_end, end_);
            mystl::unintialized_copy(first, mid, pos);
          }
        }
        else
        {
          // 备用空间不足
          const auto new_size = get_new_cap(n);
          auto new_begin = data_allocator::allocate(new_size);
          auto new_end   = new_begin;
          try
          {
            new_end = mystl::unintialized_move(begin_, pos, new_begin);
            new_end = mystl::unintialized_copy(first, last, new_end);
            new_end = mystl::unintialized_move(pos, end_, new_end);
          }
          catch( ... )
          {
            destroy_and_recover(new_begin, new_end, new_size);
            throw;
          }
          data_allocator::deallocate(begin_, cap_ - begin_);
          begin_ = new_begin;
          end_   = new_end;
          cap_   = begin_ + new_size;
        }
      }


} // namespace mystl

#endif //MYTINYSTL_VECTOR_H_