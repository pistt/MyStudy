#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <climits>
using namespace std;
void dfs(int left, int m, int n, int& sum);
int CalcCombineCount(int m, int n) {
	int sum = 0;
	dfs(1, m, n, sum);
	return sum;
}

void dfs(int left, int m, int n, int& sum) {
	if (n == 0) {
		++sum; return;
	}

	for (int i = left; i <= m && i <= n; ++i)
		dfs(i + 1, m, n - i, sum);
}

int main() {
	cout << 1000000000 % 3;
	cout << 2000000000 % 3;
	// int m = ;
	// int n = 120;
	// int dp[n + 1]; memset(dp, 0, sizeof(dp)); dp[0] = 1;
	
	// for (int i = 1; i <= m; ++i) {
	// 	for (int j = n; j - i >= 0; --j) {
	// 		dp[j] += dp[j - i];
	// 	}
	// }
	// cout << dp[n] << "\t" << CalcCombineCount(m, n);
	// dp[m][n];
	// for(int i=1;i<=m;++i){
	// 	for(int j=0;j<=n;++j){
	// 		dp[i][j]=dp[i-1][j]+dp[i-1][j-i];
	// 	}
	// }


	return 0;
}