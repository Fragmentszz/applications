#include"Grammar.h"
#include<queue>
#include<sstream>

Grammar::Grammar(vector<char> vt, vector<string> vn, string s)
	: VT(vt), VN_o(vn), S_o(s) 
{
	for (int i=0;i<vn.size();i++)
	{
		ys_s2c[vn[i]] = 'A' + i;
		VN.push_back('A' + i);
		ys_c2s['A' + i] = vn[i];
	}
	S = ys_s2c[S_o];
	for (int i = 0; i < vt.size(); i++)
	{
		ys_T[vt[i]] = i;
	}
	for (int i = 0; i < vn.size(); i++)
	{
		ys_N[ys_s2c[vn[i]]] = i;
	}
	
} // 带参数的构造函数



Grammar::Grammar():S('\0') {} // 默认构造函数

vector<char> Grammar::getVT() const { return VT; } // 获取终结符集合

vector<string> Grammar::getVN() const { return VN_o; } // 获取非终结符集合

string Grammar::getS() const { return S_o; } // 获取开始符号

string Grammar::transc2s(string s)
{
	string res;
	for (char c : s)
	{
		if (ys_c2s.find(c) != ys_c2s.end())
			res += ys_c2s[c];
		else res.push_back(c);
	}
	if (res == "") return "epsilon";
	return res;
}

string Grammar::transs2c(string beta)
{
	string res;
	for (int i = 0; i < beta.length(); i++)
	{
		string tmp;
		while (i < beta.length() - 1 && beta[i + 1] == '\'') {
			tmp.push_back(beta[i++]);
		}
		tmp.push_back(beta[i]);
		if (ys_s2c.find(tmp) != ys_s2c.end())
			res += ys_s2c[tmp];
		else res += tmp;
	}
	return res;
}




/********************************************************************************************/

vector<int> merge(vector<int>& x, vector<int>& y)
{
	vector<int> res(x.size(), 0);
	for (int i = 0; i < x.size(); i++)
	{
		res[i] = x[i] | y[i];
	}
	return res;
}

bool isUpdated(vector<int>& x, vector<int>& y)
{
	for (int i = 0; i < x.size(); i++)
	{
		if (x[i] != y[i])	return 1;
	}
	return 0;
}


vector<vector<string>> ContextFreeGrammar::getys_SELECT()
{
	vector<vector<string>> res(VN.size());
	for (int i = 0; i < VN.size(); i++)
	{
		res[i].resize(VT.size() + 1);
		for (int j = 0; j <= VT.size(); j++)
		{
			if(ys_SELECT[i][j].empty())	continue;
			if (P[ys_SELECT[i][j][0]].second.empty())
				res[i][j] = "->epsilon";
			else
				res[i][j] = "->" + P[ys_SELECT[i][j][0]].second;
			for (int k = 1; k < ys_SELECT[i][j].size(); k++)
			{
				if (ys_SELECT[i][j][k] != -1)
					if (P[ys_SELECT[i][j][k]].second.empty())
						res[i][j] += ", ->epsilon";
					else
						res[i][j] += ", ->"+P[ys_SELECT[i][j][k]].second;
			}
		}
		//if (ys_SELECT[i][VT.size()] != -1)
		//	if (P[ys_SELECT[i][VT.size()]].second.empty())
		//		res[i][VT.size()] = "epsilon";
		//	else
		//		res[i][VT.size()] = P[ys_SELECT[i][VT.size()]].second;
		//else res[i][VT.size()] = "";
	}

	return res;
}

void ContextFreeGrammar::deleteHarmful()
{
	vector<pair<int, string>> tmp;
	for (int i = 0; i < P.size(); i++)
	{
		string ts;ts.push_back(VN[P[i].first]);
		if(ts == P[i].second)	continue;
		tmp.push_back(P[i]);
	}
	P = tmp;
}


