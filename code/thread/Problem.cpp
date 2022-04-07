#include "../mybits/stdc++.h"
#include <thread>
#include <windows.h>
using namespace std;

// 题目描述：给定一组已排序数据，已经按照 x 排序大小即 A.x < B.x ，问最大的 B.y - A.y 是多少 
class pii {
public:
    //pii() = default;
    pii(int _x, int _y):x(_x), y(_y) { }
    pii() = default;
    int x;
    int y;
    friend bool operator==(const pii& a, const pii& b);
    friend bool operator<(const pii& a, const pii& b);
};

bool operator<(const pii& a, const pii& b) {
    return a.x < b.x;
}

bool operator==(const pii& a, const pii& b) {
    return a.x == b.x;
}

class Solution {
public:
    // 暴力
    int minval(set<pii>& data) {
        int n = data.size();
        int ans = INT_MIN;
        vector<pii> help;
        for (auto & p : data) help.push_back(p);

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                ans = max(help[j].y - help[i].y, ans);
            }
        }


        return ans;
        return 0;
    }

    // 单调栈
    int minval2(set<pii>& data) {
        int n = data.size();
        int ans = INT_MIN;
        vector<pii> help;
        for (auto &p : data) help.push_back(p);

        deque<int> locate;
        for (int i = 0; i < n; ++i) {
            while (!locate.empty() && help[locate.back()].y >= help[i].y) {
                ans = max(help[locate.back()].y - help[locate.front()].y, ans);
                locate.pop_back();
            }

            locate.push_back(i);
        }

        ans = max(help[locate.back()].y - help[locate.front()].y, ans);
        return ans;
    }

    // 贪心直接保存一个 curmin
    int minval3(set<pii>& data) {
        int n = data.size();
        int ans = INT_MIN;
        vector<pii> help;
        for (auto &p : data) help.push_back(p);

        int curmin = help[0].y;
        for (int i = 1; i < n; ++i) {
            curmin = min(curmin, help[i - 1].y);
            ans = max(ans, help[i].y - curmin);
        }
        return ans;

    }
};


int main() {
    int testtime = 10;
    while (testtime--) {
        srand(time(nullptr));
        set<pii> problem;
        for (int i = 0; i < 1000; ++i)
            problem.insert({rand() % 10000, rand() % 10000});
        

        Solution q;
        cout << "------Testtime : " << testtime << "------\n";
        cout << (q.minval(problem) == q.minval2(problem)) << endl;
        cout << (q.minval(problem) == q.minval3(problem)) << endl;
    }

    

    return 0;
}