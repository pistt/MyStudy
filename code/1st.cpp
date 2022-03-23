#include "mybits/stdc++.h"
#include <Windows.h>
#include <memory>
using namespace std;
template <typename T>
class TD;
#define INFO(x) TD<decltype(x)> x##x##x

class Skill {
public:
	Skill(int _atk, int _last, int _cd) : atk(_atk), last(_last), cd(_cd) { }
	bool is_skill_cooldown() { return true; }

public:
	int atk;
	int last;
	int cd;
	time_t next_skill;
};

class Attack {
public:
	Attack(int _atk, int _cd) : atk(_atk), cd(_cd), next_time(clock()) { }
	bool is_atk_cooldown() { 
		if (clock() >= next_time) return true;
		else return false;
	}
	bool update_next_atk() { next_time = clock() + cd; return true; }

public:
	int atk;
	int cd;
	time_t next_time;
};

class Player {
public:
	Player(string _name, int hp) : HP(hp), myname(_name) { flag = false; }
	bool attack(Player* b) {
		if (b == nullptr || b -> HP <= 0) { cerr << "Your attack Player GG!\n"; return false; }

		if (_P_atk -> is_atk_cooldown() == false) {
			cout << myname << " atk is not cooldown!\n";
			return false;
		} else {
			b -> HP -= _P_atk -> atk;
			if (b -> HP <= 0) b -> die();
			_P_atk -> update_next_atk();
			cout << myname << " attacks " << b -> myname << " damage " << _P_atk -> atk << endl; 
			cout << b -> myname << " Hp now is " << b -> HP << " !\n";
			return true;
		}
	}
	bool is_die() { return flag; }
	void die() {
		flag = true;
		HP = 0;
		// _P_atk = nullptr;
		// _P_skill = nullptr;
	}

// private:
	bool flag;
	int HP;
	string myname;
	unique_ptr<Attack> _P_atk;
	//unique_ptr<Skill> _P_skill;
};



int main(int argc, char const *argv[]) {
	Player a("Blader", 2500), b("Coach", 2500);
	a._P_atk = make_unique<Attack>(50, 1);
	b._P_atk = make_unique<Attack>(50, 1);
	while (!a.is_die() && !b.is_die()) {
		a.attack(&b);
		b.attack(&a); 
	}
	if (a.is_die() && b.is_die()) cout << "All Loses\n";
	else if (a.is_die()) cout << b.myname << " Win!\n";
	else cout << a.myname << " Win!\n";
	



	return 0;
}

// #include <sys/wait.h>
// #include <sys/types.h>
// #include <arpa/inet.h>
// #include <netdb.h>
// #include <unistd.h>
