#pragma once
#include"Grammar_LR.h"


struct WORD
{
	int cato;		// Àà±ð
	vector<int> values;		// Öµ
	WORD():cato(-1){}
	~WORD() {  }
};
struct semanticsAction
{
	string left;
	string right;
	vector<string> actions;
	operator pair<string, string>()
	{
		return pair<string, string>(left, right);
	}
	semanticsAction(string _l,string _r, vector<string> _a)
		:left(_l),right(_r),actions(_a)
	{

	}
};
struct dc
{
	char signal;
	int value;
	bool hasv;
	operator char()
	{
		return signal;
	}
};

struct LR_S_ROW
{
	string state_stack;
	string signal_stack;
	string semantics_stack;
	string input;
	string action;
	string GOTO;
	string semantics_action;
};
vector<dc> CFFX(string s);
struct saction
{
	int Pidx;
	string action;
};
class semanticsAnalysis_LR
	:Grammar_LR1
{
public:
	semanticsAnalysis_LR(vector<char> vt, vector<string> vn, string s, vector<semanticsAction> p);
	vector<LR_S_ROW> LR1_withSemantic(string s);
	vector<LR_S_ROW> LR_withSemantic(string s, vector<vector<vector<int>>> table);
	vector<LR_S_ROW> LR_withSemantic(string s, string which);
protected:
	vector<string> sactions;
	vector<vector<WORD>> actions;
	vector<map<string,int>> valdic;
	vector<WORD> parseExpression(string action,pair<int,string> P);
	string transSemanticsStack(vector<vector<int>> I);
	int cacu(vector<WORD> action,vector<vector<int>>& items);
	string transSignalStack(vector<dc> I,int now);
};

vector<dc> CFFX(string s);