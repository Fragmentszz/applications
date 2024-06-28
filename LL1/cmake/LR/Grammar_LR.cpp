#include"Grammar_LR.h"

char Grammar_LR::getChar_inner(int i)
{
	return chars[i];
}

string Grammar_LR::getChar_outer(int i)
{
	if (isNonTerminaled(chars[i]))
		return ys_c2s[chars[i]];
	
	else
		return string(1,chars[i]);
}

string Grammar_LR::transStateStack(vector<int> I)
{
	string res;
	for (int i : I)
	{
		if(i < 10 || I.size() == 1)
			res += to_string(i);
		else
			res += '(' + to_string(i) + ')';
	}
	return res;
}

string Grammar_LR::LR_i2s(vector<int> I)
{
	string res;
	for (int i : I)
	{
		res += getChar_outer(i);
	}
	return res;
}

Grammar_LR::Grammar_LR(vector<char> vt, vector<string> vn, string s, vector<pair<string, string>> p, bool augmented)
	:ContextFreeGrammar(vt,vn,s,p),isLR0(1),isSLR1(1),hasepsilon(0)
{

}

vector<int> Grammar_LR::closure(vector<int> I)
{
	vector<int> res = bfs(I,g_closure);	
	return res;
}

vector<int> Grammar_LR::GO(vector<int> I, char c)
{
	if(c == '\0') return closure(I);
	vector<int> res;
	for (int i : I)			// 遍历该项目集中的所有项目
	{
		if (state[i].next == c)	//如果有项目的下一个字符是c
		{
			res.push_back(i + 1);		//将该项目的下一个项目加入结果集[编号一定是i+1]
		}
	}
	return closure(res);		// 再求闭包
}





void Grammar_LR::initLR()
{
	lx; lx.resize(VN.size());
	
	int cnt = 0;
	for (int i = 0; i < P.size(); i++)
	{
		lx[P[i].first].push_back(i);
		string beta = P[i].second;
		P0.push_back(cnt);  // 记录每一个产生式的第一个项目的编号
		for (int j = 0; j < beta.length(); j++)
		{
			STATE s; s.idx = i; s.pos = j; s.next = beta[j];
			state.push_back(s);							// 通过产生式创建项目
			cnt++;
		}
		cnt++;
		STATE s; s.idx = i; s.pos = beta.length(); s.next = '\0';
		state.push_back(s);			
	}
	g_closure.resize(state.size());
	for (int i = 0; i < state.size(); i++)
	{
		int idx = state[i].idx; int pos = state[i].pos; char next = state[i].next;
		state[i].canReduce = state[i].next == '\0';
		if (isNonTerminaled(next))    // 项目i的下一个字符是个非终结符
		{
			int leftid = ys_N[next];		
			for (int j : lx[leftid])		// 找到以该字符为左部的所有产生式
			{
				g_closure[i].push_back(P0[j]);		// 通过刚才记录的P0找到这些产生式的第一个项目
			}
		}
	}
	I0 = closure({ 0 });

	for (int i = 0; i < VT.size(); i++) {
		if (VT[i] != '\0')
			chars.push_back(VT[i]);
		else hasepsilon = 1;
	}
	chars.push_back('#');
	for (int i = 0; i < VN.size(); i++) if (VN[i] != S) chars.push_back(VN[i]);
	for (int i = 0; i < chars.size(); i++)
	{
		ys_LRTABLE[chars[i]] = i;
	}
}

