#include<iostream>
#include<set>
#include<queue>
#include<map>
#include<vector>
#define null -1e9
using namespace std;

class ReturnInt
{
	vector<int> vi;
	int i;
public:
	ReturnInt(int _i = 0);
	ReturnInt(vector<int> _vi);
	operator int();
	operator vector<int>();
};


class FA				//���������Զ���
{
protected:
	vector<int> K, Z;			//״̬���Ϻ���ֹ����
	vector<bool> isEnd;			//��ʶ״̬i�Ƿ�����ֹ״̬
	vector<char> Sigma;			//�ַ�����
	int n, m;
	map<char, int> ys;
public:
	FA(vector<int> _K, vector<char> _Sigma,vector<int> _Z);
	virtual ReturnInt next(int u, char c) = 0;
	virtual void paint() const = 0;
	int getM() const;
	int getN() const;
	bool IsEnd(int t);
	vector<char> getSigma() const;
	virtual bool check(string s) = 0;
};


class NondeterministicFiniteAutomata		//NFA
	:public FA
{
protected:
	vector<int> S;							//��ʼ״̬����
	vector<vector<vector<int>>> f;			//��ά��������״̬����
	bool input(string& s, int u,int now);
public:
	NondeterministicFiniteAutomata(vector<int> _K, vector<char> _Sigma, vector<vector<vector<int>>> _f, vector<int>_S, vector<int>_Z);
	virtual ReturnInt next(int u, char c);
	virtual vector<int> closure(vector<int> I);
	virtual vector<int> move(vector<int> I, char c);
	vector<int> getStart() const;
	void paint() const;
	virtual bool check(string s);
};

class DeterministicFiniteAutomata		//DFA
	:public FA
{
	int S;								//��ʼ״̬
	vector<vector<int>> f;				//��ά��������״̬
public:
	DeterministicFiniteAutomata(vector<int> _K, vector<char> _Sigma, vector<vector<int>> _f, int S, vector<int> _Z);
	virtual ReturnInt next(int u, char c);
	void paint(string pngname) const;
	void paint() const;
	static vector<vector<int>> getI(vector<vector<int>>& K);
	DeterministicFiniteAutomata minimize();
	virtual bool check(string s);
};


DeterministicFiniteAutomata Determine(NondeterministicFiniteAutomata& dfa);