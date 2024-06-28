#include "semanticsAnalysis.h"

vector<pair<string, string>> transP(vector<semanticsAction> _p)
{
	vector<pair<string, string>> ps;
	for (auto item : _p)	ps.push_back(item);
	return ps;
}


vector<char> transVT(vector<dc> _vt)
{
	vector<char> vns;
	for (auto item : _vt)	vns.push_back(item);
	return vns;
}

semanticsAnalysis_LR::semanticsAnalysis_LR(vector<char> vt, vector<string> vn, string s, vector<semanticsAction> p)
	:Grammar_LR1(vt,vn,s,transP(p))
{
	sactions.resize(P.size());
	valdic.resize(chars.size());
	actions.resize(P.size());
	for (int i = 0; i < p.size(); i++)
	{
		string nowleft = transs2c(p[i].left);
		string nowright = transs2c(p[i].right);
		if (nowleft.length() > 1)	continue;
		for (int j = 0; j < P.size(); j++)
		{
			if (P[j].first == ys_N[nowleft[0]] && nowright == P[j].second)
			{
				if(!p[i].actions.empty())
					sactions[j] = p[i].actions[0],actions[j] = parseExpression(transs2c(p[i].actions[0]),P[j]);
				break;
			}
		}
	}
}

vector<LR_S_ROW> semanticsAnalysis_LR::LR1_withSemantic(string s)
{
	if (isLR1 == false) {
		LR_S_ROW row;
		row.input = s;
		row.signal_stack = "#";
		row.state_stack = "0";
		row.action = "error:该文法不是LR1文法";
		return { row };
	}
	return LR_withSemantic(s, table_LR1);
}




vector<LR_S_ROW> semanticsAnalysis_LR::LR_withSemantic(string s, vector<vector<vector<int>>> table)
{
	vector<dc> input = CFFX(s + "#");
	vector<LR_S_ROW> res;

	vector<int> state_stack;
	vector<int> signal_stack;
	vector<vector<int>> semantics_stack;
	state_stack.push_back(0);
	signal_stack.push_back(ys_LRTABLE['#']);
	int now = 0;
	int et = 0;
	semantics_stack.push_back({});		// 初始化语义栈,推入-
	while (!et)
	{
		LR_S_ROW row;
		row.input = transSignalStack(input,now);
		row.signal_stack = LR_i2s(signal_stack);
		row.state_stack = transStateStack(state_stack);
		row.semantics_stack = transSemanticsStack(semantics_stack);
		int top = state_stack.back();	// 取状态栈栈顶
		char c = input[now];			// 取输入串的第一个字符
		if (!isTerminaled(c) && c != '#')	// 
		{
			row.action = "error:该符号不在符号表中";
			et = 1;
			res.push_back(row);
			break;
		}
		int idc = ys_LRTABLE[c];
		if (table[top][idc].empty())	// Table[X][c] = Ø
		{
			row.action = "SyntaxError";	// 语法错误
			et = 1;
		}
		else {
			if (table[top][idc][0] > 0)	// 移进项目
			{
				signal_stack.push_back(idc);	// 符号c压入符号栈
				state_stack.push_back(table[top][idc][0]);	// 新状态压入状态栈
				if (input[now].hasv)			// 如果该终结符带有属性，就压入状态栈
				{
					semantics_stack.push_back({input[now].value});
				}
				else							// 否则状态栈压入一个'-'，也就是空
				{
					semantics_stack.push_back({});
				}
				row.action = transStateStack({ table[top][idc][0] });
				now++;			// 输入符号串前移
			}
			else if (table[top][idc][0] < 0)		// 规约项目
			{
				int idx = -table[top][idc][0];
				row.action = transStateStack({ table[top][idc][0] });	// 规约操作
				int len = P[idx].second.length();	// |β|
				vector<vector<int>> tmp;
				vector<int> equals;
				for (int i = semantics_stack.size() - len; i < semantics_stack.size(); i++)
				{
					tmp.push_back(semantics_stack[i]);
				}
				semantics_stack.resize(semantics_stack.size() - len);
				state_stack.resize(state_stack.size() - len);	// 弹出|β|个
				signal_stack.resize(signal_stack.size() - len);
				for (int i = 0; i < actions[idx].size(); i++)
				{
					if (actions[idx][i].cato == 1 && actions[idx][i].values[0] == 0) equals.push_back(i);			// 寻找等号=的位置
				}
				if (equals.empty())						// 没有=号，说明该非终结符没有属性，推入'-'
				{
					semantics_stack.push_back({});
				}
				else
				{
					vector<int> res(equals.size(), 0);

					for (int i = 0; i < equals.size(); i++)
					{
						vector<WORD> tmp2;
						for (int j = equals[i] + 1; j < (i + 1 == equals.size() ? actions[idx].size() : equals[i + 1] - 1); j++)
						{
							tmp2.push_back(actions[idx][j]);
						}
						res[actions[idx][equals[i] - 1].values[1]] = cacu(tmp2, tmp);			// 等号=右边的计算结果赋给等号左边的位置
					}
					semantics_stack.push_back(res);
				}
				

				int appended = ys_LRTABLE[VN[P[idx].first]];	// 产生左部的A
				signal_stack.push_back(appended);		// 放入符号栈
				int nowtop = state_stack.back();
				row.semantics_action = sactions[idx];
				if (table[nowtop][appended].empty())	// 取Table[X'][A]
				{
					row.GOTO = "SyntaxError";		// 空则报语法错误
					et = 1;
				}
				else
				{
					state_stack.push_back(table[nowtop][appended][0]);		// 否则把这个Table[X'][A]压入状态栈
					row.GOTO = transStateStack({ table[nowtop][appended][0] });
				}
			}
			else
			{
				row.semantics_action = sactions[0];
				row.action = "acc";
				et = 1;
			}
		}
		res.push_back(row);
	}
	return res;
}

