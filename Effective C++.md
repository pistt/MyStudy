[TOC]



## Const在C++中的使用

### - 尽可能用const，enum，inline代替#define

> #define ASPECT_RATIO 1.653
> 记号名称ASPECT_RATIO可能从未被编译器看见，于是有可能并未进入符号表。最终导致问题的出现。

### - 类中的static const可以只使用声明式

仅当需要访问其地址时需要给出定义式，编译器会在任何用到其声明的地方作常量代换。

> 如下格式，并且在cpp文件内给出定义式。
> const int GamePlayer : : Numturns;

### - const的用法

- 常量折叠：编译器会对常量符号进行值替换

- const 可以和返回值，各参数，函数自身（如果是成员函数）产生关联。

- 如果两个成员函数只是常量性不同，可以被重载。

- 成员函数**const**的意义：有两个流行学派

    1. bitwise constness学派

        > bitwise 学派主张，**成员函数不更改对象之任何成员变量，这也是C++采用的方法**。

    2. logical constness学派
    
        > logical 学派主张，一个const成员函数可以修改它所处理的对象内的某些bit，
        > 但只有在客户端侦测不出的情况下才能如此。例如你的class可能有高速缓冲
        > 文本区块的长度以便应付询问。

- const, non-const成员函数的使用

    如果要实现一个函数的const和non-const版本，理应使用const function实现出non-const版本的
    函数，反之则不成立。    







## RAII ( Resource Acquisition Is Initialization )

**RAII 通常也被称作以对象形式管理资源**

### - 构造函数

C++规定，**成员变量初始化在进入构造函数函数体之前**，因此，在构造体内发生的实际上是变量
的赋值。为了提高效率，**理应使用初始化列表**。

- explicit 的使用

    explicit 关键字用于修饰类的构造函数, 它的作用是表明该构造函数是显式的, 而非隐式的。这意味着，用于初始化的类型不能经过隐式类型转换转化为该类型对类对象进行初始化。而非隐式的则可以。例如下述例子：

    ~~~c++
    class Test {
    public:
        Test(long long size) { } 	//（1）
        explicit Test(int* ptr){ }	//（2）
    };
    
    Test a = 924.34;	//可以通过，调用（1）
    Test b(234.23);		//可以通过，调用（1）
    Test c = nullptr;	//无法通过，不能隐式转换到 int*
    Test d(nullptr);	//可以通过，调用（2）
    Test e = static_cast<int*>(nullptr);//无法通过，并且编译器尝试调用（1）失败
    ~~~

    要解释之前，要先引入两个概念，复制初始化和直接初始化。

    - 复制初始化：使用等号的初始化形式。
    - 直接初始化：使用括号，花括号的初始化形式。

    **explicit 禁止一切的复制初始化行为，但是不禁止直接初始化。**譬如上例对象 b，d 就是直接初始化，而其他的都是复制初始化。**非禁止的行为都是允许隐式类型转换的**。上例中，a，b 使用 double 类型进行初始化，都是隐式的把 double 转换为了 long long 再进行构造。而 d 则是把 nullptr 隐式转换为了 int\* 进行构造。

> 在 *《深度探索 C++对象模型》*中我们做了更深人的讨论。以下是一些特性。

1. 默认构造函数何时被生成？

   **编译器需要他们的时候，而不是程序需要。**一般而言，将会有下面两种情况。

    - 当一个类含有 virtual 成分是，不管其成分是继承自基类还是自身含有；

    - 当一个类含有除自己外的构造函数时，无论其是因为继承还是它有一个”拥有构造函数的对象“

3. 构造函数合成代码的时机

   - 构造函数合成的代码始终放在任何用户代码之前，父类构造函数始终先于子类

