#include "../mybits/stdc++.h"
#include <Windows.h>


using namespace std;

template <class T>
class TD;
#define INFO(x) TD<decltype(x)>


class A {
public:
	int mem1;
	char mem2;
	int mem3;
	void fun(void*) {

		cout << "hello";

	}
};


void f() {
	throw std::bad_alloc();
}

int main(int argc, char const *argv[]){
	auto p = async(f);
	try {
		p.get();
	} catch (std::exception& e) {
		cout << "cnm\n";
	}
	p.get();


	return 0;
}


