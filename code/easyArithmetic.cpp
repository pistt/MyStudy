#include "mybits\stdc++.h"
#include "windows.h"
using namespace std;

template<class T>
class TD;

/* 
	基于文法规则实现的算术表达式，由于文法很简单，所以不再赘述关于
	提取左公因子，消除左递归，first 集合，follow 集合等 LL(1)
	需要解决的问题。直接给出文法
		
	普通形式：
	Exp 	-> Term + Exp | Term - Exp | Term
	Term 	-> Fac * Term | Fac / Term | Fac
	Fac		-> (Exp) | number

	扩展 EBNF：
	Exp 	-> Term { ( + | - ) Term }
	Term	-> Fac { ( * | / ) Fac }
	Fac		-> (Exp) | number
*/




enum addop { plus = '+', minus = '-'};
enum mulop { multi = '*', divide = '/'};
enum bracket { left = '(', right = ')'};
enum endop { end = '#'};

class EasyArithmetic {
public:
	EasyArithmetic(string expression) : token(endop::end), loc(0), exp(expression) { }
	void reset(string expression) { token = endop::end; loc = 0; exp = expression; }
	double result();
private:
	char token;
	int loc;
	string exp;


	double Exp(char& token);
	double Term(char& token);
	double Fac(char& token);
	void match(char expected_token);
	void gettoken();
	void error();
};

void EasyArithmetic::match(char expected_token) {
	if (token == expected_token) gettoken();
	else error();
}

void EasyArithmetic::gettoken() {
	if (loc == static_cast<int>(exp.size()))
		token = endop::end;
	else
		token = exp[loc++];
}

void EasyArithmetic::error() {
	std::cerr << "Error!\n";
	abort();
}

double EasyArithmetic::result() {
	if (exp.size() == 0) return 0;
	gettoken();
	return Exp(token);
}

double EasyArithmetic::Exp(char& token) {
	double value = Term(token);
	while (token == addop::plus || token == addop::minus) {
		if (token == addop::plus) {
			match(addop::plus);
			value += Term(token);
		} else {
			match(addop::minus);
			value -= Term(token);
		}  
	}
	return value;
}

double EasyArithmetic::Term(char& token) {
	double value = Fac(token);
	while (token == mulop::multi || token == mulop::divide) {
		if (token == mulop::multi) {
			match(mulop::multi);
			value *= Fac(token);
		} else {
			match(mulop::divide);
			value /= Fac(token);
		}
	}
	return value;

}

double EasyArithmetic::Fac(char& token) {
	double value = 0;
	if (token == bracket::left) {
		match(bracket::left);
		value = Exp(token);
		match(bracket::right);
	} else if (isdigit(token)) {
		while (isdigit(token)) {
			value = value * 10 + (token - '0');
			gettoken();
		}
	} else error();
	return value;
}



int main(int argc, char const *argv[]) {
	cout << EasyArithmetic(argv[1]).result();

	return 0;
}