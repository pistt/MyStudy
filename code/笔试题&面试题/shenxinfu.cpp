#include "../mybits/stdc++.h"
#include <Windows.h>
#include <cassert>
using namespace std;
template <class T>
class TD;
#define INFO(x) TD<decltype(x)> x##x##x;
/*
	复制到 '\0' 应该有这样的提示，理应还有这样的判断
	if ('\0' == *dst || '\0' == *src) {
		std::cerr << "Reach '\\0', interruput!\n";
		break;
	}
*/
// 实际上没考虑 src + count 越过 '\0' 的情况，只考虑 dst 是否到 '\0'
void* Mymemcpy(void* dst, void* src, int count) {
	char* __src = (char*)src;
	char* __dst = (char*)dst;
	assert(src != nullptr && dst != nullptr);

	// // #1 不完善版本，没考虑内存重叠
	// while (count--) {
	// 	*__dst++ = *__src++;
	// }

	// #2 完善版本，考虑 dst 和 src 有两种相对位置
	// __dst 的地址在前面，或者 __dst 虽然在后面，但是要复制的区域没有产生重叠
	// 从前往后复制，利用 && 运算符的左结合性
	if (__dst <= __src || __dst >= __src + count) {
		while (count-- && (*__dst++ = *__src++) != '\0');
	// 从后往前复制，利用 && 运算符的左结合性
	} else {
		while (count-- && (*(__dst + count) = *(__src + count)) != '\0');
	}

	return dst;
}

int main() {
	char test1[] = "pay attention";
    char test2[] = "something important";
    //char *test3 = nullptr;
    char test4[] = "pornhub";
    char test5[] = "youjizz";
	char* ptr1 = (char*)Mymemcpy(test1 + 3, test1, 5);
    char* ptr2 = (char*)Mymemcpy(test2, test2 + 6, 8);
    char* ptr3 = (char*)Mymemcpy(test4, test4 + 5, 1000);
    char* ptr4 = (char*)Mymemcpy(test5, test5 + 3, 11000);
	cout << ptr1 << endl << ptr2 << endl << ptr3 << endl << ptr4 << endl;
    return 0;
}