void ContextFreeGrammar::deleteUseless()
{
	
	int n = VN.size();
	vector<vector<int>> g(n);
	for (int i = 0; i < P.size(); i++)
	{
		int idx = P[i].first; string beta = P[i].second;
		for (int j = 0; j < beta.length(); j++)
		{
			if (isNonTerminaled(beta[j]))
				g[idx].push_back(ys_N[beta[j]]);		//建图
		}
	}
	vector<int> reachable2 = bfs({ ys_N[S] }, g);			//可达的非终结符
	vector<bool> reachable(n, 0);
	for (int v : reachable2) 	reachable[v] = 1;
	vector<pair<int, string>> tmp;
	for (int i = 0; i < P.size(); i++)
	{
		if (reachable[P[i].first])				//左部可达的产生式保留，否则丢弃
			tmp.push_back(P[i]);
	}
	P = tmp;
	vector<int> left(P.size(), 0);
	vector<int> endable(n, -1);
	vector<vector<int>> g2(n);
	vector<int> in(n);

	for (int i = 0; i < tmp.size(); i++)
	{
		int idx = P[i].first;string beta = P[i].second;
		left[i] = beta.length();
		g2[idx].push_back(i);
		in[idx]++;
	}
	vector<int> isHave(tmp.size(), 1);
	bool flag = 1;
	while (flag)
	{
		flag = 0;
		for (int i = 0; i < tmp.size(); i++)
		{
			if (!isHave[i])	continue;
			bool allterminal = 1;
			int idx = tmp[i].first;
			string& beta = tmp[i].second;
			for (int j=0;j<beta.length();j++)
			{
				char c = beta[j];
				if (c == '\0')	continue;
				if (isNonTerminaled(c))                //如果是非终结符
				{
					allterminal = 0;
					if (endable[ys_N[c]] == 1)			//如果beta中的c可以终结
					{
						beta[j] = '\0';				//删除beta中的c
						left[i]--;
					}
				}
				else if (isTerminaled(c))		//如果是终结符
				{
					beta[j] = '\0';			//也直接删除
					left[i]--;
				}
			}
			if (left[i] <= 0)		//如果beta中的所有字符都被删除了
			{
				for (int v : g2[idx])	//说明该终结符A也可以终结
					isHave[v] = 0;			//删除所有以A为左部的产生式
				left[i] = 0;
				endable[idx] = 1;		//A可以终结
				in[idx] = 0;
				flag = 1;
			}
		}
	}
	vector<char> tVN;
	for (int i = 0; i < n; i++)
	{
		if(reachable[i] == 1 && endable[i] == 1)
			tVN.push_back(VN[i]);
	}
	vector<pair<int, string>> tmp2;
	for (int i = 0; i < P.size(); i++)
	{
		int idx = P[i].first;
		if (!(reachable[idx] == 1 && endable[idx] == 1))	continue;

		string beta = P[i].second;
		bool valid = 1;
		for (char c : beta)
		{
			if (isNonTerminaled(c) && endable[ys_N[c]] != 1)
			{
				valid = 0;
				break;
			}
		}
		if(valid)	tmp2.push_back(P[i]);
	}

	P = tmp2;
	map<char, int> ys_N2;
	for (int i = 0; i < tVN.size(); i++)
	{
		ys_N2[tVN[i]] = i;
	}
	for (int i = 0; i < P.size(); i++)
	{
		P[i].first = ys_N2[VN[P[i].first]];
	}
	VN = tVN;
	ys_N = ys_N2;
}

ContextFreeGrammar ContextFreeGrammar::compress()
{
	// print();
	deleteHarmful();
	deleteUseless();
	// print();
	
	FIRST.resize(VN.size());
	FOLLOW.resize(VN.size());
	SELECT.resize(P.size());
	ys_SELECT.resize(VN.size());
	for (int i = 0; i < VN.size(); i++)
	{
		FIRST[i].resize(VT.size(), 0);
		FOLLOW[i].resize(VT.size() + 1, 0);
		ys_SELECT[i].resize(VT.size() + 1);
	}
	for (int i = 0; i < P.size(); i++)
	{
		SELECT[i].resize(VT.size() + 1, 0);
	}
	


	return *this;
}

void ContextFreeGrammar::canGetEpsilon_graph()
{
	;
}

