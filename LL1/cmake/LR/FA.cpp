#include"FA.h"
#include<fstream>
#include<sstream>
ReturnInt::ReturnInt(int _i)
	:i(_i)
{

}

ReturnInt::ReturnInt(vector<int> _vi)
	:vi(_vi),i(0)
{

}

ReturnInt::operator vector<int>(){return vi; }
ReturnInt::operator int() { return i; }

FA::FA(vector<int> _K, vector<char> _Sigma, vector<int> _Z)
	:K(_K),Sigma(_Sigma),Z(_Z)
{
	n = _K.size(); m = _Sigma.size();
	for (int i = 0; i < m; i++) {
		ys[_Sigma[i]] = i;
	}
	isEnd.resize(n, 0);
	for (int i = 0; i < _Z.size(); i++)
	{
		isEnd[_Z[i]] = 1;
	}
}
int FA::getM() const
{
	return m;
}
int FA::getN() const
{
	return n;
}

vector<char> FA::getSigma() const
{
	return Sigma;
}

bool FA::IsEnd(int t)
{
	if (t >= n)	return 0;
	return isEnd[t];
}
/*********************************************************/


bool NondeterministicFiniteAutomata::input(string& s, int u,int now)
{
	if (now == s.length())
	{
		return isEnd[u];
	}
	char c = s[now];
	if (ys.find(c) != ys.end())
	{
		int id = ys[c];
		for (int v : f[u][id])
		{
			if (input(s, v, now + 1))
				return true;
		}
	}
	if (ys.find('\0') != ys.end())
	{
		int id = ys['\0'];
		for (int v : f[u][id])
		{
			if (input(s, v, now))
				return true;
		}
	}
	return false;
}

NondeterministicFiniteAutomata::NondeterministicFiniteAutomata(vector<int> _K, vector<char> _Sigma, vector<vector<vector<int>>> _f, vector<int>_S, vector<int>_Z)
	:FA(_K,_Sigma,_Z),f(_f),S(_S)
{
	
}

ReturnInt NondeterministicFiniteAutomata::next(int u, char c)
{
	return f[u][ys[c]];
}

vector<int> NondeterministicFiniteAutomata::closure(vector<int> I)
{
	if (ys.find('\0') != ys.end())			//�����Ƿ��Цű�
	{
		int c = ys['\0'];
		vector<vector<int>> g;			//����һ���µ�����ͼ
		for (auto row : f)
		{
			g.push_back(row[c]);
		}
		return bfs(I, g);			//bfs
	}
	else
	{
		cout << "�ַ�����û�Цű�!!" << endl;
		return I;					//���û�Цűߣ���ֱ�ӷ���I����
	}
}

vector<int> NondeterministicFiniteAutomata::move(vector<int> I, char a)
{
	if (ys.find(a) != ys.end())		//����ͼ������a��
	{
		int column = ys[a];
		vector<int> ans;
		vector<bool> vis(n, 0);
		for (int u : I)				//��������I
		{
			for (int v : f[u][column])	//�����ܴӽڵ�u����a������Ľڵ�v
			{
				if (!vis[v])		//vis����ȥ��
				{
					ans.push_back(v);
					vis[v] = 1;
				}
			}
		}
		return ans;
	}
	else
	{
		cout << "�ַ�����û�и�" << a << "�ַ�!!" << endl;
		return {};				//û��a����ֱ�ӷ��ؿռ�
	}
}

vector<int> NondeterministicFiniteAutomata::getStart() const
{
	return S;
}