void Grammar_LR::init_LR0()
{
	DFAState.clear();
	DFAState.push_back(I0);	// 求出初始状态的闭包作为初始项目集
	int pointer = 0;
	bool changed = 1;
	int m = chars.size();
	vector<vector<int>> newrow; newrow.resize(m);
	table_LR0.push_back(newrow);
	while (pointer < DFAState.size())	// 使用指针遍历项目集族，如果遍历完就说明没有新的项目集产生，退出
	{
		vector<vector<int>>& row = table_LR0[pointer];
		changed = 0;
		vector<int> nowstate = DFAState[pointer];
		vector<bool> vis(m, 0);
		for (int u : nowstate)		// 遍历项目集中的所有项目
		{
			if (state[u].canReduce)		//如果是可规约项目
			{
				for (int i = 0; i <= VT.size(); i++)
				{
					
					int idchar = -1;
					if (i < VT.size())
					{
						idchar = ys_LRTABLE[VT[i]];
						if (VT[i] == '\0')
						{
							continue;
						}
					}
					else idchar = VT.size() - hasepsilon;
					if (state[u].idx == 0)			// 如果使用初始产生式规约
					{
						if(i == VT.size()) table_LR0[pointer][idchar].push_back(0);		// 那么只有'#'上是可以接受的
					}
					else {
						table_LR0[pointer][idchar].push_back(-state[u].idx);		// 规约项目，记录产生式编号，加负号区分
						if (table_LR0[pointer][idchar].size() > 1) {
							isLR0 = 0;
						}
					}
				}
				continue;
			}
			char c = state[u].next;				// c是项目u的下一个字符
			int colomn = ys_LRTABLE[c];
			if (vis[colomn])	continue;
			vis[colomn] = 1;
			vector<int> nextstate = GO(nowstate, c);   // 求u的下一个字符的GO(I)项目集
			if (!nextstate.empty())		// 如果GO(I)不为空
			{
				bool same = 0;
				int nowk = -1;
				for (int k = 0; k < DFAState.size(); k++)
				{
					if (SAME(DFAState[k], nextstate)) {	// 如果GO(I)和已有的项目集比较
						same = 1;				// 如果相同，记录编号
						nowk = k;
						break;
					}
				}
				if (!same)		// 如果GO(I)和已有的项目集不同，加入新的项目集
				{
					nowk = DFAState.size();
					DFAState.push_back(nextstate);
					vector<vector<int>> newrow; newrow.resize(m);
					table_LR0.push_back(newrow);
					
				}
				table_LR0[pointer][colomn].push_back(nowk);		// 记录GOd的转移
				if (table_LR0[pointer][colomn].size() > 1) {
					isLR0 = 0;
				}
			}
		}
		pointer++;			// 指针后移
	}
}

void Grammar_LR::init_SLR1()
{
	DFAState.clear();
	DFAState.push_back(I0);
	int pointer = 0;
	bool changed = 1;
	int m = VT.size() + VN.size();
	vector<vector<int>> newrow; newrow.resize(m);
	table_SLR1.push_back(newrow);

	while (pointer < DFAState.size())
	{
		vector<vector<int>>& row = table_SLR1[pointer];
		changed = 0;
		vector<int> nowstate = DFAState[pointer];
		vector<bool> vis(m, 0);
		for (int u : nowstate)
		{
			if (state[u].canReduce)		//如果是可规约项目
			{
				int leftid = P[state[u].idx].first;
				for (int i = 0; i < FOLLOW[leftid].size(); i++)
				{
					if (FOLLOW[leftid][i])
					{
						int idchar = VT.size();
						if (i < VT.size())
							idchar = ys_LRTABLE[VT[i]];
						else idchar = VT.size() - hasepsilon;
						table_SLR1[pointer][idchar].push_back(-state[u].idx);
						if(table_SLR1[pointer][idchar].size() > 1) {
							isSLR1 = 0;
						}
					}
					
				}
				continue;
			}
			char c = state[u].next;
			int colomn = ys_LRTABLE[c];
			if (vis[colomn])	continue;
			vis[colomn] = 1;

			vector<int> nextstate = GO(nowstate, c);
			if (!nextstate.empty())
			{
				bool same = 0;
				int nowk = -1;
				for (int k = 0; k < DFAState.size(); k++)
				{
					if (SAME(DFAState[k], nextstate)) {
						same = 1;
						nowk = k;
						break;
					}
				}
				if (!same)
				{
					nowk = DFAState.size();
					DFAState.push_back(nextstate);
					vector<vector<int>> newrow; newrow.resize(m);
					table_SLR1.push_back(newrow);
					
				}
				table_SLR1[pointer][colomn].push_back(nowk);
				if(table_SLR1[pointer][colomn].size() > 1) {
					isSLR1 = 0;
				}
			}
		}
		pointer++;
	}
}

void Grammar_LR::printState(int i)
{
	int idx = state[i].idx; int pos = state[i].pos; char next = state[i].next;
	auto p = P[idx];
	string left = ys_c2s[VN[p.first]];
	string right = p.second;
	std::cout << left << "->";
	for (int j = 0; j < right.length(); j++)
	{
		if (j == pos) std::cout << "·";
		string tmp; tmp.push_back(right[j]);
		std::cout << transc2s(tmp);
	}
	if (pos == right.length()) std::cout << "·";
	std::cout << endl;
}

