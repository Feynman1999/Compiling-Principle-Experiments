#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <bitset>
#include <cstring>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
using namespace std;
const int maxn = 1000;

struct RG
{
	vector<char> star;  //A->(a|b)A
	vector<char> endchar;  //A->a|b
	vector<string> Next;    //A->aB|cC
	string get_star()
	{
		string ans = "";
		if (!star.empty())
		{
			int n = star.size();
			ans.push_back(star[0]);
			for (int i = 1; i < n; ++i) ans.push_back('|'), ans.push_back(star[i]);
			if (n != 1) ans = "(" + ans + ")";
			ans += "*";
		}
		return ans;
	}
	string get_endchar()
	{
		string ans = "";
		if (!endchar.empty())
		{
			int n = endchar.size();
			ans.push_back(endchar[0]);
			for (int i = 1; i < n; ++i) ans.push_back('|'), ans.push_back(endchar[i]);
		}
		return ans;
	}
}G[maxn];

string ans[maxn];

void add_grammar(string s)
{
	int n = s.length();
	if (n == 4)
	{
		if (s[3] != 'e') G[s[0]].endchar.push_back(s[3]);  //A->a
	}
	else {
		if (s[4] == s[0]) G[s[0]].star.push_back(s[3]);    //A->aA
		else G[s[0]].Next.push_back(s.substr(3, 2));     //A->aB
	}
}


string dfs(char c)
{
	if (ans[c] != "#") return ans[c];
	ans[c] = G[c].get_star();
	string tmp = G[c].get_endchar();
	for (auto k : G[c].Next)
	{
		if (tmp.length() != 0) tmp.push_back('|');
		tmp.push_back(k[0]);
		tmp += dfs(k[1]);
	}
	if (G[c].endchar.size() + G[c].Next.size() > 1) tmp = "(" + tmp + ")";
	ans[c] += tmp;
	return ans[c];
}


int main()
{
	freopen("in.txt", "r", stdin);
	int n;
	cin >> n;
	while (n--)
	{
		string s;
		cin >> s;
		add_grammar(s);
	}
	for (int i = 0; i < maxn; ++i) ans[i] = "#";
	cout << dfs('S') << endl;
	return 0;
}
