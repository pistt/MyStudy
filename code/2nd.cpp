#include "mybits/stdc++.h"
#include <Windows.h>
#include <algorithm>
#include <functional>
using namespace std;

template <typename T> class TD;
#define INFO(x) TD<decltype(x)> x##x##x;

// https://blog.csdn.net/alexhu2010q/category_10165311.html

template <typename T>
void print(T&& arg) { cout << arg << " "; }

template <typename... T>
void func(T... args) {
	(print(args), ...);
}

template <typename... T>
decltype(auto) binder(T... args) {
	return bind(func<T...>, args...);
}


namespace std {
	namespace placeholders {
		extern const _Placeholder<111> _111{};
	}
}

int main() {
	int s_size;
	int p_size;
	string s;
	string patten;

	const int testtimes = 100;
	vector<vector<time_t>> timing(testtimes, vector<time_t>(3));
	for (int i = 0; i < testtimes; ++i) {
		srand(time(nullptr));
		s_size = 1e6;
		p_size = (rand() % 10000) * 1000;
		s.resize(s_size);
		patten.resize(p_size);

		for (int i = 0; i < s_size; ++i)
			s[i] = rand() % 26 + 'a';

		for (int i = 0; i < p_size; ++i)
			patten[i] = rand() % 26 + 'a';


		auto s_str = s.c_str(), p_str = patten.c_str();

		time_t cur = clock();
		s.find(patten);
		timing[i][0] = clock() - cur;

		cur = clock();
		strstr(s_str, p_str);
		timing[i][1] = clock() - cur;

		cur = clock();
		std::search(s.begin(), s.end(), std::boyer_moore_horspool_searcher(patten.begin(), patten.end()));
		timing[i][2] = clock() - cur;
	}


	cout << "find\tstrstr\tboyer\n";
	for (int i = 0; i < testtimes; ++i) {
		cout << timing[i][0] << "\t" << timing[i][1] << "\t" << timing[i][2] << endl;
	}
	

	 


    return 0;
}

