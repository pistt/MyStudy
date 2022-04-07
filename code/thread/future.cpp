#include "../mybits/stdc++.h"
#include <thread>
#include <windows.h>
using namespace std;

long long mygcd(long long a, long long b) {
    return b == 0 ? a : mygcd(b, a % b);
}

void dosth() { Sleep(1000); }

void func(std::promise<long long>& pro) {
    dosth();
    srand(time(nullptr));
    pro.set_value(mygcd(rand() % 10000 + 100, rand() % 10000 + 100));
}


int main() {
    std::promise<long long> hel;
    auto fu = hel.get_future();
    std::thread t(func, std::ref(hel));
    t.detach();

    while (true) {
        auto p = fu.wait_for(std::chrono::milliseconds(100));
        if (p == std::future_status::timeout)
            cout << "Quick\n";
        else if (p == std::future_status::deferred) {
            cout << "deferred\n";
            fu.wait();
        }
        else {
            cout << "gcd is " << fu.get() << endl;
            break;
        }

    }



    return 0;
}