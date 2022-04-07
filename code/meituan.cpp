#include <bits/stdc++.h>
using namespace std;
// int main() {
//     int n;
//     int task[n];
    
//     cin >> n;
//     for (int i = 0; i < n; ++i)
//         cin >> task[i];

    
    
//     return 0;
// }

// #1
// int main() {
//     int n,r,b;
//     string ball;
//     cin >> n >> r >> b >> ball;

//     int need_r = 0, need_b = 0;
//     size_t ballsize = ball.size();

//     for (int i = 0; i < ballsize; ++i) {
//         if (i > 0 && ball[i] == ball[i - 1]) {
//             if (ball[i] == 'r')
//                 ++need_b;
//             else
//                 ++need_r;
//         }
//     }

//     if (need_b <= b && need_r <= r)
//         cout << ballsize + need_b + need_r;
//     else
//         cout << max(0, need_r - r) << " " << max(0, need_b - b);

int main() {
    int n; cin >> n;
    vector<int> task(n, 0);
    for (int i = 0; i < n; ++i)
        cin >> task[i];
        
    sort(task.begin(), task.end(), [](int a, int b) { return a > b; });
    
    vector<long long> bucket(3, 0);
    
    for (int i = 0; i < n; ++i) {
        int update = 0;
        for (int j = 0; j < 3; ++j) {
            if (bucket[j] + task[i] < bucket[update] + task[i])
                update = j;
        }
        bucket[update] += task[i];
    }
    
    cout << *max_element(bucket.begin(), bucket.end());
    
    
    return 0;
}