void ContextFreeGrammar::getFIRST_loop()
{
	vector<int> used(P.size(), 0);
	for (int i = 0; i < P.size(); i++)
	{
		int idx = P[i].first; string beta = P[i].second;
		if (beta.length() == 0) {
			used[i] = 1; continue;
		}
		if (ys_T.find(beta[0]) != ys_T.end())
		{
			FIRST[idx][ys_T[beta[0]]] = 1;
			used[i] = 1;
		}
	}
	bool flag = 1;
	while (flag)
	{
		flag = 0;
		for (int i = 0; i < P.size(); i++)
		{
			if (used[i])	continue;
			int idx = P[i].first; string beta = P[i].second;
			for (int j = 0; j < beta.size(); j++)
			{
				if (ifToEpsilon(beta[j]))
				{
					int idx2 = ys_N[beta[j]];
					vector<int> tmp = FIRST[idx];
					FIRST[idx] = merge(FIRST[idx], FIRST[idx2]);
					flag = isUpdated(FIRST[idx], tmp);
				}
				else
				{
					if (isTerminaled(beta[j]))
					{
						if (FIRST[idx][ys_T[beta[j]]] == 0)
						{
							flag = 1;
							FIRST[idx][ys_T[beta[j]]] = 1;
						}
					}
					else {
						int idx2 = ys_N[beta[j]];
						vector<int> tmp = FIRST[idx];
						FIRST[idx] = merge(FIRST[idx], FIRST[idx2]);
						flag = isUpdated(FIRST[idx], tmp);
					}
					break;
				}
			}
		}
	}
	for (int i = 0; i < toEpsilon.size(); i++)
	{
		if (toEpsilon[i] == 1)
		{
			FIRST[i][ys_T['\0']] = 1;
		}
	}
	
}


void ContextFreeGrammar::getFIRST_graph()
{
	int n = VN.size(),m = VT.size();
	vector<vector<int>> g(n+m);
	for (int i = 0; i < P.size(); i++)
	{
		int idx = P[i].first; string beta = P[i].second;
		if (beta.length() == 0)	continue;
		for (int j = 0; j < beta.size(); j++)
		{
			if (ys_T.find(beta[j]) != ys_T.end())
			{
				g[idx].push_back(ys_T[beta[j]] + n);
			}
			else
			{
				g[idx].push_back(ys_N[beta[j]]);
			}
			if (!ifToEpsilon(beta[j]))	break;					//如果不能推出epsilon，就不再继续
		}
	}

	for (int i = 0; i < n; i++)
	{
		vector<int> ans;
		vector<int> I = { i };
		int n = g.size();
		vector<bool> vis(n, 0);
		queue<int> q;
		for (int u : I)	if (u >= 0) q.push(u);
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
		for (int j = 0; j < ans.size(); j++)
		{
			int idx = ans[j];
			if (idx >= n)	FIRST[i][idx - n] = 1;		// 可达的终结符
		}
		if(toEpsilon[i])	FIRST[i][ys_T['\0']] = 1;			// epsilon
	}
}


void ContextFreeGrammar::print2()
{
	cout << VN.size() << " " << VT.size() << endl;
	for (char c : VN)	cout << ys_c2s[c] << " ";
	cout << endl;
	for (char c : VT) if (c != '\0') cout << c << " "; else cout << "epsilon" << " ";
	cout << "#" << endl;
}

void ContextFreeGrammar::getFOLLOW_loop()
{
	FOLLOW[ys_N[S]][VT.size()] = 1;	// '#'
	bool flag = 1;
	while (flag)
	{
		flag = 0;
		for (int i = 0; i < P.size(); i++)
		{
			int idx = P[i].first; string beta = P[i].second;
			for (int j = 0; j < beta.length(); j++)
			{
				if (isNonTerminaled(beta[j]))
				{
					int idx2 = ys_N[beta[j]];
					// vector<int> tmp1 = FOLLOW[idx];
					vector<int> tmp2 = FOLLOW[idx2];
					vector<char> cs = getFIRST(beta.substr(j + 1));
					for (char c : cs)
					{
						if (c == '\0')	continue;			// epsilon不在FOLLOW中
						FOLLOW[idx2][ys_T[c]] = 1;
					}
					if (ifToEpsilon(beta.substr(j + 1)))		//如果子串可以推导出epsilon
					{
						FOLLOW[idx2] = merge(FOLLOW[idx2], FOLLOW[idx]);
					}
					flag = flag || isUpdated(tmp2, FOLLOW[idx2]);			//变化了吗？
				}
			}
		}
	}
}

