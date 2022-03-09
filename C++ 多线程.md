

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



### thread 类和 this_thread 命名空间

- thread 类中的函数

```c++
std::thread::hardware_concurrency()	// 返回当前平台最大线程并行数量
									// 只是参考值，不保证准确。如果不明确，返回0
std::thread::id()					// 返回当前线程 id

```

- this_thread 命名空间常用函数

```c++
std::this_thread::get_id()			// 获取当前线程 id
std::this_thread::sleep_for(dur)	// 阻塞 dur 时间段
std::this_thread::sleep_until(tp)	// 阻塞直到时间点 tp
std::this_thread::yield()			// 建议释放控制，让出时间片以便重新调度
```

## async，future，promise 和 package_task

### async

- async 尝试将其所获得的函数立刻异步启动在一个分离线程中
- 传递给 async() 的可以是任何的可调用对象：即函数，成员函数，函数对象或 lambda 等，例如`std::async([]{.......})`

- 发射策略

```c++
std::launch::async;		// 异步马上发射
std::launch::deferred;	// 推迟直到调用 future.get() 或 future.wait() 才发射

// 用法，需要结合 future 使用
int f() { return 10; }
std::future<int> = std::async(std::launch::async, f);
int my_integer = future.get();
```



### future	

> future 只支持 move ，不支持 copy ，get() 返回一个 copy 除非是 future<T&> 形式才返回引用。

​	future 内部保存异步执行返回的结果，或者保存一个异常。当调用 get() 或 wait() 的时候，调用线程会阻塞直到结果返回，当发生了一个异常时，future 会立即就绪并保留这个异常。

- get()：future 内部使用了一个共享状态来存储执行结果。当调用一次 get() 取出结果之后，就不会再有和这个 future 相关联的共享状态了。第二次及之后调用 get() 将会抛出一个异常。在 gcc 上将会是下述这样。
```text
terminate called after throwing an instance of 'std::future_error'
  what():  std::future_error: No associated state
```


- get()：当运行产生异常时，这个异常也会被存储在 future 的共享状态中，直到调用 get() 时会把异常传播开去（异常传播具体是传值还是引用，不同编译器不同）。同样地，不能多次调用 get 函数。

```c++
// terminate called after throwing an instance of 'std::bad_alloc'
//   what():  std::bad_alloc
// 若不调用 get() 则会正常退出。
void f() { throw std::bad_alloc(); }
int main() {
	std::future<void> ptr = std::async(f);
	ptr.get();
    return 0;
}
```

- future 相关函数和状态

| 状态                    | 描述                                                         |
| ----------------------- | ------------------------------------------------------------ |
| future_status::ready    | 共享状态已经就绪，已设置值或异常                             |
| future_status::timeout  | 已经等待 rel_time，但共享状态未就绪                          |
| future_status::deferred | 共享状态延迟准备，直到调用 get 或 wait <br>注意 wait_for 和 wait_until 不会导致它开始准备 |

```c++
future.valid();				// 返回 bool 值，若结果准备完成返回 true 否则 false
							// 只有 true 时，你才能调用下面的函数
future.get();				// 获取结果，若结果未完成，会产生阻塞
future.wait();				// 等待结果，若结果未完成，会产生阻塞
future.wait_for(dur);
future.wait_until(tp);
future.share();				// 返回一个 shared_future 并令当前状态失效
```

- shared_future

和 future 不同的是，get() 返回的是一个 const reference，并且它支持拷贝语义，**允许多次使用 get()**，而且没有 share 函数。观察内部实现，不难理解，它们为什么表现出不同的行为，shared_future 内部使用 shared_ptr 实现，而 future 使用 unique_ptr 实现。



### promise

​	promise 主要用于两个线程之间通信。



### package_task

​	把任务封装进去给线程执行。

### call_once

```c++
std::once_flag oc;					// global flag
std::call_once(oc, initialize);		// initialize if not initialzed yet
std::call_once(oc, [] { });
// 原则上可以使用同一个 once_flag 调用不同的函数。之所以把 once_flag
// 当作第一实参传给 call_once() 就是为了确保传入的机能只被执行一次
```









## 锁

共有的

```c++
lock(m1, m2);		// 锁住 m1, m2 
try_lock();			// 可能假失败，由于内存次序原因
```



### mutex

> 更多详情可参考：https://blog.csdn.net/faihung/article/details/88411839

**调用 unlock 的线程必须拥有锁，否则是 UB**

###  shared_mutex(Since C++14)

> https://blog.csdn.net/princeteng/article/details/103952888

