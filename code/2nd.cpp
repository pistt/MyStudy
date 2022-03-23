/*
    模拟 LRU 是吧
*/
#include "mybits/stdc++.h"
#include <Windows.h>
using namespace std;

template <typename T> class TD;
#define INFO(x) TD<decltype(x)> x##x##x;

class Event { };
using EventCallBackFn = std::function<void(Event&)>;

class Test : public Event {
public:
	struct Wdata;
	void init() { 
		Wdata data;

		Event tmp;
		data.EventCallBack(tmp);
	}

	struct Wdata {
		int ccctypes;
		EventCallBackFn EventCallBack;
	};

	Wdata m_data;
};

// https://blog.csdn.net/alexhu2010q/category_10165311.html



int main(){
	{

	}
	 


    return 0;
}


// char* strcpy(char* dst, const char* src) {
// 	assert(dst == NULL);
// 	assert(src == NULL);

// 	if (src == NULL) return NULL;

	
// 	int size = strlen(src) + 1;
// 	// 存在内存重叠
// 	if (dst <= src + size || dst + size >= src) {

// 	} else {

// 	}
// 	return nullptr;
// }

