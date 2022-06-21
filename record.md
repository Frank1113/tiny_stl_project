#TinySTL project 

## 5.22

开始做了。
首先做了Type_Traits，里面就是把库函数的那个部分包装了一下放进来了

constructor.h

用于对象的构造和析构的类型，依赖于type_traits和iterator类（主要是其中的forward转发，暂时用库函数的这个类型代替）

get到了可变参数模板和类型转发机制

在模板函数中，如果要推断的类型保持一致，需要在参数列表中使用右值引用，通过折叠机制保留参数

最佳的办法是使用forward提供的完美转发机制

