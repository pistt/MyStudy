//#pragma pack(16)
#include "mybits/stdc++.h"
#include <chrono>
#include <windows.h>
using namespace std;

template<class T>
class TD;

class Base {
public:
    Base():a(10);
    int a;
};

class Derived : Base { 
    void print() { cout << a; }
};


int main(){
    Derived().print();
    return 0;
}