vector<LR_S_ROW> semanticsAnalysis_LR::LR_withSemantic(string s, string which)
{
	if (which == "SLR1")
	{
		return LR_withSemantic(s, table_SLR1);
	}
	else if (which == "LR0")
	{
		return LR_withSemantic(s, table_LR0);
	}
	else
	{
		return LR_withSemantic(s, table_LR1);
	}
}

map<string, int> ops = { {"=",0}, { "+",1 },{"-",2},{"*",3},{"/",4} };

vector<WORD> semanticsAnalysis_LR::parseExpression(string action_,pair<int,string> p)
{
	
	vector<string> actions;
	vector<int> dh;
	for (int i = 0; i < action_.size(); i++) {
		if (action_[i] == ',')dh.push_back(i);
	}

	for(int i=0;i<dh.size();i++)
	{
		if(i == 0) actions.push_back(action_.substr(0, dh[i]));
		else actions.push_back(action_.substr(dh[i - 1] + 1, dh[i] - dh[i - 1] - 1));
	}
	if(dh.empty()) actions.push_back(action_);
	else if(dh.back() != action_.size() - 1) actions.push_back(action_.substr(dh.back() + 1));
	vector<vector<int>> poses;
	poses.resize(chars.size());
	poses[ys_LRTABLE[VN[p.first]]].push_back(0);
	for (int i=0;i< p.second.size();i++)
	{
		if(p.second[i] == ' ' || p.second[i] == '\0')	continue;
		if(isNonTerminaled(p.second[i]))
			poses[ys_LRTABLE[(p.second)[i]]].push_back(i);
		else if(isTerminaled(p.second[i]))
			poses[ys_LRTABLE[(p.second)[i]]].push_back(i);
	}
	vector<WORD> sentence;
	for(auto action:actions)
	{
		string now;
		for (int i = 0; i < action.length(); i++)			// 扫描语义动作字符串
		{
			
			if (ops.find(action.substr(i, 1)) != ops.end()) {
				if (now == "")
					now.push_back(action[i]);
				WORD word;
				if (now.find('.') != -1)			// 如果是R.val类型
				{
					int pos = now.find('.');
					string l = now.substr(0, pos), r = now.substr(pos);
					char N = l[0]; int Nid = ys_LRTABLE[N];
					int idx = 0;
					if (l.length() > 1)
					{
						idx = stoi(l.substr(1));
					}
					string val = now.substr(pos + 1);
					word.cato = 0;
					word.values.assign(2, 0);
					word.values[0] = poses[Nid][idx];	//Nid 是对应的符号的编号，idx是第idx次出现，比如R,R_1
					// posed存放每个非终结符在产生式右部中的位置
					// 找到该属性对应在栈中的位置
					if (valdic[Nid].find(val) == valdic[Nid].end())		// 如果val在valdic中，为该非终结符添加一个新属性
					{
						int nowsize = valdic[Nid].size();
						valdic[Nid][val] = nowsize;
						word.values[1] = nowsize;
					}
					else word.values[1] = valdic[Nid][val];	// 如果val在valdic中，则直接取出
					i--;
				}
				else if (ops.find(now) != ops.end())		// 如果是操作符类型
				{
					word.cato = 1;
					word.values.assign(1, 0);
					word.values[0] = ops[now];
				}
				else										// 常量类型
				{
					word.cato = 2;
					word.values.assign(1, 0);
					word.values[0] = stoi(now);
					i--;
				}
				sentence.push_back(word);
				now = "";

			}
			else now.push_back(action[i]);
		}
		if (now != "")
		{
			WORD word;
			if (now.find('.') != -1)
			{
				int pos = now.find('.');
				string l = now.substr(0, pos), r = now.substr(pos);
				char N = l[0]; int Nid = ys_LRTABLE[N];
				int idx = 0;
				if (l.length() > 1)
				{
					idx = stoi(l.substr(1));
				}
				string val = now.substr(pos+1);
				word.cato = 0;
				word.values.assign(2, 0);
				word.values[0] = poses[Nid][idx];
				if (valdic[Nid].find(val) == valdic[Nid].end())
				{
					int nowsize = valdic[Nid].size();
					valdic[Nid][val] = nowsize;
					word.values[1] = nowsize;
				}
				else word.values[1] = valdic[Nid][val];
			}
			else if (ops.find(now) != ops.end())
			{
				word.cato = 1;
				word.values.assign(1, 0);
				word.values[0] = ops[now];
			}
			else
			{
				word.cato = 2;
				word.values.assign(1, 0);
				word.values[0] = stoi(now);

			}
			sentence.push_back(word);
		}
	}
	
	return sentence;
}

