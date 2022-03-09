// #1 条件变量
class H2O {
public:
    int H;
    std::mutex lk;
    std::condition_variable cv;
    H2O():H(0){
    }

    void hydrogen(function<void()> releaseHydrogen) {
        
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        std::unique_lock<mutex> l(lk);
        cv.wait(l, [this]{ return H < 2;});
        releaseHydrogen();
        ++H;
        cv.notify_all();
    }

    void oxygen(function<void()> releaseOxygen) {
        
        // releaseOxygen() outputs "O". Do not change or remove this line.
        std::unique_lock<mutex> l(lk);
        cv.wait(l, [this]{ return H == 2;});
        releaseOxygen();
        H = 0;
        cv.notify_all();
    }
};

// #2 原子变量
class H2O {
public:
    atomic<int> H;
    H2O() {
        atomic_init(&H, 0);
    }

    void hydrogen(function<void()> releaseHydrogen) {
        while (H.load() >= 2) this_thread::yield();
        releaseHydrogen();
        ++H;
    }

    void oxygen(function<void()> releaseOxygen) {
        while (H.load() < 2) this_thread::yield();
        releaseOxygen();
        H.store(0);
    }
};

// #3 信号量
#include <semaphore.h>
class H2O {
public:
    sem_t H, O;
    int val;
    H2O() {
        val = 0;
        sem_init(&H, 0, 2);
        sem_init(&O, 0, 0);
    }

    void hydrogen(function<void()> releaseHydrogen) {
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        sem_wait(&H);
        releaseHydrogen();
        if (++val == 2) sem_post(&O);
    }

    void oxygen(function<void()> releaseOxygen) {
        // releaseOxygen() outputs "O". Do not change or remove this line.
        sem_wait(&O);
        releaseOxygen();
        if (val == 2) {
            val = 0;
            sem_post(&H);
            sem_post(&H);
        }
    }
};

// #4 利用 unique_lock 可移动的特性
// 也避开了调用 unlock 必须拥有锁的陷阱
class H2O {
public:
    std::mutex lock1, lock2;
    std::unique_lock<mutex> lk1, lk2;
    H2O():lk1(lock1), lk2(lock2){
        
    }

    void hydrogen(function<void()> releaseHydrogen) {
        
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        while (!lk1.owns_lock() && !lk2.owns_lock()) this_thread::yield();
        releaseHydrogen();
        if (lk1.owns_lock())
            auto p = move(lk1);
        else
            auto p = move(lk2);
    }

    void oxygen(function<void()> releaseOxygen) {
        while (lk1.owns_lock() || lk2.owns_lock()) this_thread::yield();
        // releaseOxygen() outputs "O". Do not change or remove this line.
        releaseOxygen();
        lk1 = unique_lock<mutex>(lock1);
        lk2 = unique_lock<mutex>(lock2);
    }
};