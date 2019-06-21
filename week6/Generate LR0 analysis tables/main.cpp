/***
*	main.cpp
*	Created by feynman1999 on 2019/5/14 14:55:42.
*   in machine: FEYNMAN
***/
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
	ios::sync_with_stdio(false);
	LR_0 G1;
	int T;
	cin >> T;
	while (T--)
	{
		string s;
		cin >> s;
		G1.add_grammar(s[0], s.substr(3));
	}
	G1.set_S('E');
	G1.display_G();
	G1.get_Analysis_table();
	//G1.display_FA();
	return 0;
}

