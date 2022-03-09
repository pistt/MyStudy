// #1 Atomic
class ZeroEvenOdd {
private:
    int n;
    atomic<int> num;
public:
    ZeroEvenOdd(int n){
        this->n = n;
        atomic_init(&num, 0);
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        for (int i = 1; i <= n; ++i) {
            while (num.load() != 0) this_thread::yield();
            printNumber(0);
            if (i % 2)
                num.store(1);
            else
                num.store(2);
        }
    }

    void odd(function<void(int)> printNumber) {
        for (int i = 1; i <= n; i += 2) {
            while (num.load() != 1) this_thread::yield();
            printNumber(i);
            num.store(0);
        }
    }

    void even(function<void(int)> printNumber) {
        for (int i = 2; i <= n; i += 2) {
            while (num.load() != 2) this_thread::yield();
            printNumber(i);
            num.store(0);
        }
    }
};

// #2 条件变量
class ZeroEvenOdd {
private:
    int n;
    int num;
    std::mutex lk;
    std::condition_variable cv;

public:
    ZeroEvenOdd(int n) {
        this->n = n;
        num = 0;
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        unique_lock<mutex> locker(lk);
        for (int i = 1; i <= n; ++i) {
            cv.wait(locker, [this]{ return num == 0; });
            printNumber(0);
            num = (i % 2) ? 1 : 2;
            cv.notify_all();
        }
        
    }

    void odd(function<void(int)> printNumber) {
        unique_lock<mutex> locker(lk);
        for (int i = 1; i <= n; i += 2) {
            cv.wait(locker, [this]{ return num == 1; });
            printNumber(i);
            num = 0;
            cv.notify_all();
        }
        
    }

    void even(function<void(int)> printNumber) {
        unique_lock<mutex> locker(lk);
        for (int i = 2; i <= n; i += 2) {
            cv.wait(locker, [this]{ return num == 2; });
            printNumber(i);
            num = 0;
            cv.notify_all();
        }
    }
};

// #3 信号量
#include <semaphore.h>
class ZeroEvenOdd {
private:
    int n;
    sem_t zeros, odds, evens;
public:
    ZeroEvenOdd(int n) {
        this->n = n;
        sem_init(&zeros, 0, 1);
        sem_init(&odds, 0, 0);
        sem_init(&evens, 0, 0);
    }

    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        for (int i = 1; i <= n; ++i) {
            sem_wait(&zeros);
            printNumber(0);
            if (i % 2)
                sem_post(&odds);
            else
                sem_post(&evens);
        }
        
    }

    void odd(function<void(int)> printNumber) {
        for (int i = 1; i <= n; i += 2) {
            sem_wait(&odds);
            printNumber(i);
            sem_post(&zeros);
        }
        
    }

    void even(function<void(int)> printNumber) {
        for (int i = 2; i <= n; i += 2) {
            sem_wait(&evens);
            printNumber(i);
            sem_post(&zeros);
        }
    }
};

