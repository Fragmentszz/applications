#include<bits/stdc++.h>

using namespace std;


int parse(int argc,char* argv[],string& filepath,string& mode,string& s_value)
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("-filepath=") == 0) {
            filepath = arg.substr(10);
        } else if (arg.find("-mode=") == 0) {
            mode = arg.substr(6);
        } else if (arg.find("-s=") == 0) {
            s_value = arg.substr(3);
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return EXIT_FAILURE;
        }
    }
    return 0;
}




int main(int argc,char* argv[])
{
    string filepath,mode,s_value;
    int parseResult = parse(argc,argv,filepath,mode,s_value);
    if(parseResult)
    {
        return parseResult;
    }
    cout << filepath.c_str() << endl;
    ifstream fin(filepath.c_str(), ios::in);
	
	int n, t, p;	fin >> n >> t >> p;
	vector<char> VT, VN;
	vector<pair<string ,string>> P;
	char S;
	for (int i = 0; i < n; i++)
	{
		string s; fin >> s;
		VN.push_back(s[0]);
	}
	for (int i = 0; i < t; i++)
	{
		string s;	fin >> s;
		if (s == "epsilon")	VT.push_back('\0');
		else VT.push_back(s[0]);
	}
	for (int i = 0; i < p; i++)
	{
		string a, b;	fin >> a >> b;
		if (i == 0)	S = a[0];
		if (a == "epsilon")	a = "\0";
		if (b == "epsilon")	b = "\0";
		P.push_back({ a,b });
        cout << a << "->" << b << endl;
	}
	string s;	fin >> s;
	if(s.length() > 0)
		S = s[0];

	cout << s_value << endl;

    return 0;
}