void ContextFreeGrammar::getFOLLOW_graph1()
{
	int n = VN.size(), m = VT.size();
	vector<vector<int>> g(n + n + m + 1);
	g[ys_N[S]].push_back(2 * n + m);
	for (auto p : P)
	{
		int idx = p.first; string beta = p.second;
		if (beta.length() == 0)	continue;
		for (int i = 0; i < beta.length(); i++)			//FOLLOW 的连边
		{
			if (isNonTerminaled(beta[i]))
			{
				int idx2 = ys_N[beta[i]];
				bool flag = 1;
				for (int j = i + 1; j < beta.length(); j++)
				{
					if (beta[j] == '\0')	continue;
					if (isTerminaled(beta[j])) {
						g[idx2].push_back(ys_T[beta[j]] + 2 * n);
					}
					else if (isNonTerminaled(beta[j]))
					{
						g[idx2].push_back(ys_N[beta[j]] + n);
					}
					if (!ifToEpsilon(beta[j])) {
						flag = 0;
						break;
					}
				}
				if (flag)
				{
					g[idx2].push_back(idx);
				}
			}
		}
		for (int i = 0; i < beta.length(); i++)
		{
			if (isNonTerminaled(beta[i]))
			{
				g[idx + n].push_back(ys_N[beta[i]] + n);
			}
			else if (isTerminaled(beta[i]))
			{
				g[idx + n].push_back(ys_T[beta[i]] + 2 * n);
			}
			if (!ifToEpsilon(beta[i])) {
				break;
			}
		}
	}
	for (int i = 0; i < n; i++)
	{
		vector<int> ans;
		vector<int> I = { i };
		int n = g.size();
		vector<bool> vis(n, 0);
		queue<int> q;
		for (int u : I)	if (u >= 0) q.push(u);
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
		for (int v : ans)
		{
			if (v >= 2 * n)
			{
				FOLLOW[i][v - 2 * n] = 1;
			}
		}
	}
}

void ContextFreeGrammar::getFOLLOW_graph2()
{
	int n = VN.size(), m = VT.size();
	vector<vector<int>> g(n  + m + 1);
	g[ys_N[S]].push_back(n + m);							//初始S连接到'#'
	for (auto p : P)
	{
		int idx = p.first; string beta = p.second;
		if (beta.length() == 0)	continue;
		for (int i = 0; i < beta.length(); i++)
		{
			if (isNonTerminaled(beta[i]))
			{
				int idx2 = ys_N[beta[i]];			//idx2
				vector<char> cs = getFIRST(beta.substr(i + 1));
				for (char c : cs)
				{
					if (c == '\0')	continue;
					g[idx2].push_back(ys_T[c] + n);
				}
				if (ifToEpsilon(beta.substr(i + 1)))
				{
					g[idx2].push_back(idx);
				}
			}
			
		}
	}
	for (int i = 0; i < n; i++)
	{
		vector<int> ans = bfs({ i }, g);
		for (int u : ans)
		{
			if (u >= n)
			{
				FOLLOW[i][u - n] = 1;
			}
		}
	}
}

void ContextFreeGrammar::getSELECT_loop()
{
	int m = VT.size();

	for (int i = 0; i < P.size(); i++)
	{
		int idx = P[i].first; string beta = P[i].second;
		vector<char> first = getFIRST(beta);
		for (char c : first)				//FIRST(beta)是一定有的一部分
		{
			if(c != '\0')
				SELECT[i][ys_T[c]] = 1;

		}
		if (ifToEpsilon(beta))					//如果beta可以推导出epsilon，加上FOLLOW(A)
		{
			vector<char> follow = getFOLLOW(VN[idx]);
			for (char c : follow)
			{
				if (c == '#')
					SELECT[i][m] = 1;
				else SELECT[i][ys_T[c]] = 1;
			}

		}

	}
}

