/* The example of using c++ stl which includes 6 parts:
 * container, allocator, iterator, algorithm, adapter
 * and functors.
 */
#include <bits/stdc++.h>
using namespace std;
int main(int argc, char** argv){
	int ia[6] = {23,34,23,4,41,34};
	/* vector<> -> container
	 * allocator<> -> allocator
	 * conut_if() -> algorithm
	 * not1() -> function adapter(negator)
	 * bind2nd() -> function adapter(binder)
	 * less<> -> function obeject
	 */
	vector<int,allocator<int>> vi(ia, ia + 6);
	cout << count_if(vi.begin(), vi.end(), not1(bind2nd(less<int>(),40)));
	return 0;
}