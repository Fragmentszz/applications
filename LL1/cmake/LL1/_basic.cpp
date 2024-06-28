#include"_baisc.h"
vector<int> bfs(vector<int> I, vector<vector<int>> g)
{
	int n = g.size();
	vector<bool> vis(n, 0);
	queue<int> q;
	for (int u : I)	if (u >= 0) q.push(u);
	vector<int> ans;
	while (!q.empty())
	{
		int f = q.front(); q.pop();
		if (vis[f])	continue;
		vis[f] = 1;
		ans.push_back(f);
		for (int v : g[f])
		{
			if (v < 0)	continue;
			if (!vis[v])	q.push(v);
		}
	}
	return ans;
}