string semanticsAnalysis_LR::transSemanticsStack(vector<vector<int>> I)
{
	string res;
	for (auto item : I)
	{
		if (item.size() == 0)
		{
			res.push_back('-');
		}
		else if (item.size() == 1 && item[0] >= 0 && item[0] < 10)
		{
			res.push_back((item[0] + '0'));
		}
		else
		{
			string tmp = "(";
			for (int i = 0; i < item.size() - 1; i++)
			{
				tmp += to_string(item[i]) + ",";
			}
			tmp += to_string(item.back()) + ")";
			res += tmp;
		}
	}
	return res;
}
//map<string, int> ops = { {"=",0}, { "+",1 },{"-",2},{"*",3},{"/",4} };
int semanticsAnalysis_LR::cacu(vector<WORD> action, vector<vector<int>>& items)
{
	int n = items.size();
	WORD tmp;
	tmp.cato = 1; tmp.values.assign(1, -1);
	action.push_back(tmp);
	vector<int> num_stack;
	vector<int> op_stack;
	op_stack.push_back(-1);
	map<int, int> priority = { {-1,0},{0,1},{1,2},{2,2},{3,3},{4,3} };
	int pointer = 0;
	while (!(op_stack.empty() && action.empty()))		// 符号栈空
	{
		if (op_stack.back() == -1 && pointer >= action.size())	
		{
			return num_stack.back();
		}
		char op1 = op_stack.back();
		if (action[pointer].cato == 0)		// 如果是数字(属性)
		{
			int idx = action[pointer].values[0]; int pos = action[pointer].values[1];
			int val = items[idx][pos];
			num_stack.push_back(val);
			pointer++;
		}
		else if (action[pointer].cato == 2)		// 如果是常数
		{
			num_stack.push_back(action[pointer].values[0]);
			pointer++;
		}
		else
		{
			char op2 = action[pointer].values[0];	
			if (op1 == -1 && op2 == -1)		// 如果都是'#'
			{
				break;
			}
			if (priority[op1] < priority[op2])	// 比较栈顶操作符和当前操作符的优先级
			{
				op_stack.push_back(op2);
				pointer++;
			}
			else				// 如果是当前优先级比较大，就计算
			{
				int num2 = num_stack.back(); num_stack.pop_back();
				int num1 = num_stack.back(); num_stack.pop_back();
				int res = 0;
				if (op1 == 1)	res = num1 + num2;
				else if (op1 == 2)	res = num1 - num2;
				else if (op1 == 3)	res = num1 * num2;
				else if (op1 == 4)	res = num1 / num2;
				num_stack.push_back(res);
				op_stack.pop_back();
			}
		}
	}
	return num_stack.back();
}

string semanticsAnalysis_LR::transSignalStack(vector<dc> I, int now)
{
	string res;
	for(int i=now;i<I.size();i++)
	{
		if (I[i].hasv)
		{
			res += to_string(I[i].value);
		}
		else
		{
			res.push_back(I[i].signal);
		}
		//if (I[i].signal == '\0')	return "#";
		//return string(I[i].signal, 1);
	}
	return res;
}



vector<dc> CFFX(string s)
{
	vector<dc> res;
	for (int i = 0; i < s.length(); i++)
	{

		if (s[i] == ' ') {
			continue;
		}
		dc now;
		if (s[i] <= '9' && s[i] >= '0')		// 扫描到数字
		{
			int num = s[i] - '0';
			while (i + 1 < s.length() && s[i + 1] <= '9' && s[i + 1] >= '0')		// 把所有数字都扫描完
			{
				i++;
				num = num * 10 + s[i] - '0';	
			}
			
			now.signal = 'd';		// 符号的类型
			now.value = num;		// 存数字，符号的值
			now.hasv = 1;
		}
		else					// 非数字的就保留
		{
			now.signal = s[i];
			now.value = 0;
			now.hasv = 0;
		}
		res.push_back(now);
	}
	return res;
}


