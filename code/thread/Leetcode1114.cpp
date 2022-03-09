#include <iostream>
#include <functional>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
using namespace std;

// #1 atomic
class Foo {
public:
    atomic<bool> ready1, ready2;
    Foo():ready1(false), ready2(false){
    }

    void first(function<void()> printFirst) {
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        ready1.store(true);
    }

    void second(function<void()> printSecond) {
        // printSecond() outputs "second". Do not change or remove this line.
        
        while (ready1.load() == false) this_thread::yield();
        printSecond();
        ready2.store(true);

    }

    void third(function<void()> printThird) {
        // printThird() outputs "third". Do not change or remove this line.
        
        while (ready2.load() == false) this_thread::yield();
        printThird();
    }
};

// #2 condition_variable
class Foo {
public:
    int k;
    std::mutex lock;
    std::condition_variable lk;
    Foo():k(1){
    }

    void first(function<void()> printFirst) {
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        ++k;
        lk.notify_all();
    }

    void second(function<void()> printSecond) {
        // printSecond() outputs "second". Do not change or remove this line.
        unique_lock<mutex> locker(lock);
        lk.wait(locker, [&]{ return k == 2; });
        printSecond();
        ++k;
        lk.notify_all();
    }

    void third(function<void()> printThird) {
        // printThird() outputs "third". Do not change or remove this line.
        unique_lock<mutex> locker(lock);
        lk.wait(locker, [&]{ return k == 3; });
        printThird();
    }
};

// #3 mutex 版本
// 都是利用 unique_lock 的移动特性，使得 mutex 可以在多个函数中转移
// mutex 只能在拥有 lock 的线程中 unlock，否则是 UB 未定义行为
// #3.1
class Foo {
public:
    std::mutex lk1, lk2;
    std::unique_lock<mutex> l1, l2;
    Foo():l1(lk1),l2(lk2) {
        
    }

    void first(function<void()> printFirst) {
        // printFirst() outputs "first". Do not change or remove this line.
        unique_lock<mutex> hl1(move(l1));
        printFirst();
    }

    void second(function<void()> printSecond) {
        lock_guard<mutex> l(lk1);
        unique_lock<mutex> hl2(move(l2));
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
    }

    void third(function<void()> printThird) {
        lock_guard<mutex> l(lk2);
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }
};

//#3.2
class Foo {
public:
    mutex lk1, lk2;
    unique_lock<mutex> locker1, locker2;
    Foo():locker1(lk1),locker2(lk2){
        
    }

    void first(function<void()> printFirst) {
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        auto l = move(locker1);
    }

    void second(function<void()> printSecond) {
        while (locker1.owns_lock()) this_thread::yield();
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        auto l = move(locker2);
    }

    void third(function<void()> printThird) {
        while (locker2.owns_lock()) this_thread::yield();
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }
};

// #4 Linux 信号量
#include <semaphore.h>
class Foo {
public:
    sem_t one, two;
    Foo() {
        // int sem_init __P ((sem_t *__sem, int __pshared, unsigned int __value));
        // __pshared 为 0 时代表只能当前进程使用，否则所有进程共享
        // __value 信号量代表初始值
        // 成功时返回 0, 失败时返回 -1 并且设置 errno
        sem_init(&one, 0, 0);
        sem_init(&two, 0, 0);
    }
    ~Foo() {
        sem_destroy(&one);
        sem_destroy(&two);
    }

    void first(function<void()> printFirst) {
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        sem_post(&one);
    }

    void second(function<void()> printSecond) {
        sem_wait(&one);
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        sem_post(&two);
    }

    void third(function<void()> printThird) {
        sem_wait(&two);
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }
};

// #5 promise & future
class Foo {
public:
    promise<void> p1,p2;
    Foo() {
        
    }

    void first(function<void()> printFirst) {
        
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        p1.set_value_at_thread_exit();
    }

    void second(function<void()> printSecond) {
        auto future1 = p1.get_future();
        future1.get();
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        p2.set_value_at_thread_exit();
    }

    void third(function<void()> printThird) {
        auto future2 = p2.get_future();
        future2.get();
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }
};