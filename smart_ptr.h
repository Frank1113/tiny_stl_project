#ifndef SMART_PTR_H
#define SMART_PTR_H
#include <functional>
#include <iostream>
#define ATOMIC_ADD(src_ptr, v) (void)__sync_add_and_fetch(src_ptr, v)
#define ATOMIC_SUB(src_ptr, v) __sync_sub_and_fetch(src_ptr, v)


namespace mystl{
    template <typename T>
    class weak_ptr;
    template <typename T>
    class smart_ptr;
    class ref_count
    {
        typedef volatile long atomic_t;

    public:
        
        ref_count(long num):
            m_ref_count(num)
        {
        }
        
        ~ref_count()
        {
        }

        inline void inc()
        {
            ATOMIC_ADD(&m_ref_count, 1);
        }

        inline bool dec_and_check_zero(){
            return 0 == ATOMIC_SUB(&m_ref_count, 1);
        }

        inline void reset(long num)
        {
            m_ref_count = num;
        }

        int size()
        {
            return m_ref_count;
        }

    private:
        atomic_t m_ref_count;
    };


// smart_ptr
    template <typename T>
    class smart_ptr
    {
        friend class weak_ptr<T>;
    public:
        smart_ptr():m_p_use_count(nullptr),m_pobject(nullptr){};
        explicit smart_ptr(T*);//explicit不允许隐式转换
        smart_ptr(const smart_ptr&);
        smart_ptr(T*, std::function<void(T*)>);
        smart_ptr(const weak_ptr<T>&);
        smart_ptr& operator=(const smart_ptr&);
        T& operator*() const;
        T* operator->() const;

        ~smart_ptr();
        explicit operator bool() const;

        bool unique();
        void reset();
        void reset(T*);
        void reset(T*, std::function<void(T*)>);
        T* release();
        T* get() const;

    private:
        
        static std::function<void(T*)> default_del;

    private:
        ref_count* m_p_use_count = nullptr;
        T* m_pobject = nullptr;
        std::function<void(T*)> m_del = default_del;
    };

    template <typename T>
    std::function<void(T*)> smart_ptr<T>::default_del = [](T* p) {delete p; p = nullptr;};


    template <typename T>
    smart_ptr<T>::smart_ptr(T *p)
        :m_pobject(p)
    {
        m_p_use_count = new ref_count(1);
    }

    template <typename T>
    smart_ptr<T>::smart_ptr(const smart_ptr& rhs)
        :m_pobject(rhs.m_pobject), m_del(rhs.m_del)
    {
        m_p_use_count = rhs.m_p_use_count;
        m_p_use_count->inc();
    }

    template <typename T>
    smart_ptr<T>::smart_ptr(const weak_ptr<T>& rhs)
    {
        this->m_p_use_count = rhs.weak_count;
        this->m_pobject = rhs.m_pobject;
    }

    template <typename T>
    smart_ptr<T>::smart_ptr(T *p, std::function<void(T*)> del)
        :m_pobject(p), m_del(del)
    {
        m_p_use_count = new ref_count(1);
    }

    template <typename T, typename...Args>
    smart_ptr<T> make_smart(Args&&... args)
    {
        smart_ptr<T> sp(new T(std::forward<Args>(args)...));
        return sp;
    }

    template <typename T>
    smart_ptr<T>& smart_ptr<T>::operator = (const smart_ptr& rhs)
    {
        rhs.m_p_use_count->inc();
        if(m_p_use_count && m_p_use_count->dec_and_check_zero()){
            delete m_pobject;
        }
        m_del = rhs.m_del;
        m_p_use_count = rhs.m_p_use_count;
        m_pobject = rhs.m_pobject;
        return *this;
    }

    template<typename T>
    T& smart_ptr<T>::operator*() const
    {
        return *m_pobject;
    }

    template <typename T>
    T* smart_ptr<T>::operator->() const
    {
        return &this->operator*();
    }

    template <typename T>
    smart_ptr<T>::~smart_ptr()

    {
        if(m_pobject)
        {
            if(m_p_use_count->dec_and_check_zero())
            {
                m_del(m_pobject);
                delete m_p_use_count;
                m_p_use_count = nullptr;
            }
        }
    }

