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


int main(int argc, char const *argv[]){

	// demo *ptr = new demo;
	return 0;
}


