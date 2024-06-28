#pragma once
#include"Grammar.h"

struct STATE
{
	int idx;	//产生式标号
	int pos;	//当前圆点位置
	char next;	//下一个字符
	bool canReduce;	//是否是规约项目
};

struct LR_ROW
{
	string action;
	string GOTO;
	string signal_stack;
	string state_stack;
	string input;
};

class Grammar_LR :
	public ContextFreeGrammar
{
public:
	Grammar_LR(vector<char> vt, vector<string> vn, string s, vector<pair<string, string>> p);
	virtual vector<int> closure(vector<int> I);
	virtual vector<int> GO(vector<int> i, char c);
	void initLR();
	void init_LR0();
	void init_SLR1();
	virtual void printState(int i);
	void print_tableLR(vector<vector<vector<int>>> table);
	void print_tableLR(string name);
	vector<LR_ROW> LR0(string s);
	vector<LR_ROW> SLR1(string s);

protected:
	vector<LR_ROW> LR_Analysis(string s, vector<vector<vector<int>>> table);
	bool isLR0;
	bool isSLR1;
	// bool isLR1;
	int hasepsilon = 0;
	vector<char> chars;
	map<char, int> ys_LRTABLE;
	vector<vector<int>> g_closure;
	vector<vector<int>> DFAState; 
	vector<vector<int>> g_DFA;
	vector<vector<vector<int>>> table_LR0;
	vector<vector<vector<int>>> table_SLR1;
	vector<vector<vector<int>>> table_LR1;
	vector<STATE> state;
	char getChar_inner(int i);
	string getChar_outer(int i);
	string transStateStack(vector<int> I);
	string LR_i2s(vector<int> I);
	vector<int> P0;
	vector<int> I0;
	vector<vector<int>> lx;
	Grammar_LR(vector<char> vt, vector<string> vn, string s, vector<pair<string, string>> p, bool augmented);
};

struct LR1_STATE
{
	int idx;
	int after;

	LR1_STATE(int _idx, int _after)
		:idx(_idx),after(_after)
	{

	}
};

class Grammar_LR1 :
	public Grammar_LR
{
public:
	Grammar_LR1(vector<char> vt, vector<string> vn, string s, vector<pair<string, string>> p);
	vector<LR_ROW> LR1(string s);
protected:
	// vector<int> LR1_bfs(vector<int> I);
	virtual void printState(int i);
	LR1_STATE decode(int num);
	int encode(int idx, int after);
	bool isLR1;
	void init_LR1();
	virtual vector<int> closure(vector<int> I);
	virtual vector<int> GO(vector<int> i, char c);
	
};