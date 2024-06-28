#include "Grammar_LR.h"
#include "semanticsAnalysis.h"
#include <fstream>
#include <sstream>
using namespace std;

int parse(int argc, char *argv[], string &filepath, string &mode, string &s_value, bool &isseman)
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
        else if (arg.find("-semantic=" == 0))
        {
            string tmp = arg.substr(10);
            if (tmp == "true")
            {
                isseman = 1;
            }
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
    vector<pair<string, vector<char> > > first = cfg.getFISRT();
    vector<pair<string, vector<char> > > follow = cfg.getFOLLOW();
    vector<pair<PSS, vector<char> > > select = cfg.getSELECT();

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
    vector<vector<string> > table = cfg.getys_SELECT();
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

void getLR_table(Grammar_LR &cfg, string which)
{
    cfg.print_tableLR(which);
}

void LR0_Analysis(Grammar_LR &cfg, string str)
{
    vector<LR_ROW> rows = cfg.LR0(str);
    for (auto row : rows)
    {
        cout << row.state_stack << " " << row.signal_stack << " " << row.input << " " << row.action << " " << row.GOTO << endl;
    }
}

void SLR1_Analysis(Grammar_LR &cfg, string str)
{
    vector<LR_ROW> rows = cfg.SLR1(str);
    for (auto row : rows)
    {
        cout << row.state_stack << " " << row.signal_stack << " " << row.input << " " << row.action << " " << row.GOTO << endl;
    }
}
void LR1_Analysis(Grammar_LR1 &cfg, string str)
{
    vector<LR_ROW> rows = cfg.LR1(str);
    for (auto row : rows)
    {
        cout << row.state_stack << " " << row.signal_stack << " " << row.input << " " << row.action << " " << row.GOTO << endl;
    }
}


void LR1_semantics_Analysis(semanticsAnalysis_LR cfg, string str)
{
    vector<LR_S_ROW> rows = cfg.LR1_withSemantic(str);
    for (auto row : rows)
    {
        cout << row.state_stack << " " << row.signal_stack << " " << row.semantics_stack << " " << row.input << " " << row.action << " " << row.GOTO << " " << row.semantics_action << endl;
    }
}

void SLR1_semantics_Analysis(semanticsAnalysis_LR cfg, string str)
{
    vector<LR_S_ROW> rows = cfg.LR_withSemantic(str, "SLR1");
    for (auto row : rows)
    {
        cout << row.state_stack << " " << row.signal_stack << " " << row.semantics_stack << " " << row.input << " " << row.action << " " << row.GOTO << " " << row.semantics_action << endl;
    }
}
void LR_semantics_Analysis(semanticsAnalysis_LR cfg, string str,string mode)
{
    vector<LR_S_ROW> rows = cfg.LR_withSemantic(str, mode);
    for (auto row : rows)
    {
        cout << row.state_stack << " " << row.signal_stack << " " << row.semantics_stack << " " << row.input << " " << row.action << " " << row.GOTO << " " << row.semantics_action << endl;
    }
}
void LR_Analysis(Grammar_LR1 &cfg, string str, string mode)
{
    if (mode == "LR0")
        LR0_Analysis(cfg, str);
    else if (mode == "SLR1")
        SLR1_Analysis(cfg, str);
    else if (mode == "LR1")
        LR1_Analysis(cfg, str);
}
#ifdef __linux__
int main(int argc, char *argv[])
{
    string filepath, mode, s_value;
    bool isseman = 0;
    int parseResult = parse(argc, argv, filepath, mode, s_value, isseman);
    if (parseResult)
    {
        return parseResult;
    }
    ifstream fin(filepath, ios::in);
    int n, t, p;
    fin >> n >> t >> p;
    vector<char> VT;
    vector<string> VN;
    vector<pair<string, string> > P;
    vector<semanticsAction> P2;
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
    string tmp;
    getline(fin, tmp);
    for (int i = 0; i < p; i++)
    {
        string line;
        getline(fin, line);
        string now;
        stringstream ss;
        ss << line;

        string a, b;
        ss >> a;
        vector<string> actions;
        string tmpaction;
        bool inaction = 0;
        while (ss >> now)
        {
            if (now == "")
            {
                break;
            }
            if (now[0] == '{' && now.back() == '}')
            {
                actions.push_back(now.substr(1, now.length() - 1));
            }
            else if (now[0] == '{')
            {
                inaction = 1;
                tmpaction = now.substr(1);
            }
            else if (now.back() == '}')
            {
                inaction = 0;
                tmpaction += now.substr(0, now.length() - 1);
                actions.push_back(tmpaction);
            }
            else if (!inaction)
            {
                b += now;
            }
            else
            {
                tmpaction += now;
            }
        }
        if (i == 0)
            S = a[0];
        if (a == "epsilon")
            a = "\0";
        if (b == "epsilon")
            b = "\0";
        semanticsAction action = semanticsAction(a, b, actions);
        P2.push_back(action);
        P.push_back(action);
    }
    string s;
    fin >> s;
    if (s.length() > 0)
        S = s;
    
    // Grammar_LR cfg_LR(VT, VN, S, P);
    // Grammar_LR1 cfg_LR1(VT, VN, S, P);
    if (mode == "cacuset")
    {
        ContextFreeGrammar cfg(VT, VN, S, P);
        cacuSet(cfg);
    }
    else if (mode == "derivation")
    {
        ContextFreeGrammar cfg(VT, VN, S, P);
        derivation2(cfg, s_value);
    }
    else if(isseman == true)
    {
        semanticsAnalysis_LR cfg_seman(VT, VN, S, P2);
        LR_semantics_Analysis(cfg_seman,s_value,mode);
    }
    else
    {
        int pos = mode.find('_');
        string action = mode.substr(0, pos);
        string amode = mode.substr(pos + 1);
        if (action == "table")
        {
            if (amode != "LR1")
            {
                Grammar_LR cfg(VT, VN, S, P);
                getLR_table(cfg, amode);
            }
            else
            {
                Grammar_LR1 cfg(VT, VN, S, P);
                getLR_table(cfg, amode);
            }
        }
        else if (action == "deduce")
        {
            Grammar_LR1 cfg(VT, VN, S, P);
            LR_Analysis(cfg, s_value, amode);
        }
    }
}
#elif _WIN32

#endif