3. 构造函数的行为

    - 自动合成构造函数

    ~~~c++
    class Foo {public: Foo(), Foo(int)...};
    class Bar {public: Foo foo; }
    //以下是编译器行为伪代码的模拟
    inline Bar() {
        Foo::Foo();
    }
    ~~~

    - 构造函数的扩张

    ~~~c++
    class Bar {public: Foo(); virtual void fun(); }
    //编译器扩张后可能是这样，具体顺序我不知道，但大致如此
    Bar() {
        Foo::Foo();
        Bar_object.vptr = ...;
        //用户代码
    }
    ~~~

 4. 构造函数的初始化列表

    以下情况只能用初始化列表：

    - 调用父类或者成员的构造函数，且他们需要有参数时
    - 初始化引用或者常量时

    **初始化列表中的成员变量，会依照他们在类中的声明顺序（调用他们的构造函数）一一初始化，而非依照他们在初始化列表中的排列顺序。此外，你可以在初始化列表中调用成员函数进行初始化，但是这对一个继承体系来说，可能是危险的。例如你可能想把一个派生类成员函数的返回值，当做父类构造函数的参数。**

    参考以下代码：

    ~~~c++
    class Base {
    public:
        Base(int);
    };
    class Derived {
    public:
        Derived(int val)
        : a(1),
          b(fun(val)), 
          c(init()),
          d(...)，
          Base(init()) { }
        
        int a;
        int b;
        int c;
        string d;
        int fun(int tmp) { return tmp % 100; }
        int init() {return a;}
    };
    
    //大致会被编译器扩张成
    Derived::Derived(int val){
        //哦豁完蛋，init()返回 a 的值，但这时候 a 还没被初始化
        Base::Base(this, this -> init());
        a = 1;
        b = this -> fun(val);//调用成员函数初始化，确实可行
        c = this -> init();
        d = string::string(...);//调用 string 类的构造函数
        
        //用户代码........
    }
    ~~~

    

    

### - local / global 变量的初始化

> non-local static 对象：这种对象包括 global 对象，定义于 namespace 作用域内的对象，在 class 内，在函数内，以及在 file 作用域内被声明为 static 的对象。

>编译单元(translation unit)：指产出单一目标文件的那些源码。基本上它是单一源码文件加上其所
>含入的头文件。

**如果某个编译单元内的某个 non-local static 对象的初始化动作使用了另一个编译单元内的某个 non-local static 对象，它所指涉到的这个对象可能尚未被初始化。**C++对于定于在不同编译单元内的non-local static对象的初始化相对次序无明确定义，因此如果不加警醒很可能触发未定义行为。此外，对于多线程程序，任何 non-const static 都可能引发问题。

### - 使用 Pimpl 思想来管理资源，降低编译依存性

1. Pimpl 习惯用法的第 1 部分，是声明一个指针指涉到一个非完整型别
2. Pimpl 习惯用法的第 2 部分， 是动态分配和回收持有从前在原始类里的那些数据成员对象，而分配和回收则放在实现文件中

- 考虑以下文件

```c++
// Widget.hpp
class Widget {
private:
    string name;
    vector<double> data;
    Gadget g1, g2, g3; // Gadget 涉及到某个自定义类
};
```

​	乍看之下上面似乎没有什么问题，但实际上。Widget.hpp 对包含 Gadget 的定义文件会产生依赖。这不仅会增加编译的时间，倘若 Gadget 的定义文件发生了变化，Widget.hpp 也需要重新编译。我们使用 Pimpl 的思想，可以很好的解决问题，降低编译依存性。如下：

```c++
class Widget {
private:
    struct Impl; // 声明实现结构体
    Impl *pImpl; // 以及指涉到他的指针
};
```

Widget.hpp 不再需要知道 Gadget 的定义，不需要包含其头文件。这样编译速度提升，而 Widget 也不会因为 Gadget 的改变而需要重新编译。**但是他们的依赖性依然存在，因为在析构的时候需要 delete 指针，而 delete 需要知道被 delete 对象的完整定义。而解决方法同下面 c++11 讨论的一样。**



#### - 走入 c++11，Pimpl更方便的用法

下面是有问题的一种用法：尽管 **unique_ptr** 号称可以支持非完整型别，但是这会带来问题，编译器会在`Widget w`那行报错，**具体错误信息取决于编译器，**无外乎是 delete 或者 sizeof 报错。**！！！shared_ptr 不会产生下述问题！！！**

- 错误的原因在于 Widget 类拥有一个**编译器隐式声明的析构函数，析构函数内编译器会插入数据成员 Pimpl **
- 而 Pimpl 是使用默认析构的 unique_ptr 指针，默认析构使用 delete 算符
- 但是，delete 算符会用 static_assert 确保裸指针未指涉到非完整型别
- 但是， 在编译器生成这段默认代码的过程中，并不知道 Impl 的定义，因此会报错

```C++
// Widget.hpp 内部
class Widget {
public:
    Widget();
private:
    struct Impl; // 声明实现结构体
    unique_ptr<Impl> pImpl; // 以及指涉到他的指针
};

// Widget.cpp 内部
#include "widget.h"
#include "gadget.h"
#include <string>
#include <vector>
struct Widget::Impl {
    string name;
    vector<double> data;
    Gadget g1, g2, g3;
};

Widget::Widget() : pImpl(make_unique<Impl>()) { }

// 某客户代码
#include "widget.h"
Widget w;
```