    template <typename T>
    bool smart_ptr<T>::unique()
    {
        return m_p_use_count->size() == 1;
    }

    template <typename T>
    void smart_ptr<T>::reset()
    {
        if(m_p_use_count->dec_and_check_zero())
        {
            m_del(m_pobject);
        }
        m_pobject = nullptr;
        m_p_use_count -> reset(0);
        m_del = default_del;
    }

    template <typename T>
    void smart_ptr<T>::reset(T* p)
    {

        if(m_p_use_count->dec_and_check_zero())
        {
            m_del(m_pobject);
        }

        m_pobject = p;
        m_p_use_count -> reset(1);
        m_del = default_del;
    }

    template <typename T>
    void smart_ptr<T>::reset(T *p, std::function<void(T*)> del)
    {
        reset(p);
        m_del = del;
    }

    template <typename T>
    T* smart_ptr<T>::release()
    {
        if(m_p_use_count -> dec_and_check_zero())
        {
            m_p_use_count -> reset(1);
        }

        auto p = m_pobject;
        m_pobject = nullptr;

        return p;
    }

    template <typename T>
    T* smart_ptr<T>::get() const
    {
        return m_pobject;
    }

    template <typename T>
    smart_ptr<T>::operator bool() const
    {
        return m_pobject != nullptr;
    }

// weak_ptr
//配合smart_ptr使用
template <typename T>
class weak_ptr
{
public:
    weak_ptr(){};
    weak_ptr(const weak_ptr& );
    weak_ptr(const weak_ptr&& );
    weak_ptr(const smart_ptr<T>& );
    weak_ptr& operator= (const smart_ptr<T>& );
    weak_ptr& operator= (const weak_ptr& );
    weak_ptr& operator= (const weak_ptr&& );
    smart_ptr<T> lock();
    void swap(weak_ptr& rhs);
    void reset();
    bool expired();
private:
    ref_count* weak_count{nullptr};
    T* m_pobject{nullptr};
    friend class weak_ptr;
    friend class smart_ptr<T>;
};


template <typename T>
weak_ptr<T>::weak_ptr(const weak_ptr& rhs) : weak_count(rhs.weak_count), m_pobject(rhs.m_pobject)
{
}

template<typename T>
weak_ptr<T>& weak_ptr<T>::operator = (const weak_ptr& rhs)
{
    weak_count = rhs.weak_count;
    m_pobject = rhs.m_pobject;
    return *this;
}

template<typename T>
weak_ptr<T>::weak_ptr(const weak_ptr&& rhs)
{
    this->weak_count = rhs.weak_count;
    this->m_pobject = rhs.m_pobject;
    rhs->reset();
}

template<typename T>
weak_ptr<T>::weak_ptr(const smart_ptr<T>& rhs):weak_count(rhs.m_p_use_count), m_pobject(rhs.m_pobject)
{

}

template<typename T>
weak_ptr<T>& weak_ptr<T>::operator= (const weak_ptr&& rhs)
{
    this->weak_count = rhs.weak_count;
    this->m_pobject = rhs.m_pobject;
    rhs->reset();
    return *this;
}

template<typename T>
weak_ptr<T>& weak_ptr<T>::operator= (const smart_ptr<T>& rhs)
{
    this->weak_count = rhs.m_p_use_count;
    this->m_pobject = rhs.m_pobject;
    return *this;
}

template<typename T>
smart_ptr<T> weak_ptr<T>::lock()
{
    return smart_ptr<T>(*this);
}

template<typename T>
void weak_ptr<T>::swap(weak_ptr& other)
{
    std::swap(this->weak_count, other->weak_count);
    T* temp = other->m_pobject;
    other->m_pobject = this->m_pobject;
    this->m_pobject = temp;
}

template<typename T>
void weak_ptr<T>::reset()
{
    weak_ptr<T>().swap(*this);
}

template<typename T>
bool weak_ptr<T>::expired()
{
    return weak_count->size() == 0;
}

}//namespace mystl



