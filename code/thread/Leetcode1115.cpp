// #1 atomic
class FooBar {
private:
    int n;
    atomic<bool> flag;
public:
    FooBar(int n){
        this->n = n;
        atomic_init(&flag, false);
    }

    void foo(function<void()> printFoo) {
        
        for (int i = 0; i < n; i++) {
            while (flag.load() == true) this_thread::yield();
        	// printFoo() outputs "foo". Do not change or remove this line.
        	printFoo();
            flag.store(true);
        }
    }

    void bar(function<void()> printBar) {
        
        for (int i = 0; i < n; i++) {
            while (flag.load() == false) this_thread::yield();
        	// printBar() outputs "bar". Do not change or remove this line.
        	printBar();
            flag.store(false);
        }
    }
};

// #2 条件变量
class FooBar {
private:
    int n;
    bool flag;
    std::mutex lock;
    std::condition_variable locker;

public:
    FooBar(int n) {
        this->n = n;
        flag = true;
    }

    void foo(function<void()> printFoo) {
        unique_lock<mutex> lk(lock);
        for (int i = 0; i < n; i++) {
            locker.wait(lk, [this]{ return flag; });
            // printFoo() outputs "foo". Do not change or remove this line.
            printFoo();
            flag = false;
            locker.notify_one();
        }
    }

    void bar(function<void()> printBar) {
        unique_lock<mutex> lk(lock);
        for (int i = 0; i < n; i++) {
            locker.wait(lk, [this]{ return !flag; });
            // printBar() outputs "bar". Do not change or remove this line.
            printBar();
            flag = true;
            locker.notify_one();
        }
    }
};

// #3 信号量
#include <semaphore.h>
class FooBar {
private:
    int n;
    sem_t pv1, pv2;
public:
    FooBar(int n) {
        this->n = n;
        sem_init(&pv1, 0, 0);
        sem_init(&pv2, 0, 1);
    }

    void foo(function<void()> printFoo) {
        
        for (int i = 0; i < n; i++) {
            // printFoo() outputs "foo". Do not change or remove this line.
            sem_wait(&pv2);
            printFoo();
            sem_post(&pv1);
        }
    }

    void bar(function<void()> printBar) {
        
        for (int i = 0; i < n; i++) {
            // printBar() outputs "bar". Do not change or remove this line.
            sem_wait(&pv1);
            printBar();
            sem_post(&pv2);
        }
    }
};

// #4 unique_lock
// 比较取巧，利用了 unique_lock 的移动特性，不断地转移锁的所有权。
class FooBar {
private:
    int n;
    mutex lock1, lock2;
    unique_lock<mutex> lk1, lk2;

public:
    FooBar(int n):lk1(lock1, std::defer_lock), lk2(lock2){
        this->n = n;
    }

    void foo(function<void()> printFoo) {
        for (int i = 0; i < n; i++) {
            while (lk1.owns_lock()) this_thread::yield();
            printFoo();
            auto p = move(lk2);
            lk1 = move(unique_lock<mutex>(lock1));
        }
    }

    void bar(function<void()> printBar) {
        
        for (int i = 0; i < n; i++) {
            while (lk2.owns_lock()) this_thread::yield();
            printBar();
            auto p = move(lk1);
            lk2 = move(unique_lock<mutex>(lock2));
        }
    }
};

// #5 promise 别学，没用。不是用来解决这种场景问题的
// 请见别人的题解：https://leetcode-cn.com/problems/print-foobar-alternately/solution/c-duo-fang-fa-by-zhouzihong-zdvj/