ContextFreeGrammar::ContextFreeGrammar()
	:Grammar()
{

}

ContextFreeGrammar::ContextFreeGrammar(vector<char> vt, vector<string> vn, string s, vector<pair<string, string>> p)
	:Grammar(vt,vn,s)
{
	for (int i = 0; i < p.size(); i++)
	{
		PSS pss = p[i];
		string alpha = pss.first, beta = transs2c(pss.second);
		bool fq = 0;
		for (char c:beta)
		{
			if (ys_N.find(c) == ys_N.end() && ys_T.find(c) == ys_T.end())
			{
				cerr << "产生式 " << alpha << "->" << beta << " 不符合要求:" << endl << "beta中字符并非字符集字符" << endl;
				fq = 1;
				break;
			}
		}
		if (fq)
		{
			continue;
		}
		if (ys_N.find(ys_s2c[alpha]) == ys_N.end())
		{
			cerr << "产生式 " << alpha << "->" << beta << " 不符合要求:" << endl << "不是上下文无关文法" << endl;;
		}
		else
		{
			string beta2;
			for (int i = 0; i < beta.length(); i++)
			{
				if (beta[i] == '\0')	continue;
				beta2.push_back(beta[i]);
			}
			int idx = ys_N[ys_s2c[alpha]];
			pair<int, string> t = { idx,beta2 };
			P.push_back({ idx,beta2 });
		}
	}


	compress();
	canGetEpsilon();
	getFIRST_loop();

	getFOLLOW_loop();

	getSELECT_loop();
	isll1 = isLL1();
}

void ContextFreeGrammar::print()
{
	cout << VN.size() << endl;
	for (int i = 0; i < VN.size(); i++)
	{
		cout << ys_c2s[VN[i]] << " ";
	}
	cout << endl;
	cout << VT.size() << endl;
	for (int i = 0; i < VT.size(); i++)
	{
		if(VT[i] != '\0')
			cout << VT[i] << " ";
		else cout << "epsilon ";
	}
	cout << endl;
	cout <<  S_o << endl;
	cout << P.size() << endl;
	for (int i = 0; i < P.size(); i++)
	{
		cout << ys_c2s[VN[P[i].first]] << "->" << transc2s(P[i].second) << endl;
	}
}

vector<char> ContextFreeGrammar::getVN()
{
	return VN;
}

vector<char> ContextFreeGrammar::getVT()
{
	return VT;
}



vector<int> ContextFreeGrammar::canGetEpsilon()
{
	vector<int> res(VN.size()), in(VN.size(), 0), left(P.size(), 0);
	vector<bool> isHave(P.size(), 1);
	vector<vector<int>> lx(VN.size());
	vector<pair<int, string>> P = this->P;
	for (int i = 0; i < VN.size(); i++)	res[i] = -1;
	for (int i = 0; i < P.size(); i++)
	{
		int idx = P[i].first;
		string beta = P[i].second;

		in[idx]++;
		left[i] = beta.length();
		isHave[i] = 1;
		lx[idx].push_back(i);
	}
	for (int i = 0; i < P.size(); i++)
	{
		if (!isHave[i])	continue;
		if (left[i] == 0)			// A->epsilon
		{
			int idx = P[i].first;
			res[idx] = 1;					//res[A] = 1
			in[idx] = 0;
			for (int v : lx[idx])					//删除所有以A为左部的产生式
			{
				isHave[v] = 0;						
			}
			continue;
		}
		for (char c : P[i].second)
		{
			if (ys_T.find(c) != ys_T.end())       // 产生式中有终结符beta
			{
				isHave[i] = 0;						//删除该产生式
				int idx = P[i].first;
				in[idx]--;
				if (!in[idx])				    // 如果以A为左部的产生式都被删除了
				{
					res[idx] = 0;				// res[B] = 0
				}
			}
		}
	}

	bool flag = 1;
	while (flag)
	{
		flag = 0;
		for (int i = 0; i < P.size(); i++)
		{
			if (!isHave[i])	continue;
			int idx = P[i].first;
			string& beta = P[i].second;
			for (int j=0;j<beta.size();j++)
			{
				char c = beta[j];
				if (c == '\0')	continue;
				if (res[ys_N[c]] == -1)	continue;
				else if (res[ys_N[c]] == 1) {		//如果beta中的c可以推导出epsilon
					beta[j] = '\0';				//删除beta中的c，用\0代替
					left[i]--;
				}
				else {						//beta中的c不能推导出epsilon
					isHave[i] = 0;				//删除该产生式
					in[idx]--;	
					if (!in[idx])				//如果以A为左部的产生式都被删除了
					{
						res[idx] = 0;			//res[B] = 0
						flag = 1;
					}
				}
			}
			if (isHave[i] && left[i] == 0)		//如果beta中的所有字符都被删完了
			{
				res[idx] = 1;					//res[A] = 1	
				for (int v : lx[idx])			//删除所有以A为左部的产生式
				{
					isHave[v] = 0;
				}
				flag = 1;
			}
		}
	
	}
	toEpsilon = res;

	return res;
}

