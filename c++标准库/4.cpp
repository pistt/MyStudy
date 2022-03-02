#include <bits/stdc++.h>
using namespace std;


class String{
public:
	String(){cout << "default\n";}
	String(const char* q):p(q){cout << "---String---" << endl;}
	String(const String&){}
	~String(){}

	const char* p;
};

class Word{
public:
	Word(const String& tmp):str(tmp){cout << "---Word---\n";}
	~Word(){};

	int cnt;
	String str;
};

// Word noun(static_cast<String>("book"));
// void foo(){
// 	Word cur = noun;
// 	cout << (noun.str.p) << endl;
// 	cout << (cur.str.p) << endl;
// 	system("pause");
// }

template<class T>
class TD;

int main(int argc, char const *argv[]){
	//printf("%s", (static_cast<String>("book").p));
	//foo();
	//cout << (Word(static_cast<String>("book")).str.p);
	Word noun("book");




	



	return 0;
}

