#include "../mybits/stdc++.h"
#include <Windows.h>
using namespace std;
template <class T>
class TD;
#define INFO(x) TD<decltype(x)> x##x##x;

void f(char a[100]) {
	cout << sizeof(a) << endl;
}

int main(int argc, char const *argv[]){
	char a[100];
	cout << sizeof(a) << endl;
	cout << sizeof(a + 3) << endl;

	f(a);





	return 0;
}