vector<pair<string,vector<char>>> ContextFreeGrammar::getFISRT()
{
	vector<pair<string, vector<char>>> res;
	for (int i = 0; i < FIRST.size(); i++)
	{
		vector<char> ans;
		for (int j = 0; j < FIRST[i].size(); j++)
		{
			if (FIRST[i][j])
			{
				ans.push_back(VT[j]);
			}
		}
		res.push_back({ ys_c2s[VN[i]],ans });
	}
	return res;
}

vector<char> ContextFreeGrammar::getFIRST(string s)
{
	vector<int> bs(VT.size(), 0);
	vector<char> res;
	bool flag = 1;
	for (char c : s)
	{
		vector<char> cs = getFIRST(c);
		for (char cc:cs)
		{
			if (cc == '\0')	continue;
			bs[ys_T[cc]] = 1;
		}
		if (!ifToEpsilon(c)) {
			flag = 0;
			break;
		}
	}
	if (ys_T.find('\0') != ys_T.end())
	{
		bs[ys_T['\0']] = flag;
	}
	for (int i = 0; i < bs.size(); i++)
	{
		if (bs[i])	res.push_back(VT[i]);
	}
	
	return res;
}

vector<char> ContextFreeGrammar::getFIRST(char c)
{
	vector<char> res;
	if (ys_N.find(c) != ys_N.end())
	{
		int idx = ys_N[c];
		for (int i = 0; i < FIRST[idx].size(); i++)
		{
			if (FIRST[idx][i] == 1) {
				res.push_back(VT[i]);
			}
		}
	}
	if (ys_T.find(c) != ys_T.end())
	{
		res.push_back(c);
	}

	return res;
}

vector<char> ContextFreeGrammar::getFOLLOW(char c)
{
	vector<char> res;
	if (isNonTerminaled(c))
	{
		int idx = ys_N[c];
		for (int i = 0; i < FOLLOW[idx].size()-1; i++)
		{
			if (FOLLOW[idx][i])	res.push_back(VT[i]);
		}
		if (FOLLOW[idx].back())	res.push_back('#');
	}
	return res;
}

vector<pair<string, vector<char>>> ContextFreeGrammar::getFOLLOW()
{
	vector<pair<string, vector<char>>> res;
	for (int i = 0; i < VN.size(); i++)
	{
		res.push_back({ ys_c2s[VN[i]],getFOLLOW(VN[i]) });
	}
	return res;
}

vector<pair<PSS, vector<char>>> ContextFreeGrammar::getSELECT()
{
	vector<pair<PSS, vector<char>>>  res;
	for (int i = 0; i < P.size(); i++)
	{
		if(P[i].second.length() > 0)
			res.push_back({ {ys_c2s[VN[P[i].first]],transc2s(P[i].second)},getSELECT(i) });
		else
			res.push_back({ {ys_c2s[VN[P[i].first]],"epsilon"},getSELECT(i)});
	}
	return res;
}

vector<char> ContextFreeGrammar::getSELECT(int idx)
{
	vector<char> res;
	if (idx >= P.size())
	{
		cerr << "产生式下标不合法!" << endl;
		return res;
	}
	for (int i = 0; i < SELECT[idx].size()-1; i++)
	{
		if(SELECT[idx][i])
			res.push_back(VT[i]);
	}
	if (SELECT[idx].back())
		res.push_back('#');
	return res;
}

