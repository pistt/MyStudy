#include <iostream>
using namespace std;
/*
	-std option		__cplusplus
	-std=c++03		199711L
	-std=c++11		201103L
	-std=c++14		201402L
	-std=c++17		201703L
	-std=c++20		202002L
*/

class cat {
public:
	void run(int a) { cout << "cat run\n"; }
};

class dog {
public:
	void eat(int a) { cout << "dog eat\n"; }

};

class rabbit {
public:
	void eat(int a) { cout << "rabbit eat\n"; }
	void run(int a) { cout << "rabiit run\n"; }
};

template <class T>
concept has_eat = requires(T t, int a) { t.eat(a); };

template <class T>
concept has_run = has_eat<T> || requires(T t, int a) { t.run(a); };

template <has_eat T>
void Say(T t) {
	t.eat(0);
}

template <has_run T>
void Say(T t) {
	t.eat(0);
}