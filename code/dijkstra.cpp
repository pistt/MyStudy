// #include "mybits\stdc++.h"
// #include "windows.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cctype>
#include <queue>
#include <stack>
#include <cstring>
using namespace std;

template<class T>
class TD;


struct Edge {
	Edge(long long vv, long long costcost) : to(vv), cost(costcost){ }
	long long to;
	long long cost;
};

int main(int argc, char const *argv[]) {
	long long n, m, s;
	cin >> n >> m >> s;
	
	long long vertexs = n, start = s;
	long long path[n + 1], dis[n + 1];
	bool vis[n + 1];
	memset(vis, 0, sizeof(vis));
	memset(dis, 0x3f, sizeof(dis));
	memset(path, 0xff, sizeof(path));
	vis[start] = true;
	dis[start] = 0;
	vector<Edge> graph[n + 1];

	while (m--) {
		int st;
		cin >> st >> n >> s;
		graph[st].push_back({n, s});
	}

	for (int i = 0; i < (int)graph[start].size(); ++i) {
		Edge& k = graph[start][i];
		dis[k.to] = min(k.cost, dis[k.to]);
		path[k.to] = start;
	}



	for (int i = 1; i <= vertexs; ++i) {

		int v = -1;
		for (int j = 1; j <= vertexs; ++j)
			if (!vis[j] && (v == -1 || dis[j] < dis[v]))
				v = j;

		if (v == -1) break;
		vis[v] = true;

		for (int j = 0; j < (int)graph[v].size(); ++j) {
			Edge& k = graph[v][j];
			if (!vis[k.to] && dis[v] + k.cost < dis[k.to]) {
				dis[k.to] = dis[v] + k.cost;
				path[k.to] = v;
			}
		}
	}

	for (int i = 1; i <= vertexs; ++i)
		if (dis[i] == 0x3f3f3f3f3f3f3f3f) 
			cout << 0x7fffffff << " ";
		else 
			cout << dis[i] << " ";



	return 0;
}