- 解决方法：在 Widget.hpp 中声明一个析构函数但不定义，在 Widget.cpp 内定义它，而且放在 Impl 定义后面，保证在析构的时候，编译器是知道 Impl 的完整类型的。

```c++
// Widget.hpp 内部增加
class Widget {
public:
    ~Widget();
};

// Widget.cpp 内部增加，注意析构函数的定义
// 要放在 Impl 定义后面
struct Widget::Impl { ... };
Widget::~Widget() { } // 或者 Widget::Widget() = default;
```

#### - 对于移动语义 move

- 下面的想法不错，但同样会产生问题。原因在于：编译器会在 move 构造函数内，抛出异常的事件中，生成析构 pImpl 的代码。而 pImpl 对析构要求具备完整型别。**由于产生问题的原因如出一辙，因此解决方法也同上**

```c++
class Widget {
public:
	Widget(Widget&& rhs) = default;
    Widget& operator<<(Widget&& rhs) = default;
};
```

#### - 总结

Pimpl 是一种可以在类实现和类使用者之间减少编译依赖性的方法，但是从概念上说，Pimpl 并不能改变类所代表的事物，我们依然需要进行深拷贝。



## 赋值/构造/析构/复制函数

### - 赋值运算符

- 赋值运算符应该要实现两个属性：**异常安全，自我赋值安全**

~~~c++
Base& Base::operator=(const Base& rhs){
    //证同测试(identity test)
    //if (this == rhs) return this;
    
    auto tmp = ptr;	//这里的ptr是一个指针，
    ptr = new somthing(*rhs.ptr);
    delete tmp;
    return *this;
}
~~~

这样实现如果在 new 抛出异常，那么 ptr 将保持原状，而且他也能保证自我赋值安全。这里省略了证同测试，因为这一项测试这也需要成本，它会使代码变得更大（包括原始码和目标码），并导入一个新的控制分支，两者都会降低执行速度。Prefetching, caching 和 pipilining 等指令的效率都会因此降低。



**另一个替代方案是，使用 copy and swap 技术，我们将在别的地方讨论他**。有如下两种实现方式。方法 2 将复制动作从函数本体移到了函数参数构造阶段，这有时确实可以令编译器生成更高效的代码，但一个缺点是，这样写牺牲了代码的清晰性和可读性。

~~~c++
//方法1
Base& Base::operator=(const Base& rhs){
    Base tmp(rhs);
    swap(tmp);
    return *this;
}

//方法2
Base& Base::operator=(Base rhs){//rhs是按值传递，意味着它是一个副本
    swap(rhs);
    return *this;
}
~~~



- 赋值运算只在**已初始化变量**之间相互赋值的时候被调用。如下操作的是调用复制或移动构造函数。

~~~c++
Base a = b; //调用复制或者移动构造函数，取决于b是左值还是右值
~~~



- **引用变量**和**const变量**都会**阻止编译器生成默认的赋值运算符**。如下例子：

~~~c++
class Base{
public:
	std::string& namevalue;
	const int objectValue;
};
Base a, b;
a = b; //编译器会报错
~~~





### - 使用构造，析构等编译器自动生成的默认版本

- 使用默认版本

~~~c++
class Base{
public:
    Base() = default;			//构造函数
    ~Base() = default;			//析构函数
    Base(const Base&) = default;//复制构造函数
    Base(Base&&) = default;		//移动构造函数
    Base& operator=(const Base&) = default;	//拷贝赋值
    Base& operator=(Base&&)	= default;		//移动赋值
};
~~~

- 禁用这些函数

    只需要将=defaul替换成=delete即可。当客户尝试使用这些函数的时候即会报错。

### - 复制构造函数

1. 复制构造函数何时被生成？

​         编译器需要他们的时候，即当该类不展现 bitwise copy semantic（位逐次拷贝）。一般而言有以下两种情况（和构造函数类似）

​		**（1）**当该类内含 virtual 成分时，无论是自身含有还是继承等方式含有；

​		**（2）**当该类含有别人的 copy 构造函数时，无论它是继承还是以内含等方式含有。

~~~c++
// String 类对象拥有一个拷贝构造函数
// Foo 类含有一个 String 类对象
class String {public: String(const String&); };
class Foo {public: int member; String tmp; };

Foo a = b;// 当如此拷贝构造的时候，会发生什么？

//编译器会合成一个拷贝构造函数，产生逐层调用
inline Foo::Foo(const Foo& b){
    a.String::String(b);//调用 String 类的拷贝构造函数
    a.member = b.member;
}
~~~



不要忘了在派生类中复制基类成员。只需要在派生类的复制构造函数中使用基类的复制构造函数即可。

