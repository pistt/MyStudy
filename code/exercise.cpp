#include "mybits\stdc++.h"
#include "windows.h"
using namespace std;

constexpr static size_t mod1 = 7;
constexpr static size_t mod2 = 8;
constexpr static size_t last1 = 2;
constexpr static size_t last2 = 5;


template <size_t N>
constexpr static bool is_exp = (N % mod1 == last1 && N % mod2 == last2);

template <bool check, size_t T>
constexpr static size_t AndVal = 0;

template <size_t T>
constexpr static size_t AndVal<true, T> = T;

template <size_t T>
constexpr static size_t AllOdd = AndVal<is_exp<T>, T + AllOdd<T - 1>>;

template <>
constexpr size_t AllOdd<0> = 0;



int main(int argc, char const *argv[]) {
	// cout << AllOdd<35> << endl;
	for (int i = 1; i < 1000; ++i)
		if (i % mod1 == last1 && i % mod2 == last2) cout << i << endl;

	return 0;
}

/*

35
154
273
392
511
630
749
868
987

*/