void Grammar_LR::print_tableLR(vector<vector<vector<int>>> table)
{
	cout << DFAState.size() << endl;
	for (int i = 0; i < DFAState.size(); i++)
	{
		std::cout << "I" << i << ":" << DFAState[i].size() << endl;
		for (int j : DFAState[i])
		{

			printState(j);
		}
	}
	cout << table.size() << " " << VT.size() + 1 - hasepsilon << " " << chars.size() - (VT.size() + 1 - hasepsilon) << endl;
	for (int i = 0; i < chars.size(); i++)
	{
		cout << chars[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < table.size(); i++)
	{
		for (int j = 0; j < table[i].size(); j++)
		{
			if(table[i][j].empty()) continue;
			std::cout << i << " " << j << ": ";
			std::cout << "{";
			for (int k=0;k<table[i][j].size()-1;k++)
			{
				std::cout << table[i][j][k] << ",";
			}
			std::cout << table[i][j].back() << "}" << endl;
		}
	}
}

void Grammar_LR::print_tableLR(string name)
{
	if (name == "LR0")
	{
		print_tableLR(table_LR0);
	}
	else if (name == "SLR1")
	{
		print_tableLR(table_SLR1);
	}
	if (name == "LR1")
	{
		print_tableLR(table_LR1);
	}
}

vector<LR_ROW> Grammar_LR::LR0(string s)
{
	if (isLR0 == false) {
		LR_ROW row;
		row.input = s;
		row.signal_stack = "#";
		row.state_stack = "0";
		row.action = "error:该文法不是LR0文法";
		return { row };
	}
	return LR_Analysis(s, table_LR0);
}

vector<LR_ROW> Grammar_LR::SLR1(string s)
{	
	
	if (isSLR1 == false) {
		LR_ROW row;
		row.input = s;
		row.signal_stack = "#";
		row.state_stack = "0";
		row.action = "error:该文法不是SLR1文法";
		return { row };
	}
	return LR_Analysis(s, table_SLR1);
}



vector<LR_ROW> Grammar_LR::LR_Analysis(string s, vector<vector<vector<int>>> table)
{
	vector<LR_ROW> res;
	s += '#';
	vector<int> state_stack;
	vector<int> signal_stack;
	state_stack.push_back(0);				// 把0压入状态栈
	signal_stack.push_back(ys_LRTABLE['#']);		// 把‘#’压入符号栈
	int now = 0;
	bool et = 0;
	while (!et)
	{
		LR_ROW row;
		row.input = s.substr(now);
		row.signal_stack = LR_i2s(signal_stack);
		row.state_stack = transStateStack(state_stack);
		int top = state_stack.back();	// 取状态栈栈顶
		char c = s[now];			// 取输入串的第一个字符
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
				row.action = transStateStack({ table[top][idc][0] });	
				now++;			// 输入符号串前移
			}
			else if (table[top][idc][0] < 0)		// 规约项目
			{
				int idx = -table[top][idc][0];
				row.action = transStateStack({ table[top][idc][0] });	// 规约操作
				int len = P[idx].second.length();	// |β|
				state_stack.resize(state_stack.size() - len);	// 弹出|β|个
				signal_stack.resize(signal_stack.size() - len);
				int appended = ys_LRTABLE[VN[P[idx].first]];	// 产生左部的A
				signal_stack.push_back(appended);		// 放入符号栈
				int nowtop = state_stack.back();
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
				row.action = "acc";
				et = 1;
			}
		}

		
		res.push_back(row);
	}

	return res;
}

Grammar_LR::Grammar_LR(vector<char> _vt, vector<string> _vn, string _s, vector<pair<string, string>> _p)
{
	string sp = "S'";
	if(_s[0] == 'S')  sp = _s + "'";	// 新的开始符号 加'
	vector<pair<string, string>> p;
	p.push_back({ sp,_s });			// 添加一个新的产生式： S'->S
	for (auto z : _p)
	{
		p.push_back(z);
	}
	vector<string> vn; vn.push_back(sp);
	for(auto z:_vn) vn.push_back(z);
	_vn.push_back(sp);
	*this = Grammar_LR(_vt, vn, sp, p, 1);

	initLR();
	init_LR0();
	init_SLR1();

}




Grammar_LR1::Grammar_LR1(vector<char> vt, vector<string> vn, string s, vector<pair<string, string>> p)
	:Grammar_LR(vt,vn,s,p),isLR1(1)
{
	init_LR1();
}