~~~c++
Derived::Derived(const Derived& rhs)
    : Base(rhs){ ... }
~~~





### - 多态与虚函数（与final）

概述：要实现虚函数从而实现运行时多态，就要求对象携带某些信息，用以决定运行器间哪个虚函数应该被调用。这通常由每个对象持有的虚指针(vptr)指出，它指向一个由函数指针构成的数组，即虚函数表(vtbl)。每一个带有虚函数的类都有一个相应的虚函数表。当对象调用某个虚函数时，编译器会在这个对象的vptr指向的vtbl中寻找适当的函数指针。（虚表地址和模型等请自行参阅其它）



#### 虚基类
拥有纯虚函数（pure virtual function）的类是抽象类（abstract class），抽象类不能被实体化，
因此非常适合作为基类，这种类我们称之为**虚基类**。由于基类的析构函数通常需要一个虚函数，
所以一种简便的方式是，把基类的析构函数声明为纯虚函数，即可一举两得。

> ~~~c++
> virtual ~Base() = 0;//创建虚函数的方法是在函数后面加一个=0
> Base::~Base(){ }	//我们同时需要为它这个纯虚析构函数提供一个定义
> ~~~

#### 析构函数(继承)中的异常处理

> <a href="#best_practice">最佳解决方案</a>

尽量**不要让异常逸出析构函数**，一方面可能会造成资源泄露（不仅限于内存泄漏），另一方面可能造成未定义行为。假如我们有一个class负责数据库连接，为了确保用户不忘记关闭连结，一个想法是在析构函数中调用关闭函数close()。这个想法对应的伪代码是：

~~~c++
class DBConnection{
public:
    static DBConnection create();//创建一个DBConnection对象
    void close();	//关闭联机，失败则抛出异常
};

class DBConManage{	//用来管理DBConnection对象
public:
    ~DBConManage(){	//确保数据库总是关闭
        db.close();
    }
private:
    DBConnection db;
};
~~~

如果close()函数抛出异常，而析构函数没有处理，就有可能造成资源泄露和未定义行为。合理的操作应该是这样的：

- 如果close()抛出异常就结束程序。通常通过abort完成。
- 吞下close()抛出的异常。

~~~c++
//第一种方案
DBConManager::~DBConManager(){
    try { db.close(); }
    catch (...){
        //do something
        std::abort();
    }
}

//第二种方案
DBConManager::~DBConManager(){
    try { db.close(); }
    catch (...){
       //do something
    }
}
~~~

<a id="best_practice">**最佳解决方案，把调用close()的责任从DBConManager析构函数手上转移到使用这个类的客户手上。**</a>这样可以给客户处理异常的机会，当客户不处理后再转交给系统。如此操作有几个优点：

- 责任归属的转移
- 客户的异常处理更加专一化和具体化（即依赖于客户的具体实现）

~~~C++
class DBConManager{
public:
    void close(){
        db.close(); closed = true;
    }
    ~DBconManager(){
        if (closed) return;
        try {
            db.close();
        } catch (...) {
            //do something
        }
    }

private:
    DBConnection db;
    bool closed;
};
~~~



#### 禁用继承和重写（final）

final 是 c++11 新特性。其主要用途有两种：

- 禁用继承

    ~~~c++
    class Base final{ ... };
    ~~~

- 禁用重写

    ~~~c++
    class Base {
    public:
        virtual void f() final;
    };
    ~~~



#### 不要在构造和析构过程调用虚函数

考虑以下的继承关系：

~~~c++
class Base{
public:
    Base(){f();}
    virtual void f(){ cerr << "Base\n"; }
};

class Derived : public Base{
public:
    Derived(){f();}
    virtual void f() override { cerr << "Derived\n"; }
};

int main(){
    Derived q
    return 0;
}
~~~

> 你以为的预期结果：
> Derived
> Derived
>
> 实际输出结果：
> Base
> Derived

从结果可以知道，base Class 在构造的过程中虚函数绝对不会下降到 derived class 阶层。一个更传神的说法是：**在 base class 构造期间，virtual 函数不是 virtual 函数。相同道理也适用于析构函数**，一旦derived class 析构函数开始执行，对象内的derived class成员变量便呈现未定义值，即**进入 base class析构函数后对象就成为了一个base class 对象**。

- 可行的解决方案是，利用辅助函数创建一个值传给 base class 构造函数往往比较方便，也比较可读。相较在成员初始列中给予 base class 所需数据会更好。

~~~c++
class Base{ ... };
class Dervied : public Base{
public:
    Derived():Base(createSomething( parameters )){ ... }
private:
    value-type createSomething( parameters ){ ... }
};
~~~

