[toc]

## 设计与声明

概述：良好的设计应该对外开放一个易用，不易引起误解的接口，而封装内部的数据和实现。总体来说包括正确性，高效性，封装性，维护性，延展性，以及协议的一致性等。



### - 易用，不易误解的接口

- 一个设计准则是：除非有好理由，不然尽量令你的 type 表现得和内置 type 类型一致。
- **接口具有一致性**可以更加易用。例如 STL 容器中，都统一有 size 函数。与之相反 java 的数组需要用 size，而链表等则用 length，这样就丧失了易用性。

假如有如下日期类，下面两种传递方式都是合乎日常逻辑的，但第一种这却会导致程序引入错误信息。

~~~c++
class Date{
public:
    Date(int month, int day, int year);
    ...
};

Date d(30, 3, 1999);
Date d(3, 30, 1999);
~~~

一种简单的解决方法是，导入外覆类型（wrapper types）来区别年月日。

~~~c++
//年和日的结构体相似，不再列出
struct Month{
    explicit Month(int m)
      : val(m) { }
    int val;
};

Date d(Month(3), Day(30), Year(1995));
~~~

有时候如果要对输入的值加以限制，可以利用 enum，例如可以利用表现月份，但是 enum 不具有我们所希望的类型安全性，因为它可以被当做一个 int 来使用。一个有效方法是预先定义所有有效的值。

~~~c++
class Month {
public:
    //为什么用函数而不用成员变量？
    //因为不同编译单元中局部静态变量的初始化顺序是未知的
    static Month Jan() { return Month(1); }
    static Month Feb() { return Month(2); }
    ...
    static Month Dec() { return Month(12); }
private:
    explicit Month(int x) {};//通过声明私有，阻止生成新的月份
};
~~~



### - Treat class design as type design

**一个类的设计者应该带着“设计语言内置类型”一样的谨慎来研讨类的设计**，好的类型有自然的语法，直观的语义，和一个到多个的实现品。

设计一个类应该仔细考虑以下问题：从设计，异常处理，使用者，和泛化等方面考虑。

- <strong>新 type 的对象应该如何被创建和销毁？</strong>这会影响构造析构和内存分配。
- <strong>对象的初始化和对象的赋值该有什么差别？</strong>
- <strong>新 type 的合法值是什么？</strong>这决定了你的 class 必须维护的约束条件（invariants），也就决定了成员函数（特别是构造函数，赋值运算符，和所谓的“setter”函数）需要进行的错误检查工作。此外还有函数抛出的异常。
- <strong>新 type 需要配合某个继承图系吗？</strong>即继承问题。
- <strong>新 type 需要什么类型转换？</strong>
- <strong>什么样的操作符和函数对该 type 是合理的？</strong>这关于什么函数该声明为成员函数或非成员函数。
- <strong>什么样的标准函数应该驳回？</strong>这正是必须声明为 private 的。
- <strong>谁该使用这个新 type 及其成员？</strong>这可以帮助解决public，protected，private 的使用，和 friend 函数以及嵌套类问题。
- <strong>新 type 有多泛化？</strong>这关乎是否该声明为模板类。





### - 使用常引用代替值传递

从编译器角度看，一个引用往往以指针的类型实现出来。因此传递引用往往意味着传递一个指针，因此对于内置类型来说，值传递往往比引用效率更高。对于 STL 迭代器和函数对象而言，他们习惯上也被设计为值传递方式。



### - 必须返回对象时，不要返回引用

在必须返回对象时返回以下操作**属于危险行为：**

- 返回局部变量的引用

- 函数内部使用 new，返回了 new 出来对象的指针。考虑以下行为：
  
    ~~~c++
    //Rational 是有理数类，当然也可以是你自己写的其它类
    const Rational& operator*(const Rational& l, const Rational& r){
        Rational* result = new Rational(l * r);
        return result;
    }
    
    Rational a, b, c, d;
    a = b * c * d;
    //b * c * d 等价于operator*(operator*(b, c), d)
    //这会导致由operator*(b, c)动态分配的内存无法释放
    ~~~
    
    即使客户聪明地记得每次都要 delete 释放你返回的指针，但是却没有办法可以阻止上述情况产生的内存泄漏，所以返回一个 new 出来的指针绝对是一个愚蠢的做法。
    
- 返回 static 静态对象的引用。考虑以下行为：

    ~~~c++
    const Rational& operator*(const Rational& l, const Rational& r){
        static Rational result = l * r;
        return result;
    }
    
    if ((a * b) == (c * d)) {
        //do something
    } else {
        //do something
    }
    ~~~

    **条件判断总是返回 true ！！！！！**因为虽然两次调用都改变了 result 的值，但是由于它是静态变量且返回的是引用，因此调用端看到的永远是最新一次调用的值。也就是说最新一次调用的值==最新一次调用的值。

### - 封装和非成员函数