void NondeterministicFiniteAutomata::paint() const
{
#define Endl ";" << endl
	ofstream fout("./html/render.txt", ios::out);
	ofstream dot("./NFA.dot", ios::out);
	dot << "digraph G {" << endl;
	dot << "graph[ layout = \"dot\"];" << endl;
	dot << "node[color = \"red\", shape = \"circle\", height = 0.1,fontsize=20];" << endl;
	dot << "edge[len = 1.0,fontsize=20];" << endl;
	dot << "-1 [style=\"invis\"];" << endl;
	vector<vector<int>> edges;
	for (int i = 0; i < n; i++)
	{
		if (isEnd[i])
			dot << i << "[shape=\"doublecircle\"]" << Endl;
		else
			dot << i << Endl;
		for (int j = 0; j < m; j++)
		{
			for(int v:f[i][j])
				edges.push_back({ j,i,v });
		}
	}
	fout << n << " " << m << " " << n * m << " " << 1 << " " << Z.size() << endl;
	for (int i = 0; i < m; i++)	fout << Sigma[i] << " ";
	fout << endl;
	for (auto edge : edges)
	{
		int u = edge[1], v = edge[2];
		char label = Sigma[edge[0]];
		if(label == '\0')
			dot << u << "->" << v << "  " << "[label=" << "epsilon" << "]" << Endl;
		else
			dot << u << "->" << v << "  " << "[label=" << label << "]" << Endl;
		for (int v : edge)	fout << v << " ";
		fout << endl;
	}
	for (int v : S)
	{
		fout << v << " ";
		dot << "-1->" << v << Endl;
	}
	cout << endl;
	for (int i = 0; i < Z.size(); i++)	fout << Z[i] << " ";
	fout << endl;
	dot << "}";
	dot.close();
	fout.close();
	system("print_NFA.bat NFA.png");
	system("NFA.png");
}



bool NondeterministicFiniteAutomata::check(string s)
{
	for (int i = 0; i < S.size(); i++)
	{
		if (input(s, S[i], 0))	return true;
	}
	return false;
}



/*******************************************************/
DeterministicFiniteAutomata::DeterministicFiniteAutomata(vector<int> _K, vector<char> _Sigma, vector<vector<int>> _f, int _S, vector<int> _Z)
	:FA(_K,_Sigma,_Z), S(_S)
{
	f.resize(_K.size());
	for (int i = 0; i < _K.size(); i++)
	{
		for (int j = 0; j < _Sigma.size(); j++)
		{
			if (_f[i][j] >= 0)	f[i].push_back(_f[i][j]);
			else f[i].push_back(null);
		}
	}
}

ReturnInt DeterministicFiniteAutomata::next(int u, char c)
{
	return f[u][ys[c]];
}

void DeterministicFiniteAutomata::paint(string pngname) const
{
#define Endl ";" << endl
	ofstream fout("./html/render.txt", ios::out);
	ofstream dot("./DFA.dot", ios::out);
	dot << "digraph G {" << endl;
	dot << "graph[layout = \"neato\",start=20];" << endl;
	dot << "node[color = \"red\", shape = \"circle\", height = 0.1,fontsize=20];" << endl;
	dot << "edge[len = 2.0,fontsize=20];" << endl;
	dot << "-1 [style=\"invis\"];" << endl;
	vector<vector<int>> edges;
	for (int i = 0; i < n; i++)
	{
		if (isEnd[i])
			dot << i << "[shape=\"doublecircle\"]" << Endl;
		else 
			dot << i << Endl;
		for (int j = 0; j < m; j++)
		{
			if(f[i][j] >= 0)
				edges.push_back({ j,i,f[i][j] });
		}
	}
	fout << n << " " << m << " " << n * m << " " << 1 << " " << Z.size() << endl;
	for (int i = 0; i < m; i++)	fout << Sigma[i] << " ";
	fout << endl;
	for (auto edge : edges)
	{
		int u = edge[1], v = edge[2];
		char label = Sigma[edge[0]];

		dot << u << "->" << v << "  " << "[label=" << label << "]" << Endl;
		for (int v : edge)	fout << v << " ";
		fout << endl;
	}
	fout << S << endl;
	dot << "-1->" << S << Endl;
	for (int i = 0; i < Z.size(); i++)	fout << Z[i] << " ";
	fout << endl;
	dot << "}";
	dot.close();
	fout.close();
	system(("print_DFA.bat " + pngname).c_str());
	system(pngname.c_str());
	//system("python paint.py ./html/render.txt");
	cout << "״̬ת��������" << endl;
	cout << "  ";
	for (int i = 0; i < Sigma.size(); i++)	cout << Sigma[i] << " ";
	cout << endl;
	for (int i = 0; i < n; i++)
	{
		cout << i << " ";
		for (int j = 0; j < m; j++)
		{
			cout << f[i][j] << " ";
		}
		if (isEnd[i])	cout << 1 << endl;
		else cout << 0 << endl;
	}
}



