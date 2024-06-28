#pragma once
#include<vector>
#include<string>
#include<map>
#include<iostream>
#include<stack>
#include"_baisc.h"
using namespace std;
typedef pair<string, string> PSS;
typedef pair<char, string> PCS;
class ParseGrammar;
class Grammar
{
protected:
    
    vector<char> VT,VN;
    vector<string> VT_o, VN_o;
    string S_o;
    char S;
    map<char, int> ys_N,ys_T;
    map<string, char> ys_s2c;
    map<char, string> ys_c2s;
    
    //vector<pair<string,string>> P;
    
public:
    friend class ParseGrammar;
    Grammar(); // 默认构造函数

    Grammar(vector<char> vt, vector<string> vn, string s); // 带参数的构造函数

    vector<char> getVT() const; // 获取终结符集合
    vector<string> getVN() const; // 获取非终结符集合

    string getS() const; // 获取开始符号
    string transc2s(string s);
    string transs2c(string s);
    // vector<pair<string, string>> getP() const; // 获取产生式集合


};
struct LL1Row
{
    string stack;
    string sentence;
    string action;
};

class ContextFreeGrammar :
    public Grammar
{
private:
    friend class parseGrammar;
protected:
    
    vector<pair<int, string>> P;
    vector<int> toEpsilon;
    vector<vector<int>> FIRST,FOLLOW,SELECT;
    vector<vector<vector<int>>> ys_SELECT;
    bool isll1;
    
    void deleteHarmful();
    void deleteUseless();
    ContextFreeGrammar compress();
    void canGetEpsilon_graph();
    void getFIRST_loop();
    void getFIRST_graph();
    void getFOLLOW_loop();
    void getFOLLOW_graph1();
    void getFOLLOW_graph2();
    void getSELECT_loop();
    bool isLL1();
public:
    ContextFreeGrammar();
    vector<vector<string>> getys_SELECT();
    vector<int> canGetEpsilon();
    vector<pair<string, vector<char>>> getFISRT();
    vector<char> getFIRST(string s);
    vector<char> getFIRST(char c);
    vector<char> getFOLLOW(char c);
    vector<pair<string, vector<char>>> getFOLLOW();
    vector<pair<PSS,vector<char>>> getSELECT();
    vector<char> getSELECT(int idx);
    vector<char> getSELECT(char c,string beta);

    bool isLL_1() const;
    string LL1(string _s, vector<PCS>& res);
    vector<LL1Row> LL1(string _s);
    bool ifToEpsilon(char c);
    bool ifToEpsilon(string c);
    bool isTerminaled(char c);
    bool isNonTerminaled(char c);
    bool inV(char c);
    ContextFreeGrammar(vector<char> vt, vector<string> vn, string s, vector<pair<string, string>> p);
    //friend double testFirst(ContextFreeGrammar& cfg);
    //friend double testFollow(ContextFreeGrammar& cfg);
    //friend double testFirst2(ContextFreeGrammar& cfg);
    //friend double testFollow2(ContextFreeGrammar& cfg);
    void print();
    void print2();
    vector<char> getVN();
    vector<char> getVT();
    /*LR*/
public:
    
protected:
    vector<vector<int>> gLR0;
};

class ContextFreeGrammar_LR
    :public ContextFreeGrammar
{
   
        
    
};