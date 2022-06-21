#include <iostream>
#include "smart_ptr.h"
using std::cout;
using std::endl;

class A
{
public:
    A()
    {
        std::cout << "A default construct" << std::endl;
    }

    ~A()

    {
        cout << "A destory" << endl;
    }
};

smart_ptr<A> test(){
    smart_ptr<A> ptr = make_smart<A>();
    return ptr;
}

void testB(smart_ptr<A> _ptr){
    smart_ptr<A> ptr2 = _ptr;
    cout << "testB" << endl;
}

int main(){
    smart_ptr<int> ptr = make_smart<int>(1);
    std::cout << *ptr << std::endl;
    testB(test());
    return 0;
}