void DeterministicFiniteAutomata::paint() const
{
#define Endl ";" << endl
	ofstream fout("./html/render.txt", ios::out);
	ofstream dot("./DFA.dot", ios::out);
	dot << "digraph G {" << endl;
	dot << "graph[layout = \"neato\",start=20];" << endl;
	dot << "node[color = \"red\", shape = \"circle\", height = 0.1,fontsize=20];" << endl;
	dot << "edge[len = 2.0,fontsize=20];" << endl;
	dot << "-1 [style=\"invis\"];" << endl;
	vector<vector<int>> edges;
	for (int i = 0; i < n; i++)
	{
		if (isEnd[i])
			dot << i << "[shape=\"doublecircle\"]" << Endl;
		else
			dot << i << Endl;
		for (int j = 0; j < m; j++)
		{
			if (f[i][j] >= 0)
				edges.push_back({ j,i,f[i][j] });
		}
	}
	fout << n << " " << m << " " << n * m << " " << 1 << " " << Z.size() << endl;
	for (int i = 0; i < m; i++)	fout << Sigma[i] << " ";
	fout << endl;
	for (auto edge : edges)
	{
		int u = edge[1], v = edge[2];
		char label = Sigma[edge[0]];

		dot << u << "->" << v << "  " << "[label=" << label << "]" << Endl;
		for (int v : edge)	fout << v << " ";
		fout << endl;
	}
	fout << S << endl;
	dot << "-1->" << S << Endl;
	for (int i = 0; i < Z.size(); i++)	fout << Z[i] << " ";
	fout << endl;
	dot << "}";
	dot.close();
	fout.close();
	system("print_DFA.bat DFA.png");
	system("DFA.png");
	//system("python paint.py ./html/render.txt");
	cout << "״̬ת��������" << endl;
	cout << "  ";
	for (int i = 0; i < Sigma.size(); i++)	cout << Sigma[i] << " ";
	cout << endl;
	for (int i = 0; i < n; i++)
	{
		cout << i << " ";
		for (int j = 0; j < m; j++)
		{
			cout << f[i][j] << " ";
		}
		if (isEnd[i])	cout << 1 << endl;
		else cout << 0 << endl;
	}
}

/// @brief 从等价关系E中取得划分
/// @param E 等价关系
/// @return 等价关系对应的划分
vector<vector<int>> DeterministicFiniteAutomata::getI(vector<vector<int>>& E)
{
	vector<vector<int>> I;
	vector<bool> vis(E.size(), 0);
	for (int i = 0; i < E.size(); i++)
	{
		if (E[i][0] == -1)	continue;
		if (vis[i])	continue;
		vector<int> ans;
		vis[i] = 1;
		for (int j = i; j < E.size(); j++)
		{
			if (E[i][j]) {
				ans.push_back(j);
				vis[j] = 1;
			}
		}
		I.push_back(ans);
	}
	return I;

}

