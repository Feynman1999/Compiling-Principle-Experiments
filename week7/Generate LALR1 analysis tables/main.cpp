// LALR(1) Grammar
// Feynman1999 on Earth
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <queue>
#include <map>
#include <bitset>
#include <cstring>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "Grammar.h"
using namespace std;


int main()
{
	freopen("in.txt", "r", stdin);
	freopen("out.txt", "w", stdout);
	ios::sync_with_stdio(false);
	LALR_1 g1;
	int T;
	cin >> T;
	while (T--)
	{
		string s;
		cin >> s;
		g1.add_grammar(s[0], s.substr(3));
	}
	g1.set_S('S');
	g1.init();
	g1.display_G();
	g1.get_Analysis_table();
	g1.display_FA_LR();
	g1.display_FA_LALR();
	return 0;
}
