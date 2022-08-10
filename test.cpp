#include <iostream>
#include <memory>
#include "smart_ptr.h"
using std::cout;
using std::endl;
using namespace mystl;

class B;  
  
class A  
{  
public:  
    //std::shared_ptr<B> ptrA_B;
    smart_ptr<B> ptrA_B;  
public:  
    A()  
    {  
        cout << "调用class A的默认构造函数" << endl;  
    }  
    ~A()  
    {  
        cout << "调用class A的析构函数" << endl;  
    }  
};  
  
class B  
{  
public:  
    smart_ptr<A> ptrB_A;  
   // std::shared_ptr<A> ptrB_A;
public:  
    B()  
    {  
        cout << "调用class B的默认构造函数" << endl;  
    }  
    ~B()  
    {  
        cout << "调用class B的析构函数" << endl;  
    }  
};  
  
int main()  
{  
   // std::shared_ptr<B> ptrB = std::make_shared<B>();
   // std::shared_ptr<A> ptrA = std::make_shared<A>();
    smart_ptr<B> ptrB = make_smart<B>(); 
    smart_ptr<A> ptrA = make_smart<A>();
    cout << "Here" << endl;
    ptrA->ptrA_B = ptrB;
    ptrB->ptrB_A = ptrA;    
    cout << "Here" << endl;
    return 0;
} 