DeterministicFiniteAutomata DeterministicFiniteAutomata::minimize()
{
	vector<int> K_reachable = bfs(K, f);
	vector<int> K_tmp;
	for (int u : K_reachable)
	{
		vector<int> tmp = bfs({ u }, f);
		bool endable = 0;
		for (int v : tmp)
		{
			if (IsEnd(v))
			{
				endable = 1;
				break;
			}
		}
		if (endable)	K_tmp.push_back(u);
	}
	vector<vector<int>> table;
	table.resize(K.size());
	for (int i = 0; i < K.size(); i++)
	{
		table[i].resize(K.size());
		for (int j = 0; j < K.size(); j++)
		{
			table[i][j] = -1;
		}
	}
	for (int i = 0; i < K_tmp.size(); i++)
	{
		for (int j = 0; j < K_tmp.size(); j++)
		{
			table[K_tmp[i]][K_tmp[j]] = (isEnd[K_tmp[i]] == isEnd[K_tmp[j]]);
		}
	}
	while (1)
	{
		vector<vector<int>> Is = getI(table);
		vector<int> where(getN(), 0);
		bool flag = 0;
		for (int i = 0; i < Is.size(); i++)
		{
			for (int u : Is[i])
			{
				where[u] = i;
			}
		}
		for (vector<int>& I : Is)
		{
			for (int u : I)
			{
				for (int v : I)
				{
					for (int i = 0; i < getM(); i++)
					{
						if ((f[u][i] >= 0) ^ (f[v][i] >= 0)) {
							table[u][v] = table[v][u] = 0;
							flag = 1;
						}
						if (f[u][i] < 0 || f[v][i] < 0)	continue;
						if (where[f[u][i]] != where[f[v][i]])
						{
							table[u][v] = table[v][u] = 0;
							flag = 1;
						}
					}
				}
			}
		}
		if (!flag)	break;
	}

	vector<vector<int>> Is = getI(table);
	vector<int> where(getN(), 0);
	bool flag = 0;
	for (int i = 0; i < Is.size(); i++)
	{
		for (int u : Is[i])
			where[u] = i;
	}
	vector<int> Ks;
	vector<int> Kz;
	int S2 = -1;
	vector<vector<int>> F;
	F.resize(Is.size());
	for (int i = 0; i < Is.size(); i++)
	{
		Ks.push_back(i);
		F[i].resize(Sigma.size());
		for (int j = 0; j < Sigma.size(); j++)
		{
			if (f[Is[i][0]][j] >= 0)
				F[i][j] = where[f[Is[i][0]][j]];
			else F[i][j] = null;
		}
		if (isEnd[Is[i][0]])	Kz.push_back(i);
		for (int v : Is[i])
		{
			if (v == S) S2 = i;
		}
	}

	return DeterministicFiniteAutomata(Ks,Sigma,F,S2,Kz);
}

bool DeterministicFiniteAutomata::check(string s)
{
	int nowu = S;
	for (int i = 0; i < s.length(); i++)
	{
		if (ys.find(s[i]) == ys.end())
		{
			return false;
		}
		int id = ys[s[i]];
		if (f[nowu][id] < 0)	return false;
		nowu = f[nowu][id];
	}

	return isEnd[nowu];
}



/*************************************************************************/
DeterministicFiniteAutomata Determine(NondeterministicFiniteAutomata& nfa)
{
	queue<int> q;
	vector<int> S = nfa.getStart();
	vector<int> Ks = nfa.closure(S);
	vector<vector<int>> K;
	K.push_back(Ks);
	vector<vector<int>> F;
	int m = nfa.getM(), n = nfa.getN();
	vector<char> Sigma0 = nfa.getSigma();
	vector<char> Sigma = {};
	for (char c : Sigma0) { if (c != '\0')Sigma.push_back(c); }
	for (int k = 0;; k++)
	{
		if (k >= K.size())	break;
		vector<int> nowstate = K[k];
		F.push_back(vector<int>(Sigma.size(), -1));
		for (int i = 0; i < Sigma.size(); i++)	F[k][i] = null;
		for (int i = 0; i < Sigma.size(); i++)
		{
			vector<int> tmp = nfa.move(nowstate, Sigma[i]);
			vector<int> newstate = nfa.closure(tmp);
			vector<bool> newbool(n, 0);
			for (int u : newstate)	newbool[u] = 1;
			int flag = -1;
			for (int j = 0; j < K.size(); j++)
			{
				vector<int> oristate = K[j];
				vector<bool> oribool(n, 0);
				for (int v : oristate)	oribool[v] = 1;
				bool same = 1;
				for (int i = 0; i < n; i++) {
					if (newbool[i] != oribool[i])
					{
						same = 0;
						break;
					}
				}
				if (same) {
					flag = j;
					break;
				}
			}
			if (newstate.empty())	F[k][i] = null;
			else if (flag >= 0) { F[k][i] = flag; }
			else {
				F[k][i] = K.size();
				K.push_back(newstate);
			}
		}
	}
	vector<int> Kt = {};
	vector<int> Kk = {};
	for (int i = 0; i < K.size(); i++)
	{
		Kk.push_back(i);
		vector<int> nowstate = K[i];
		for (int v : nowstate) {
			if (nfa.IsEnd(v)) {
				Kt.push_back(i);
				break;
			}
		}
	}
	DeterministicFiniteAutomata dfa = DeterministicFiniteAutomata(Kk, Sigma, F, 0, Kt);

	return dfa;
}
