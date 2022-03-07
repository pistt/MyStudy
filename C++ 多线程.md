

[TOC]

# 多线程

## C++11 STD

### thread

- 基本用法

直接传入函数名

```c++
void f() { cout << "hello world\n"; }
std::thread my_thread(f);
// output:19
```



std::thread 可以和任何可调用 (callable) 类型一同工作。

```c++
class task {
public:
    void operator()() const { /* do something */ }
};

//  #1
task f;
std::thread my_thread(f);

/*
	#2
	equals to funciton declaration
    std::thread my_thread(task (*)())
    "task (*)()" means that it is a function pointer that
    has no parameters and returns a task object
*/
std::thread my_thread(task());

//	#3
std::thread my_thread((task()));	// one more bracket than #2
std::thread my_thread{task()};		// or using {}
```

在 #1 中，函数对象被复制到新创建线程的存储器中，并从那里调用。**重要的是，副本和原版应该有着等效的行为，否则结果可能和预期不符合。**在 #2 中，并不是想像中的那样启动了一个线程，**而是声明了一个函数**，函数接受一个参数(类型是一个函数指针，它不接受参数，并返回 task 对象)，并返回 std::trhread。在 #3 中，才是正确的使用方法，或者用 #1 的方法也可以。





- 传递参数——**具体传参调用移动构造和复制构造的次数，还需要具体问题具体分析**

**重要的是，参数默认是以复制的方式传入线程。除非你知道你在干什么，不然应该坚持用复制（值传递）的方式传递参数给线程。**就算参数是引用类型，std::thread 也会复制而非引用，如果坚持使用引用，需要用 std::ref(param) 包裹住你的参数传递给 std::thread，这一点和 std::bind 类似。

```c++
// 有问题的玩法
// thread 能够接受可兼容的参数
// 什么叫可兼容：尽管该参数可能和函数实参类型不匹配
// 但是从该参数出发，最终仍能够构造函数实参对象
// 那么我们称之为它是一个可兼容的参数
void fun(std::string const& s);
void do_sth() {
    char buffer[1024] = "hello world\n";
    std::thread my_thread(f, buffer);
    t.detach();
}
/*
	参数会被以 char* 的形式，传递给线程 my_thread。
	哦豁完蛋，这时候 do_sth 完成了任务，buffer 被析构
	char* 指向了一个被释放的内存区域
	这时候线程试图从这个被释放的区域中，构造一个 string
	引发了未定义行为（尽管很少数时候确实可以成功）
*/

// 正确的做法
void fun(std::string const& s);
void do_sth() {
    char buffer[1024] = "hello world\n";
    std::thread my_thread(f, string(buffer));
    t.detach();
}
```





- 深入的讨论，当我们用正确的做法传递参数的时候，会发生什么？用一个类去探索一下

```c++
class BB {
public:
	BB(char*){ cout << "Con By Char*...\n"; }
	BB(){ cout << "Con...\n"; }
// private:
	BB(const BB& tmp) { cout << "Copy...\n"; }
public:
	BB(BB&& tmp) { cout << "Move...\n"; }
};

// 值传递
void f(BB tmp) { }
// 右值传递
void f(BB&& tmp) { }
// 引用传递
void f(BB& tmp) { }
```

1. 研究发现，当函数参数是**值传递**或**右值传递**时，都需要一个**移动构造函数**。否则 std::thread::_Invoker 将会报错。当函数参数是引用，而初始化的时候也使用了 std::ref 包裹，那么甚至不需要移动和复制构造函数都可以。
2. 调用移动构造和复制构造的次数。当函数参数是值传递时，调用移动构造函数3次（或1次复制构造，2次移动构造），右值传递时，调用2次(或1次复制构造，1次移动构造)。（我的测试如此，具体情况仍需具体分析）

> 参考链接:
>
> 1.https://www.cnblogs.com/yanghh/p/12965858.html
>
> 2.https://blog.csdn.net/weixin_44517656/article/details/111936910



> 传递参数的方式(2次传参)
>
>   **a.** **第一次传参(向 std::thread 构造函数传参)：**在创建thread对象时，std::thread构建函数中的所有参数均会**按值并以副本的形式**保存成一个tuple对象。
>
>    该tuple由调用线程（一般是主线程）在堆上创建，并交由子线程管理，在子线程结束时同时被释放。
>
>    注：**如果要达到按引用传参的效果，可使用std::ref来传递。**
>
>   **b. 第二次传参(向线程函数的传参)：**由于std::thread对象里保存的是参数的副本，为了效率同时兼顾一些只移动类型的对象，所有的副本均被
>
>    std::move到线程函数，即以右值的形式传入，所以最终传给线程函数参数的均为右值。



- 传递参数——成员函数

这时候第一个参数是成员函数指针，第二个参数是具体的对象地址，第三个参数及之后是调用的成员函数接受的参数。

```c++
class X {
public:
	void do_sth(auto&&... args) { };    
};

X my_x;
std::thread t(&X::do_sth, &my_x, args...)
```



- 常用函数

```c++
join()		// 结合线程
detach()	// 分离线程
swap()		// 交换线程id
get_id()	// 获取线程id
joinable()	// 线程是否可结合，是返回 true，否则返回 false
```



- useful example

thread 的移动特性，使得我们可以像下述代码这样，生成一批线程，然后等待它们完成。

使用 vector 管理线程，这是我们迈向自动化管理的第一步。

```c++
void do_work(unsigned id);
void f() {
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < 20; ++i) {
        threads.push_back(std::thread(do_work, i));	//生成线程
    }
    // 轮流在每个线程上调用 join
    // for_each 用法，第三个参数是 Function
    // mem_fn 是 member_function 的意思
    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join))
}
```



### thread 和 this_thread 命名空间





## future，promise 和 package_task

### future，shared_future

### promise

### package_task









## 锁

### mutex

**只能在同一个线程中，调用 lock 和 unlock，否则是 UB**

### recursive_mutex

### lock_guard

### unique_lock



## 条件变量

### condition_variable 和 condition_variable_any





## 时钟

### std::chrono