vector<char> ContextFreeGrammar::getSELECT(char c, string beta)
{
	string beta2;
	for (char c : beta)
	{
		if (c == '\0')	continue;
		beta2.push_back(c);
	}
	for (int i = 0; i < P.size(); i++)
	{
		int idx = P[i].first; string _b = P[i].second;
		if (VN[idx] == c && beta2 == _b)	return getSELECT(i);
	}
	cerr << "没有" << c << "->" << beta << "这一条产生式！" << endl;
	return vector<char>();
}

//vector<PCS> ContextFreeGrammar::getP()
//{
//	return vec;
//}

bool ContextFreeGrammar::isLL1()
{
	vector<vector<int>> lx(VN.size());
	for (int i = 0; i < P.size(); i++)
	{
		lx[P[i].first].push_back(i);			//同一个非终结符的产生式下标
	}
	bool flag = 1;
	for (int i = 0; i < lx.size(); i++)
	{
		auto idxs = lx[i];
		if (idxs.empty())	continue;
		for (int j = 0; j < SELECT[idxs[0]].size(); j++)			//每一个终结符
		{
			int cnt = 0;
			for (int u : idxs)
			{
				if (SELECT[u][j]) {
					cnt++;													//记录在几个产生式中出现
					ys_SELECT[i][j].push_back(u);
				}
			}
			if (cnt > 1)	flag = 0;				//出现次数不能超过1

		}
	}
	return flag;
}
bool ContextFreeGrammar::isLL_1() const
{
	return isll1;
}

string ContextFreeGrammar::LL1(string _s,vector<PCS>& res)
{
	stringstream ss(_s);
	if (!isll1) {
		return "该文法不是LL(1)文法，不能使用LL(1)分析!";
	}
	string s;
	for (char c : _s)	if (c != '\0')	s.push_back(c);
	s.push_back('#');
	int i = 0, j = 0;
	string now; now.push_back(S);
	while (i < now.length())
	{
		if (!inV(s[j]) && s[j] != '#')
		{
			return "该字符串中出现了非字符表中的字符";
		}
		if (now[i] == s[j])
		{
			i++, j++;
			continue;
		}
		if (isNonTerminaled(now[i]))
		{
			int idx = ys_N[now[i]];
			int idc = -1;
			if (s[j] == '#')
				idc = VT.size();
			else idc = ys_T[s[j]];
			if (!ys_SELECT[idx][idc].empty())
			{
				int idx2 = ys_SELECT[idx][idc][0];
				res.push_back({ now[i],P[idx2].second });
				string tmp;
				for (int k = 0; k < i; k++)
				{
					tmp.push_back(now[k]);
				}
				tmp = tmp + P[idx2].second;
				for (int k = i + 1; k < now.length(); k++)
				{
					tmp.push_back(now[k]);
				}
				ss << "使用" << ys_c2s[now[i]] << "->" << transc2s(P[idx2].second) << ", 变为" << transc2s(tmp) << endl;
				now = tmp;
			}
			else {
				return "该文法不能推导出该句型";
			}
		}
		else
		{
			if (now[i] != s[j]) {					//失败
				return "该文法不能推导出该句型";
			}
			i++, j++;
		}
	}

	if (j < s.length() - 1)
	{
		return "该文法不能推导出该句型";
	}
	string tmp;
	while (!ss.eof())
	{
		string s;
		getline(ss, s);
		tmp += s + "\n";
	}
	return tmp;
}