// template <typename T>
// class smart_ptr
// {
// public:
//     smart_ptr();
//     explicit smart_ptr(T*);//explicit不允许隐式转换
//     smart_ptr(const smart_ptr&);
//     smart_ptr(T*, std::function<void(T*)>);
//     smart_ptr& operator=(const smart_ptr&);
//     T& operator*() const;
//     T* operator->() const;

//     ~smart_ptr();
//     explicit operator bool() const;

//     bool unique();
//     void reset();
//     void reset(T*);
//     void reset(T*, std::function<void(T*)>);
//     T* release();
//     T* get() const;

// private:
    
//     static std::function<void(T*)> default_del;

// private:
//     unsigned* m_p_use_count = nullptr;
//     T* m_pobject = nullptr;
//     std::function<void(T*)> m_del = default_del;
// };

// template <typename T>
// std::function<void(T*)> smart_ptr<T>::default_del = [](T* p) {delete p; p = nullptr;};


// template <typename T>
// smart_ptr<T>::smart_ptr(T *p)
//     :m_pobject(p), m_p_use_count(new unsigned(1))
// {

// }

// template <typename T>
// smart_ptr<T>::smart_ptr(const smart_ptr& rhs)
//     :m_pobject(rhs.m_pobject), m_p_use_count(rhs.m_p_use_count),m_del(rhs.m_del)
// {
//     (*m_p_use_count)++;
// }

// template <typename T>
// smart_ptr<T>::smart_ptr(T *p, std::function<void(T*)> del)
//     :m_pobject(p), m_p_use_count(new unsigned(1)), m_del(del)
// {
// }

// template <typename T, typename...Args>
// smart_ptr<T> make_smart(Args&&... args)
// {
//     smart_ptr<T> sp(new T(std::forward<Args>(args)...));
//     return sp;
// }

// template <typename T>
// smart_ptr<T>& smart_ptr<T>::operator = (const smart_ptr& rhs)
// {
//     ++(*rhs.m_p_use_count);
//     --(*m_p_use_count);
//     if(m_p_use_count == 0){
//         delete m_pobject;
//     }
//     m_del = rhs.m_del;
//     m_p_use_count = rhs.m_p_use_count;
//     m_pobject = rhs.m_pobject;

//     return *this;
// }

// template<typename T>
// T& smart_ptr<T>::operator*() const
// {
//     return *m_pobject;
// }

// template <typename T>
// T* smart_ptr<T>::operator->() const
// {
//     return &this->operator*();
// }

// template <typename T>
// smart_ptr<T>::~smart_ptr()
// {
//     if(--(*m_p_use_count) == 0)
//     {
//         m_del(m_pobject);
//         m_pobject = nullptr;

//         delete m_p_use_count;
//         m_p_use_count = nullptr;
//     }
// }

// template <typename T>
// bool smart_ptr<T>::unique()
// {
//     return *m_p_use_count == 1;
// }

// template <typename T>
// void smart_ptr<T>::reset()
// {
//     (*m_p_use_count)--;

//     if(*m_p_use_count == 0)
//     {
//         m_del(m_pobject);
//     }
//     m_pobject = nullptr;
//     *m_p_use_count = 0;
//     m_del = default_del;
// }

// template <typename T>
// void smart_ptr<T>::reset(T* p)
// {
//     (*m_p_use_count)--;

//     if(*m_p_use_count == 0)
//     {
//         m_del(m_pobject);
//     }

//     m_pobject = p;
//     *m_p_use_count = 1;
//     m_del = default_del;
// }

// template <typename T>
// void smart_ptr<T>::reset(T *p, std::function<void(T*)> del)
// {
//     reset(p);
//     m_del = del;
// }

// template <typename T>
// T* smart_ptr<T>::release()
// {
//     (*m_p_use_count)--;

//     if(*m_p_use_count == 0)
//     {
//         *m_p_use_count = 1;
//     }

//     auto p = m_pobject;
//     m_pobject = nullptr;

//     return p;
// }

// template <typename T>
// T* smart_ptr<T>::get() const
// {
//     return m_pobject;
// }

// template <typename T>
// smart_ptr<T>::operator bool() const
// {
//     return m_pobject != nullptr;
// }

#endif