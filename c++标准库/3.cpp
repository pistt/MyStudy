#include <bits/stdc++.h>
using namespace std;

class Screen{
public:
	Screen(int x) :i(x){}
	int get(){return i;}

	void* operator new(size_t);
	void operator delete(void*, size_t);
	static Screen* freeStore;
	Screen* next;
private:
	static const int screenChunk;
private:
	int i;
};
Screen* Screen::freeStore = 0;
const int Screen::screenChunk = 24;

void* Screen::operator new(size_t size){
	Screen *p;
	if (!freeStore){
		cout << "new now!!!!!\n";
		size_t chunk = screenChunk * size;
		cout << "chunk:" << chunk << endl;
		freeStore = p = reinterpret_cast<Screen*>(new char[chunk]);
		for (; p != &freeStore[screenChunk - 1]; ++p){
			//cout << "p:" << p << "\t" << "freeStore:" <<freeStore << endl;
			p -> next = p + 1;
		}
		p -> next = 0;
	}
	p = freeStore;
	freeStore = freeStore -> next;
	cout << "p:" << p << "\t" << "freeStore:" <<freeStore << endl;
	return p;
}

void Screen::operator delete(void* p, size_t){
	(static_cast<Screen*>(p)) -> next = freeStore;
	freeStore = static_cast<Screen*>(p);
}

int main(int argc, char const *argv[])
{
	int n = 208;
	Screen *p[n];
	cout << n << "\t" << sizeof(Screen) << endl;
	for (int i = 0; i < n; ++i){
		p[i] = new Screen(i);
		cout << p[i] << endl;
	}
	cout << "freeStore:" << Screen::freeStore << endl;
	cout << "freeStore->next:" << Screen::freeStore -> next << endl;
	cout << "!freeStore:" << !Screen::freeStore << endl;
	cout << "\n---------------\n";

	Screen *q[n];
	cout << n << "\t" << sizeof(Screen) << endl;
	for (int i = 0; i < n; ++i){
		q[i] = ::new Screen(i);
		cout << q[i] << endl;
	}


	return 0;
}