[TOC]



## C++ 杂项

### - 只允许在堆/栈上分配内存

- 只允许堆：析构函数 private
- 只允许栈：重载 new 和 delete

### - 单例类

>单例一般分为两种模式：懒汉式，饿汉式。顾名思义，懒汉，需要的时候才生成；饿汉，初始时就生成，需要的时候直接调用。
>
>参考这篇博客：https://www.cnblogs.com/sunchaothu/p/10389842.html

- 饿汉式

- 懒汉式

- 双检锁

    双检锁是有问题的，因为内存构造顺序不定（指令重排等等），有可能指针先获得了，但是对象的构造还没完成。此时并发访问的是未构造完成的对象，GG。Java 中使用 volatile 关键字解决；C++ 中要加入内存栅栏。
    
    

### - 成员函数指针

可以参考腾讯云这篇[博客](https://cloud.tencent.com/developer/article/1420751)

获取类成员的偏移地址：`&((class_or_struct*)0) -> class_member`



### - 结构体对齐

- 结构体对齐的规则

1. 设第一个成员的起始地址为0，每个成员的起始地址，必须是其数据类型所占空间大小的整数倍
2. 结构体的最终大小必须是其成员（基础数据类型成员）里最大成员所占大小的整数倍

- pragma pack(n) $n = 2^0,2^1,2^2,2^3....$

    当 n > 结构体内最大的成员变量所占内存是，无效。否则按照制定规则对齐。



- 结构体对齐的目的

    内存对齐，从而提高访问内存的速度，可以一整片地读写。第 2 条规则也是为了内存对齐。

    

- 以下是我的理解了，发现可以贪心，例如下述结构体，a 和 b 塞好后，8 字节不够再放入 c， c 只能自己对齐到 8 字节。最后 d 占 8 字节，一共占 24 字节。实验验证的起始地址也恰好如此。

```c++
struct tmp {
  	int a; 		// 0
    wchar_t b;	// 0x4
    int c;		// 0x8
    double d;	// 0x10
};


// 使用如下语句，可以获取类成员地偏移地址
// 可以使用 #define 定义一个函数方便使用
&((class_or_struct*)0) -> class_member
```



### - RVO

- C++ 自面世以来，就存在返回值优化这一标准，但是这完全取决于编译器的实现。不同编译器可能会有不同的结果，甚至关掉优化还会有部分优化。
- 我们给出测试类和测试函数如下：我们有一个空的 Test 类和一个简单的 foo 函数。（感兴趣的朋友可以自己写构造，复制，移动，赋值函数观察一下）

```c++
class Test{
public:
	Test() { printf("Construct--this:%p\n", (void*)this); }
	Test(const Test& rhs) { 
        printf("Copy-------this:%p\tCopyFrom:%p\n", 
               (void*)this, (void*)&rhs); 
    }
	//Test(Test&&) { printf("Move\n"); }
	//Test& operator=(Test&&) { printf("=Move"); return *this; }
	//Test& operator=(const Test& ) { printf("=Copy"); return *this;}
	~Test() { printf("Destruct---this:%p\n", (void*)this); }
};

Test foo() {
    Test mm; // Line1
    return mm; // Line2
}

// 在 C++14 及以前，下面写法产生的效果，等同于上面的写法
// Test foo() { return Test(); }

//在 main 中调用
Test q = foo();
```



####  - 未开启 RVO 时的情况，对应 g++ 编译命令 -fno-elide-constructors

- 我们来讨论在 main 函数中下述代码产生的效果`Test q = foo();`

- 先来讨论一般的情况，这是我们在 c++ 基础课程中应该了解到的：
    1. 调用到 foo() Line1 时，Test 的构造函数被调用以构造对象 mm ；
    2. 调用到 foo() Line2 时，foo 函数在预留的返回栈上，利用复制构造函数（如果有移动构造函数，则会使用移动构造函数）构造临时的返回对象，暂称为 tmp ；
    3. 随后，foo 函数生命周期即将结束，局部对象 mm 被析构；
    4. tmp 被拷贝构造函数用于构造 main 中对象 q ；
    5. foo 函数生命周期结束，函数占用的内存空间被释放，tmp 被析构；
    6. q 在 main 函数的生命周期结束，被析构。

    - 整个过程一共经历了1次构造，2次复制，2次析构（不含 main 函数中被析构的 q）。根据下面测试的情况也可以佐证。**这也是 C++14 及以前的情况。**

    <img src = ".\images\RVO1.png" alt="RVO实验结果1" style = "zoom:75%" align="left">

- 接下来我们讨论一下 C++17 之后的情况，即使我们关闭了优化，甚至使用 —O0 关闭所有优化，只启用基本的编译模式，结果仍然如下：

    > 在 main 中，Test q = foo(); 和 foo(); 语句产生的结果是一样的，都如下图所示。

    <img src = ".\images\RVO2.png" alt="RVO实验结果2" style = "zoom:75%" align="left">

- 笔者推断，在 c++17 之后，编译器优化了函数内部的结构。例如，在 main 函数中调用带有返回值的函数时，编译器会为 main 函数保留一个接受返回值变量的栈空间，在函数返回的时候直接在该处构建或复制返回变量，而不再另外重新构建返回变量。那么整个语句执行的过程应该如下：

    1. 调用到 foo() Line1 时，Test 的构造函数被调用以构造对象 mm ；
    2. 调用到 foo() LIne2 时，直接在 main 函数预留的栈空间上（这时是属于变量 q 的栈空间），调用复制构造函数（如果有移动构造函数，则会使用移动构造函数）构造变量 q；
    3. 随后，foo 函数生命周期结束，局部变量 mm 被析构。

    - 整个过程一共经历了 1次构造，1次复制，1次析构（不含 main 函数中被析构的 q），对比 c++14 及之前，减少了一次构造变量的开支。

- 根据上述推断，笔者做了另一个实验，将 foo 的代码修改为`Test foo() { return Test(); }`，产生了如下结果：

    > 在 main 中，Test q = foo(); 和 foo(); 语句产生的结果是一样的，都如下图所示。但若返回值没有被接管，就会被立即析构。

    <img src = ".\images\RVO3.png" alt="RVO实验结果3" style = "zoom:75%" align="left">

- 从结果上说，Test() 临时变量直接在属于 q 的栈空间上被构造出来。整个过程只经历了一次构造，比 c++14 及之前减少了 2 次构造变量的开支，比没有改变函数写法时减少了 1 次。

#### - 启用 RVO 后，编译器默认启用（c++11 之后结果均一致）

> 以下讨论对于两种等价形式 Test foo() { Test mm; return mm; } 和 Test foo() { return Test(); }都一样

- 启用 RVO 后，无论是内部的匿名变量，还是命名变量。都会被编译器优化。结果如下

    <img src = ".\images\RVO4.png" alt="RVO实验结果4" style = "zoom:75%" align="left">

- 优化后都是只有一次构造函数和析构函数的调用，可见编译器直接在待返回处构造了这个对象，节省了 1~2 次的构造次数。效率提升了 33%~66%，这是很可怕的。

#### - 启用 RVO 后的一些扩展知识

- 多层函数返回嵌套后，RVO还管用吗？管用！对于下列 3 个函数，嵌套调用。最后在 main 中调用 函数 f2 ，得到的结果是一样的

```c++
Test f4() {
	Test nn = Test();
	return nn;
}

Test f3() {
	Test mm = f4();
	return mm;
}

Test f2() {
	Test bb = f3();
	return bb;
}

//在 main 中
Test p = f2();
```

<img src = ".\images\RVO5.png" alt="RVO实验结果5" style = "zoom:75%" align="left">



- 含有多个返回语句时，返回匿名或命名变量，RVO 还管用吗？管用！

```c++
// Test 类增加以下两个构造函数
Test(int n) {/* ...内部不变，同上... */}
Test(int n, int m) {/* ...内部不变，同上... */}

Test f2(int n) {
	if (n == 10)		return Test();
	else if (n < 10)	return Test(1);
	else				return Test(1,1);
}

Test f3(int n) {
	Test mm;
	if (n == 10)		return mm;
	else if (n < 10)	return mm;
	else				return mm;
}

// 在 main 中调用 Test q = f2();
// 在 main 中调用 Test q = f3();
```

无论是调用函数 f2() 还是函数 f3() 都只会发生一次构造和析构，也就是说，RVO 依然可以帮助我们省去很多不必要的对象构造，提高运行效率。**但是，一旦返回的不是全部为临时对象，或者不是全部为同一局部变量，这时候 RVO 就失去作用了。**如下

```c++
// RVO 失败例子一
Test f2(int n) {
    Test mm;
    if (n == 10) 		return mm;
    else if (n > 10)	return Test(1);
    else				return Test(1,1);
}
// RVO 失败例子二
Test f3(int n) {
    Test mm, NN;
    if (n == 10) 		return mm;
    else if (n > 10)	return NN;
    else				return NN;
}
```



### - 位域

> 参考链接：https://blog.csdn.net/chenzhongjing/article/details/11472549

**基本知识：**

~~~c++
struct Ipv4Header{
	Ipv4Header() : version(0b1000), IHL(0b1000), DSCP(0b100000), ECN(0b10), totalLength(0x1) { }
	std::uint32_t version : 4,
			 	  IHL : 4,
			 	  DSCP : 6,
			 	  ECN : 2,
			 	  totalLength : 16;
};
~~~

C++ 标准说 “非 const 引用不得绑定到位域”，即位域并不能被任何东西绑定，包括指针和引用。实际上，即使使用 const& 绑定某个位域变量，也只是复制了它的副本并绑定到一个“常规”对象。如上代码，定义了一个 IPv4 头部的位域。**位域中变量的内存排布顺序按照声明顺序，从内存低位到高位排放。**这意味着上述代码各个变量的排放位置如下图所示：

<img src=".\images\Ipv4Header.png" alt="Ipv4Header" style="zoom:80%" align="left" />





**位域的对齐**

1. 如果相邻位域字段的类型相同，且其位宽之和小于类型的sizeof大小，则后面的字段将紧邻前一个字段存储，直到不能容纳为止；

2. 如果相邻位域字段的类型相同，但其位宽之和大于类型的sizeof大小，则后面的字段将从新的存储单元开始，其偏移量为其类型大小的整数倍；

3. 如果相邻的两个位域字段的类型不同,则各个编译器的具体实现有差异,VC6采取不压缩方式,GCC和Dev-C++都采用压缩方式;

4. 整个结构体的总大小为最宽基本类型成员大小的整数倍。

5. 如果位域字段之间穿插着非位域字段，则不进行压缩；（不针对所有的编译器）

6. 空位域的使用，可以控制对齐

    <img src=".\images\bitfield.png" alt="bitfield" style="zoom:60%;" align="left" />



### - 模板模板参数

> 详细解释建议见侯捷的视频。

~~~c++
template<class T,
		template<class,
				 class = allocator<T>
		>
		class Container
>
class XCL{
public:
	Container<T> container;
	void fun() {
		if (typeid(container) == typeid(vector<int>))
        	{ cerr << "yes\n"; }
        else { cerr << "no"; }
	}
};

int main() {
    XCL<int, vector> tmp;
    tmp.fun() //输出 yes
    return 0;
}
~~~

模板的头部，template 嵌套 template 的意思是，内部的 Container 也是一个模板类，它接受类型 T 作为它的模板参数。因此可以这样定义 XCL 的类变量**”XCL<int, vector> tmp;“**。这样做的结果是我们可以**根据不同的容器和不同的类型**，用一个类模板实现出来，进而对这些容器进行测试等操作。

- 等价定义也可以这样写，结合模板别名（alias template）：

~~~c++
template<typename T>
using Vec = vector<T, allocator<T>>;

template<class T,
		template<class> class Container
>
class XCL { ... };

int main(){
    XCL<int, Vec> tmp; //改变的是：现在定义变量要这样写
    return 0;
}
~~~

- 当然函数也可以这样用，只不过函数传参要求的是实例，而不是类型名。这当然不一样。

~~~c++
template<class T,
		template<class,
				 class = allocator<T>
		>
		class Container
>
void fun(T type, Container<T> con) {
	con.push_back(type); cerr << "Surely OK!\n";
}

//main 函数中变为这样
fun(3, vector<int>());
~~~

### - 考虑置入而非插入

**置入函数使用的是直接初始化，而插入函数使用的是复制初始化.这是下面所有讨论的根本原因**

假如有如下代码`std::vector<string> vs; vs.push_back("xyzzy")`，这个代码将会构造 2 次，析构一次。用`vs.emplac_back("xyzzy")`代替则只会发生一次构造。

详细过程如下：

1. 从字符串字面值”xyzzy“出发，创建 string 的临时对象 temp，这是一次构造调用；
2. temp 是个右值，他被传递给 push_back 的右值版本，而后会在 vector 预留给该对象的内存中构造它的一个副本；
3. 当 push_back 返回时，temp 被析构。

**原理：**

​	因为字符串字面量并不是 string 类型，而是 const char* ，所以要先经过 string 构造函数转换，才能够被 push_back 在 vector 所拥有的内存中构建。

但是 emplace_back 使用了完美转发，接受待插入对象的构造函数实参，**亦即它调用的是对象的构造函数**，因此只要不遭遇完美转发的限制，就可以传递任意型别和任意数量的实参。这使得它可以减少构造和析构的次数。

**适用场景：**

- **欲添加的值是以构造的方式而非赋值方式加入容器**
- 传递的实参与容器持有的类型性别不同（譬如上例，string 与 const char* 不一致）
- 容器不太可能由于重复而拒绝添加新的值（譬如 set，map 就会拒绝添加）

**特别注意：**

**由于 emplace_back 调用对象构造函数的特殊性**，会产生一些问题，可能导致错误代码通过编译，或者导致异常不安全。**此时使用 push_back 反而是最佳实践。**

- 例子 1

~~~c++
//Widget 是一个窗口类，killWidet 是绑定的析构函数
void killWidget(Widget* pWidget);
vector<std::shared_ptr<Widget>> ptrs;

ptrs.emplace_back(new Widget, killWidget);
~~~

考虑以下过程：从 new 出发返回裸指针被完美转发，并运行到 emplace_back 内为链表节点分配内容的的执行点。但是却因分配失败，抛出了异常。该异常传播到了 emplace_back 之外，但作为唯一可以获取到 Widget 内容的那个裸指针，却丢失了。这就发生了内存泄露。但是 push_back 没有这样的问题。

~~~C++
ptrs.push_back({new Widget, killWidget});
~~~

push_back 会按引用方式接受生成的 shared_ptr\<Widget\>对象，称之为 temp。而后 push_back 抛出异常，temp 被析构， 但是此时shared_ptr\<Widget\>对象会自动释放这个 Widget。

- 例子 2

~~~c++
vector<regex> test;

test.push_back(nullptr);//代码不能通过编译
test.emplace_back(nullptr);//代码可以通过编译，并且会引发未定义行为
~~~

原理是，regex 可以接受由 const char\* 出发从字符串构造的事实。nullptr 在被 emplace_back 构造的时候隐式转换为了一个const char*，为什么可以这样转换呢？（尽管在 regex 类中参数为const char\*的构造函数为 explicit）因为它使用直接初始化，而 push_back 则使用复制初始化。

>~~~c++
>string tmp1 = "xyzzz";//复制初始化
>string tmp2("hello");//直接初始化
>~~~
>
>对于使用等号初始化的语法，对应于复制初始化；对应于使用括号（或者花括号）的语法，对应于直接初始化。而复制初始化不允许调用 explicit 的构造函数，直接初始化则可以。

### - 智能指针的使用

**术语：空悬指针 dangling pointer**

**无论是哪种智能指针，都不应该指向栈对象！！即形如`std::shared_ptr<int> ptr = &a; `**

#### - std::unique_ptr

使用 unique_ptr 管理具备专属所有权的资源。unique_ptr 不允许被复制，移动一个 unique_ptr 会将资源所有权转移到目标指针，而源指针被置空。

- std::uniqueLptr 大小。

```text
1. 默认情况是 1 个指针的大小；
2. 使用自定义析构器时，若使用的时函数指针，则须加上函数指针的大小；
3. 使用的时函数对象时，增加的大小取决于该函数对象中存储了多少状态；
4. 使用的是 lambda 表达式时（lambda 是无状态对象），不会有额外大小。
```

- 使用方法：
    1. `std::unique_ptr<int> ptr(new int);`对 new 的对象使用默认析构器进行析构，即使用 delete 函数；
    2. `std::unique_ptr<YourType, Deleter_type> ptr(new YourType, function_pointer_type)` 对 new 的对象使用自定义析构器进行析构，传入的第二个参数可以是函数指针，也可以是函数对象，还可以是 lambda 表达式。**注意自定义析构器的类型要作为模板参数传入**；
    3. （了解）`std::unique_ptr<T[]>` 这样 unique_ptr 指涉的就是数组对象。

#### - std::shared_ptr

使用 std::shared_ptr 管理具备共享所有权的资源。我们先来看看 shared_ptr 的结构。

<img src = ".\images\shared_ptr.jpg" alt = "shared_ptr_structure" style = "zoom:20%" align="left">



- **shared_ptr 的尺寸是裸指针的两倍。**从结构中可以看出，它包含一个指涉到资源的裸指针，和一个指涉到引用计数的指针。
- **引用计数的内存必须动态分配。**从概念上说，引用计数与被指涉到的对象相关联，而被指涉到的对象却对此一无所知。因此他们没有存储引用计数的位置。值得一提的是，使用 std::make_ptr 创建 shared_ptr 可以避免动态分配的成本，但是有些场景是不推荐的。无论如何，引用计数都会作为动态分配的数据进行存储
- **引用计数的递增和递减必须是原子操作。**

#### - shared_ptr 的使用

- 使用方法和 unique 是大体一致的，只是多了些注意点。依然可以使用自定义析构器和默认析构器
- **注意在 unique_ptr 中，自定义析构器的类型是要作为模板参数一并传入的，而 shared_ptr 不用**

1. `std::shared_ptr<YourType> pw1(new YourType)`
2. ``std::shared_ptr<YourType> pw2(new YourType, function_pointer_type)`` 

#### - 复制和移动

​	复制会递增引用计数，而移动则不会，因为移动会”窃取资源所有权“。这也意味着移动操作比复制操作更快，因为复制要求递增引用计数，而引用计数是原子操作意味着会慢一些。

#### - 控制块

 - 直觉上来说，同一资源应该只用一个控制块控制即可，也即一个对象的控制块应该由首个指涉到该对象的 shared_ptr 来确定。

 - 何时会创建新的控制块呢？由于正在指涉到某对象的 shared_ptr 是无法知道是否有其它 shared_ptr 指涉到该对象，因此以下几种创建方法，会创建一个新的控制块。

    1. **std::make_shared 总是创建一个控制块；**

    2. **从具备专属所有权的指针 (unique_ptr, auto_ptr) 出发构建一个 shared_ptr 时，会创建一个控制块；**

        > auto_ptr 是 c++98 遗留的产物，在现代化 c++ 中，你不应该使用它

    3. **当使用裸指针作为 shared_ptr 构造函数实参调用时，也会创建一个控制块。**这会导致非常严重的问题，创建多个控制块，意味着对象会有多重引用计数，会被多次析构！！！

    ```c++
    // 下述代码会带你，坐上未定义行为的火箭
    auto p = new YourType;
    
    auto Deleter = [](YourType *pw){ delete pw; };	// Your_Deleter
    
    std::shared_ptr<YourType> spw1(pw, Deleter);	// 注意析构器型别不是
    std::shared_ptr<YourType> spw2(pw, Deleter);	// 智能指针型别的一部分
    ```

#### - 高级使用和注意事项

高级使用见《Effective Modern C++》P126 - P127

注意事项：一旦将资源的生存期托管给了 shared_ptr ，就不能再更改注意了。即使引用计数为一，也不能回收资源的所有权，并让一个 unique_ptr 管理他。因此 shared_ptr 管理资源是至死方休的。



### - std::weak_ptr

见《effective modern c++》



### - lambda函数的使用（还没写）

注意捕获的是**作用域内，非静态，局部变量**，本质是一个匿名类的匿名对象( godbolt.org 可以看汇编)，重载了 operator() 运算符。注意和 std::function 的区别！

#### C++11

#### C++14

- 初始化捕获，移动操作移入 lambda





