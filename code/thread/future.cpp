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

class BB {
public:
	BB(char*){ cout << "Con By Char*...\n"; }
	BB(){ cout << "Con...\n"; }
// private:
	BB(const BB& tmp) { cout << "Copy...\n"; }
// public:
private:
	BB(BB&& tmp) { cout << "Move...\n"; }

};

void f(BB tmp){}

int main(int argc, char const *argv[]){
	char buffers[1024] = "1234567890";
	BB heys;
	std::thread t(f, heys);
	t.detach();
	Sleep(1000);


	return 0;
}