​	shared_mutex 类是一个同步原语，可用于保护共享数据不被多个线程同时访问。与便于独占访问的其他互斥类型不同，**shared_mutex 拥有 2 个访问级别：共享 - 多个线程能共享同一互斥的所有权；独占性 - 仅一个线程能占有互斥。**

- 若一个线程已经通过**lock或try_lock获取独占锁（写锁）**，则无其他线程能获取该锁（包括共享的）。尝试获得读锁的线程也会被阻塞。
- **仅当任何线程均未获取独占性锁时，共享锁（读锁）才能被多个线程获取（通过 lock_shared 、try_lock_shared ）**。
- 在一个线程内，同一时刻只能获取一个锁（共享或独占性）。

　　成员函数主要包含两大类：排他性锁定（写锁）和共享锁定（读锁）。

```c++
lock();			// 排他性锁
try_lock();		// 排他性锁
unlock();		// 解除互斥

// -------------------
lock_shared();	// 共享性锁，若互斥，则阻塞
try_lock_shared();//同上
unlock_shared();// 解锁互斥
```



### recursive_mutex

### lock_guard

​	lock_guard 其实是用 RAII 思想封装了的 mutex 这样子。很容易想到的是，如果上锁的函数执行过程中抛出了一个异常，那么这个异常后面的代码将不会执行，也就是说，后面释放 mutex 的代码不会执行，导致该资源一直被锁住。但是 lock_guard 可以避免这个问题，因为抛出异常后，程序会自动调用相对应代码段变量的析构函数，lock_guard 将在析构函数中释放 mutex.

​	另一个好处是，程序员可以摆脱手动控制锁的烦恼。

### unique_lock

​	对于 unique_lock 的对象来说，一个对象只能和一个 mutex 锁唯一对应，不能存在一对多或者多对一的情况，不然会造成死锁的出现。所以如果想要传递两个 unique_lock 对象对 mutex 的权限，需要运用到移动语义或者移动构造函数两种方法。



```c++
// Constructors
std::unique_lock<mutex> lock(mutex);
std::unique_lock<mutex> lock(mutex, tp);
std::unique_lock<mutex> lock(mutex, dur);
std::unique_lock<mutex> lock(mutex, std::defer_lock);

//假定当前线程已经获得互斥对象的所有权，所以不再请求锁
std::unique_lock<mutex> lock(mutex, std::adopt_lock);
std::unique_lock<mutex> lock(mutex, std::try_lock);

// Functions
swap(l1, l2);
release();			// release mutex and returns a pointer pointed to the mutex
owns_lock();		// if unique_owns it lock, return true
mutex();			// returns a pointer pointed to the mutex
if (unique_lock);	// 检查是否被锁定
lock();
try_lock();
try_lock_for(dur);
try_lock_until(tp);
unlock();
```





## 条件变量

### condition_variable 和 condition_variable_any

`notify_all_at_thread_exit(cv, ul)`



## 信号量 semaphore.h

```c++
/*
	__pshared 如果为0，只能该进程使用；否则进程间共享
	__value 初始信号量的值
	返回值：成功返回 0，否则返回 -1
	如无特殊说明，下面函数都是成功返回 0，失败返回 -1
*/
int sem_init(sem_t *__sem, int __pshared, unsigned int __value);
// 当前信号量的值，保存在 sval 中。若有 >=1 个线程在等待，返回等待的线程数量
int sem_getvalue(sem_t *sem, int *sval);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem); 
int sem_trywait(sem_t *sem);
int sem_destroy(sem_t *sem);
```

