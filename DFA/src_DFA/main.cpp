#include<iostream>
#include<set>
#include<vector>
#include"FA.h"
using namespace std;

#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

void createTest(int test_num) 
{
	std::ofstream outputFile("test.txt",ios::out);
	if (!outputFile) {
		std::cerr << "无法打开文件" << std::endl;
		return;
	}

	// 初始化随机数生成器
	std::srand(std::time(0));

	// 设置字符串长度
	const int MAX_LENGTH = 20;

	// 生成并写入1000个随机字符串
	for (int i = 0; i < test_num; ++i) {
		std::string randomString;
		int nowl = rand() * 1.0 / RAND_MAX * MAX_LENGTH + 1;
		randomString.resize(nowl);

		for (int j = 0; j <nowl; ++j) {
			// 随机选择'a', 'b', 或 'c'
			char randomChar = 'a' + std::rand() % 3;
			randomString[j] = randomChar;
		}

		outputFile << randomString << std::endl;
	}
	outputFile.close();
	std::cout << "随机字符串已写入test.txt" << std::endl;
}


int main(int argc, char* argv[])
{
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " input_file" << std::endl;
            return 1;
        }

	freopen(argv[1], "r", stdin);
	
	int n, m, p, q, t;	cin >> n >> m >> p >> q >> t;
	vector<int> K,S,Z;
	vector<vector<vector<int>>> F;
	vector<vector<int>> f;
	vector<char> Sigma;
	for (int i = 0; i < n; i++)	K.push_back(i);
	for (int i = 0; i < m; i++) {
		string s;	cin >> s;
		if (s != "epsilon")
			Sigma.push_back(s[0]);
		else Sigma.push_back('\0');
	}
	F.resize(n);
	f.resize(n);
	for (int i = 0; i < n; i++)
	{
		F[i].resize(m);
		f[i].resize(m);
		for (int j = 0; j < m; j++)	f[i][j] = -1;
	}
	for (int i = 0; i < p; i++)
	{
		int c, u, v;
		cin >> c >> u >> v;
		F[u][c].push_back(v);
		f[u][c] = v;
	}
	for (int i = 0; i < q; i++) {
		int u; cin >> u; S.push_back(u);
	}
	for (int i = 0; i < t; i++) {
		int u;	cin >> u; Z.push_back(u);
	}

	NondeterministicFiniteAutomata nfa = NondeterministicFiniteAutomata(K, Sigma, F, S, Z);
	DeterministicFiniteAutomata dfa = Determine(nfa);
	// DeterministicFiniteAutomata dfa = DeterministicFiniteAutomata(K, Sigma, f, S[0], Z);
	nfa.paint();				//nfa的png
	dfa.paint();				//dfa的png


	DeterministicFiniteAutomata dfa2 = dfa.minimize();
	dfa2.paint("DFA_min.png");	//dfa2的png
	return 0;
}
