#include <bits/stdc++.h>
using namespace std;
inline void operator delete(void* p){
	cout << "global delete!\n";
}

class Bad{};
class test{
public:
	static void* operator new(size_t size){cerr << "new test\n"; return malloc(size);}
	static void operator delete(void*){cerr << "delete test\n";}
	~test(){cerr << "-----Des-----\n";}

};
class demo{
public:
	demo(){
		test *ptr = new test;
		cerr << "Success!\n"; 
		throw Bad();
	}

	static void* operator new(size_t size){
		cerr << "new demo\n"; 
		return malloc(size);
	}

	static void operator delete(void*, size_t){
		cerr << "demo\n";
	}

	~demo(){
		cerr << std::uncaught_exception() << endl;
		cerr << "-----delete demo-----\n";
	}
};


class Test {
public:
	Test(){cout << "Init...\n";}
	Test(const Test&) {cout << "Copy\n";}
	Test(Test&&) {cout << "Move\n";}
	~Test(){cout << "Destruct...\n";}
	void doo(){}
};

void C(Test& mm) {
	cout << (void*)&mm << endl;
	mm.doo();
}


void B(Test& mm) {
	cout << (void*)&mm << endl;
	mm.doo();
	C(mm);
}

void A(Test& mm) {
	cout << (void*)&mm << endl;
	mm.doo();
	B(mm);
}




int main(int argc, char const *argv[]){
	auto p = Test();
	A(p);
	return 0;
}


