#include "Grammar.h"
#include <fstream>
using namespace std;

int parse(int argc, char *argv[], string &filepath, string &mode, string &s_value)
{
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (arg.find("-filepath=") == 0)
		{
			filepath = arg.substr(10);
		}
		else if (arg.find("-mode=") == 0)
		{
			mode = arg.substr(6);
		}
		else if (arg.find("-s=") == 0)
		{
			s_value = arg.substr(3);
		}
		else
		{
			std::cerr << "Unknown option: " << arg << std::endl;
			return EXIT_FAILURE;
		}
	}
	return 0;
}

void cacuSet(ContextFreeGrammar &cfg)
{
	cfg.print();
	vector<pair<string, vector<char>>> first = cfg.getFISRT();
	vector<pair<string, vector<char>>> follow = cfg.getFOLLOW();
	vector<pair<PSS, vector<char>>> select = cfg.getSELECT();

	for (int i = 0; i < first.size(); i++)
	{
		cout << first[i].first << ": {";
		for (int j = 0; j < first[i].second.size() - 1; j++)
		{
			if (first[i].second[j] != '\0')
				cout << first[i].second[j] << ", ";
			else
				cout << "epsilon, ";
		}
		if (first[i].second.back() != '\0')
			cout << first[i].second.back() << "}" << endl;
		else
			cout << "epsilon}" << endl;
	}

	for (int i = 0; i < follow.size(); i++)
	{
		cout << follow[i].first << ": {";
		if (follow[i].second.empty())
			cout << "}" << endl;
		else
		{
			for (int j = 0; j < follow[i].second.size() - 1; j++)
			{
				cout << follow[i].second[j] << ", ";
			}
			cout << follow[i].second.back() << "}" << endl;
		}
	}

	for (int i = 0; i < select.size(); i++)
	{
		PSS p = select[i].first;
		cout << p.first << "->" << p.second << ":{";
		if (select[i].second.empty())
		{
			cout << "}" << endl;
			continue;
		}
		for (int j = 0; j < select[i].second.size() - 1; j++)
		{
			cout << select[i].second[j] << ", ";
		}
		cout << select[i].second.back() << "}" << endl;
	}
	cout << cfg.isLL_1() << endl;
}

void derivation(ContextFreeGrammar &cfg, string s)
{
	vector<PCS> res;
	string ans = cfg.LL1(s, res);
	cout << ans << endl;
}
void derivation2(ContextFreeGrammar &cfg, string s)
{

	cfg.print2();
	vector<char> VN = cfg.getVN();
	vector<char> VT = cfg.getVT();
	vector<vector<string>> table = cfg.getys_SELECT();
	vector<LL1Row> rows = cfg.LL1(s);
	for (int i = 0; i < VN.size(); i++)
	{
		for (int j = 0; j <= VT.size(); j++)
			cout << table[i][j] << endl;
	}
	cout << rows.size() << endl;
	for (int i = 0; i < rows.size(); i++)
	{
		cout << rows[i].stack << " " << rows[i].sentence << " " << rows[i].action << endl;
	}
}

#ifdef __linux__
int main(int argc, char *argv[])
{
	string filepath, mode, s_value;
	int parseResult = parse(argc, argv, filepath, mode, s_value);
	if (parseResult)
	{
		return parseResult;
	}

	ifstream fin(filepath.c_str(), ios::in);

	int n, t, p;
	fin >> n >> t >> p;
	vector<char> VT;
	vector<string> VN;
	vector<PSS> P;
	string S;
	for (int i = 0; i < n; i++)
	{
		string s;
		fin >> s;
		VN.push_back(s);
	}
	for (int i = 0; i < t; i++)
	{
		string s;
		fin >> s;
		if (s == "epsilon")
			VT.push_back('\0');
		else
			VT.push_back(s[0]);
	}
	for (int i = 0; i < p; i++)
	{
		string a, b;
		fin >> a >> b;
		if (i == 0)
			S = a[0];
		if (a == "epsilon")
			a = "\0";
		if (b == "epsilon")
			b = "\0";
		P.push_back({a, b});
	}
	string s;
	fin >> s;
	if (s.length() > 0)
		S = s;
	ContextFreeGrammar cfg(VT, VN, S, P);

	if (mode == "cacuset")
	{
		cacuSet(cfg);
	}
	else if (mode == "derivation")
	{
		derivation2(cfg, s_value);
	}
}
#elif _WIN32
int main(int argc, char *argv[])
{
	ifstream fin("./docs/sy4/sy4_2.txt", ios::in);

	int n, t, p;
	fin >> n >> t >> p;
	vector<char> VT;
	vector<string> VN;
	vector<PSS> P;
	string S;
	for (int i = 0; i < n; i++)
	{
		string s;
		fin >> s;
		VN.push_back(s);
	}
	for (int i = 0; i < t; i++)
	{
		string s;
		fin >> s;
		if (s == "epsilon")
			VT.push_back('\0');
		else
			VT.push_back(s[0]);
	}
	for (int i = 0; i < p; i++)
	{
		string a, b;
		fin >> a >> b;
		cout << a << "->" << b << endl;
		if (i == 0)
			S = a[0];
		if (a == "epsilon")
			a = "\0";
		if (b == "epsilon")
			b = "\0";
		P.push_back({a, b});
	}
	string s;
	fin >> s;
	if (s.length() > 0)
		S = s;
	ContextFreeGrammar cfg(VT, VN, S, P);

	cacuSet(cfg);
	string tt;
	cin >> tt;
	cout << "dev2" << endl;
	derivation2(cfg, tt);
}
#endif