vector<int> Grammar_LR1::closure(vector<int> I)
{
	vector<int> vis(state.size() * (VT.size() + 1), 0);
	vector<int> res;
	queue<int> q;
	for (int i : I)
	{
		q.push(i);
	}
	while (!q.empty())
	{
		int f = q.front(); q.pop();
		if (vis[f]) continue;
		vis[f] = 1;
		res.push_back(f);
		LR1_STATE sa = decode(f);
		int idx = sa.idx, after = sa.after;
		int Pidx = state[idx].idx, pos = state[idx].pos;
		vector<int> first;
		if (pos == P[Pidx].second.length())
		{
			first.push_back(after);
		}
		else
		{
			string hz = P[Pidx].second.substr(pos + 1);
			if (ifToEpsilon(hz))
			{
				first.push_back(after);
			}
			vector<char> cs = getFIRST(hz);
			for (char c : cs)
			{
				if (c == '\0')	continue;
				first.push_back(ys_T[c]);
			}
		}
		for (int u : g_closure[idx])
		{
			for (int fir : first)
			{
				int now = encode(u, fir);
				if (now >= state.size() * (VT.size() + 1))
				{ 
					cout << u << " " << fir << endl;
				}
				q.push(encode(u, fir));
			}
		}
	}
	return res;
}

void Grammar_LR1::printState(int i)
{
	LR1_STATE sa = decode(i);
	int idx = sa.idx, after = sa.after;

	int Pidx = state[idx].idx; int pos = state[idx].pos; char next = state[idx].next;
	auto p = P[Pidx];
	string left = ys_c2s[VN[p.first]];
	string right = p.second;
	std::cout << left << "->";
	for (int j = 0; j < right.length(); j++)
	{
		if (j == pos) std::cout << "·";
		string tmp; tmp.push_back(right[j]);
		std::cout << transc2s(tmp);
	}
	if (pos == right.length()) std::cout << "·";
	if(after < VT.size())
		std::cout <<" ," << VT[after] << endl;
	else
		std::cout << " ,#" << endl;

}

LR1_STATE Grammar_LR1::decode(int num)
{
	if (num >= state.size() * (VT.size() + 1))
	{
		cout << num << endl;
		cout << num << "产生:" << num / (VT.size() + 1) << " " << num % (VT.size() + 1) << endl;
	}
	
	return LR1_STATE(num / (VT.size() + 1),num % (VT.size() + 1));
}

int Grammar_LR1::encode(int idx, int after)
{
	return idx * (VT.size() + 1) + after;
}


void Grammar_LR1::init_LR1()
{
	DFAState.clear();
	DFAState.push_back(closure({ encode(0,VT.size())}));		// 'S'->·S '#'
	int k = 0;
	vector<vector<int>> row0;
	row0.resize(chars.size());
	while (k < DFAState.size())
	{
		vector<int> nowstate = DFAState[k];
		vector <vector<int>> newrow = row0;
		table_LR1.push_back(newrow);
		vector<bool> vis(chars.size(),0);
		for (int i : nowstate)
		{
			LR1_STATE sa = decode(i);
			int idx = sa.idx, after = sa.after;
			char c = '#';
			char nxt = state[idx].next;
			int Pidx = state[idx].idx;
			if (after < VT.size())	c = VT[after];
			if (state[idx].canReduce)
			{
				table_LR1[k][ys_LRTABLE[c]].push_back(- Pidx);
				if(table_LR1[k][ys_LRTABLE[c]].size() > 1) {
					isLR1 = 0;
				}
			}
			else
			{
				if (vis[ys_LRTABLE[nxt]])	continue;
				vis[ys_LRTABLE[nxt]] = 1;
				vector<int> newstate = GO(nowstate, nxt);
				bool same = 0;
				int zy = -1;
				for (int j = 0; j < DFAState.size(); j++)
				{
					if (SAME(newstate, DFAState[j]))
					{
						same = 1;
						zy = j;
						break;
					}
				}
				if (!same)
				{
					zy = DFAState.size();
					DFAState.push_back(newstate);
				}
				table_LR1[k][ys_LRTABLE[nxt]].push_back(zy);
				if (table_LR1[k][ys_LRTABLE[nxt]].size() > 1) {
					isLR1 = 0;
				}
			}
		}
		k++;
	}
}

vector<int> Grammar_LR1::GO(vector<int> I, char c)
{
	if (c == '\0') return this->closure(I);
	vector<int> res;
	for (int i : I)			// 遍历该项目集中的所有项目
	{
		LR1_STATE sa = decode(i);
		int idx = sa.idx, after = sa.after;
		if (state[idx].next == c)	//如果有项目的下一个字符是c
		{
			res.push_back(encode(idx + 1,after));		//将该项目的下一个项目加入结果集[编号一定是i+1]
		}
	}
	return this->closure(res);		// 再求闭包
}

vector<LR_ROW> Grammar_LR1::LR1(string s)
{
	if (isLR1 == false) {
		LR_ROW row;
		row.input = s;
		row.signal_stack = "#";
		row.state_stack = "0";
		row.action = "error:该文法不是LR1文法";
		return { row };
	}
	return LR_Analysis(s, table_LR1);
}