​	如果*pshared*不为零，则信号量在进程之间共享，并且应该位于共享内存的区域中（参见 ***[shm_open](https://linux.die.net/man/3/shm_open)** (3)*、***[mmap](https://linux.die.net/man/2/mmap)** (2)*和***[shmget](https://linux.die.net/man/2/shmget)** (2)*）。（由于***[fork](https://linux.die.net/man/2/fork)** (2)*创建的子进程继承了其父进程的内存映射，它也可以访问信号量。）任何可以访问共享内存区域的进程都可以使用***[sem_post](https://linux.die.net/man/3/sem_post)** (3)*、 ***[sem_wait](https://linux.die.net/man/3/sem_wait)** (3)*等对信号量进行操作.



## 原子类型（请参考《C++并发编程实战》）

### 基本用法

原子类型需要硬件支持，显然，这意味着在不同平台上和不同硬件架构上，会有不同的成本。可以通过`atomic.is_lock_free()`查看内部是否需要使用 lock 实现，否则返回 true，是则返回 false。

- 一般而言，通过原子类型操作获得的一个 copy 而非一个 reference。

- **Default 构造函数，并未完全初始化 atomic 对象，需要显示地用 atomic_init() 初始化**

    ```c++
    std::atomic<bool> object;
    std::atomic_init(&object, false);
        
    // or init object like this, two ways have the same effect
    std::atomic<bool> object(false); // thii is not an atomic operaiton
    ```

- 接受相关类型值的那个构造函数并不是 atomic 的

- 所有函数除了构造函数，都被重载为 volatile 和 non-volatile 两个版本

- 常用的函数

    ```c++
    atomic.is_lock_free();
    atomic.store(val);
    atomic.load();
    atomic.exchange(val);
    atomic.compare_exchange_strong(exp, des);	// 请见官方文档
    atomic.compare_exchange_weak(exp, des);		// 请见官方文档
    atomic.operator atomic();					// 返回一个该类型的拷贝
    ```

    



### 原子类型高级用法

​	在使用原子类型的低级接口时，你应该确保自己了解你在干什么，并且了解内存顺序模型。并且你应该明白，在不同 CPU 架构上，这些模型依然有着不同的实现和成本。帮助你了解内存顺序的是，了解 CPU 的**指令重排**乃至编译器的**语句重排**，以及**内存屏障**。一般而言，底层通过利用 CPU 的 CAS(Compare and Swap) 指令实现缓存锁和总线锁，从而实现多处理器间的原子操作。

<img src = ".\images\CAS.png" alt="RVO实验结果2" style = "zoom:75%" align="left">



- 内存顺序

| 内存顺序模型名称 | 英文名              | c++对应                                                      |
| ---------------- | ------------------- | ------------------------------------------------------------ |
| 顺序一致顺序     | sequence consistent | memory_order_seq_cst                                         |
| 获得--释放顺序   | acquire-release     | memory_order_consume<br>memory_order_acquire<br>memory_order_release<br>memory_order_acq_rel |
| 松散顺序         | relaxed             | memory_order_relaxed                                         |



- 可供使用的常用函数——具体使用方法和效果，请参考官方文档和书籍

```c++
atomic.load(val, memory_order);
atomic.store(memory_order);
atomic.exchange(val, memory_order);
atomic.compare_exchange_strong(exp, des, mo);	// 还有另一个版本
atomic.compare_exchange_weak(exp, des, mo);    	// 还有另一个版本
```





## 时钟

有时间限制的等待。下面是一些接受超时的函数

> dur: duration
>
> tp: time_point
>
> pre: predicate

| 类/命名空间                                       | 函数                                                    | 返回值                                                       |
| ------------------------------------------------- | :------------------------------------------------------ | ------------------------------------------------------------ |
| this_thread 空间                                  | sleep_for(dur)<br>sleep_until(tp)                       | 不可用                                                       |
| condition_variable<br>condition_variable_any      | wait_for(lock, dur)<br>wait_until(lock, tp)             | std::cv_status::timeout<br>std::cv_status::no_timeout        |
|                                                   | wait_for(lock, dur, pre)<br>wait_until(lock, tp, pre)   | bool                                                         |
| mutex<br>recursice_timed_mutex                    | try_lock_for(dur)<br>try_lock_until(tp)                 | bool                                                         |
| unique_lock\<TimeLockable\>                       | unique_lock(lockable, dur)<br>unique_lock(lockable, tp) | 不可用--owns_lock()在新<br>构造对象上；如果获得了<br>锁返回 true， 否则false |
|                                                   | try_lock_for(dur)<br>try_lock_until(tp)                 | bool                                                         |
| future\<ValueType\><br>shared_future\<ValueType\> | wait_for(dur)<br>wait_until(tp)                         | future_status::timeout<br>future_status::ready<br>future_status::deferred |



### std::chrono

## 注意事项

### 线程假醒(spurious wakeup)

有两种说法，一种是 notify 引起的竞争，一种是系统实现问题，如系统错误返回的 EINTR 信号。解决线程假醒的方法，是把 if 判断改成 while 循环，让线程在醒来时再次判断条件。c++ 使用条件 wait 也可以`condition_variable.wait(lock, lambda_function)`

参考链接：[维基百科](https://en.m.wikipedia.org/wiki/Spurious_wakeup)，[知乎](https://www.zhihu.com/question/271521213)，[简书](https://www.jianshu.com/p/0eff666a4875)，[某博客](http://www.manongjc.com/detail/15-skvyvcwcmkgurum.html)