vector<LL1Row> ContextFreeGrammar::LL1(string _s)
{
	if (!isll1)
	{
		return vector<LL1Row>({ {"该文法不是LL(1)文法","该文法不是LL(1)文法","Syntax Error:该文法不是LL(1)文法"}});
	}
	vector<LL1Row> res;
	string s;
	for (char c : _s)	if (c != '\0')	s.push_back(c);
	s.push_back('#');
	string st;
	st.push_back('#');
	st.push_back(S);
	int len = s.length();
	int now = 0;
	while (!st.empty() && now < len)
	{
		LL1Row tmp;
		tmp.sentence = transc2s(s.substr(now));
		tmp.stack = transc2s(st);
		char X = st.back(); st.pop_back();
		char c = s[now];
		string cs = ""; cs.push_back(c);
		string left; left.push_back(X);
		if (!inV(c))				// 该字符不在字符集合里
		{
			tmp.action = "Syntax Error: 字符" + cs + string("不在字符集里");
			res.push_back(tmp);
			break;
		}
		else if (X == '#')		// X是#
		{
			if (c == '#')		// c也是# 推导成功
			{
				tmp.action = "over: 推导成功";
				res.push_back(tmp);
			}
			else {				// X是# c不是# 前缀是一个句子
				tmp.action = "Syntax Error: 该串的前缀是一个句子，但该串不是本文法的句子";
				res.push_back(tmp);
				break;
			}
		}
		else if (isNonTerminaled(X))		// X是非终结符
		{
			int idN = ys_N[X];
			int idT = -1;
			if(c != '#')	idT = ys_T[c];
			else idT = VT.size();
			if (!ys_SELECT[idN][idT].empty())		// 查预测分析表，查到TABLE[X][c]非空，为产生时idx
			{
				int idx = ys_SELECT[idN][idT][0];
				string right = P[idx].second;		// 选用产生式idx
				if(right.empty())
					tmp.action = ys_c2s[left[0]] + "->epsilon";
				else
					tmp.action = ys_c2s[left[0]] + "->" + transc2s(right);
				
				res.push_back(tmp);
				for (int i = right.length() - 1; i >= 0; i--)	//倒序入栈
				{
					if (right[i] != '\0')
						st.push_back(right[i]);
				}
			}
			else			// 查预测分析表，为空
			{	
				if (c == '#'){			//如果c == '#' 说明该串是某个句子的真前缀，不完整
					tmp.action = "Syntax Error: 该串不完整，是某个句子的真前缀，但不是句子";
					res.push_back(tmp);
					break;
				}
				else {				// 否则，该串不是本文法的句子，X没办法推导出以c开头的串
					tmp.action = "Syntax Error: 非终结符" + ys_c2s[left[0]] + string("无法推导出以") + "终结符" + cs + "开头的句子";
					res.push_back(tmp);
					break;
				}
			}
		}
		else			// X是终结符
		{
			if (X == c)			// 字符c匹配成果，消去c
			{
				tmp.action = "终结符\"" + left + string("\"匹配");
				res.push_back(tmp);
				now++;			// 消去c[指针前移]
			}
			else if (c == '#')		// 串完了，但是推导未完，句子不完整
			{
				tmp.action = "Syntax Error: 该串不完整，是某个句子的真前缀，但不是句子";
				res.push_back(tmp);
				break;
			}
			else			// 终结符不能匹配
			{
				tmp.action = "Syntax Error: " + left + "和" + s.substr(now,1) + "无法匹配";
				res.push_back(tmp);
				break;
			}
		}
	}
	return res;
}

bool ContextFreeGrammar::ifToEpsilon(char c)
{
	if (ys_T.find(c) != ys_T.end())
	{
		if (c != '\0')	return false;
		return true;
	}
	if (ys_N.find(c) != ys_N.end())
	{
		return toEpsilon[ys_N[c]];
	}
	return false;
}

bool ContextFreeGrammar::ifToEpsilon(string s)
{
	for (char c : s)
	{
		if (ys_T.find(c) != ys_T.end())
		{
			if (c != '\0')	return false;
		}
		if (ys_N.find(c) != ys_N.end())
		{
			if (!toEpsilon[ys_N[c]])return false;
		}
	}

	return true;
}

bool ContextFreeGrammar::isTerminaled(char c)
{
	return ys_T.find(c) != ys_T.end();
}


bool ContextFreeGrammar::isNonTerminaled(char c)
{
	return ys_N.find(c) != ys_N.end();
}


bool ContextFreeGrammar::inV(char c)
{
	return isTerminaled(c) || isNonTerminaled(c) || c == '#';
}