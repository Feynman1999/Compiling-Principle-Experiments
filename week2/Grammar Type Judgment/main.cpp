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
#include <fstream>
#include <ctype.h>
#include <sstream>
using namespace std;
typedef pair<string, string> pss;

inline bool is_capital(char c)
{
	if (c >= 'A' && c <= 'Z') return true;
	return false;
}

inline bool is_lowercase(char c)
{
	if (c >= 'a' && c <= 'z') return true;
	return false;
}

class Grammar {
private:
	map<string, vector<string> > mp;
	set<char> non_terminal;
	set<char> terminal;
	short type;

public:
	//Grammar() {}
	Grammar(vector<pss> _pcs){
		mp.clear();
		non_terminal.clear();
		terminal.clear();
		type = 0;
		for (auto k : _pcs) this->add(k.first, k.second);
		//遍历键，加入non_terminal;
		for (auto k : mp) {
			for (auto c : k.first) {
				if(is_capital(c)) non_terminal.insert(c);
			}
		}		
		//遍历值(vector)，凡是目前不在non_terminal里的为terminal
		for (auto k : mp) {
			for (auto str : k.second) {
				for (int i = 0; i < str.length(); ++i) {
					if (non_terminal.count(str[i]) == false) {
						terminal.insert(str[i]);
					}
				}
			}
		}
		//type
		this->check_type();
	}
	void add(string S, string D)
	{
		this->mp[S].push_back(D);
	}
	void print_non_terminal() {
		cout << "非终结字符有：\n";
		for (auto k : non_terminal) {
			cout << k << " ";
		}
		cout << "\n\n";
	}
	void print_terminal() {
		cout << "终结字符有：\n";
		for (auto k : terminal) {
			cout << k << " ";
		}
		cout << "\n\n";
	}
	void check_type() {
		vector<short> tmp;
		//这里注意 若为3型 且Rlen=2 则要么非终结符都在左边  要么都在右边
		for (auto k : this->mp) {
			for (auto str : k.second) {
				cout << k.first << " ";
				tmp.push_back(get_one_type(k.first, str));
			}
		}
		cout << endl;
		short minn = 3;
		for (auto k : tmp) {
			minn = min(minn, k);
			cout << k << " ";
		}
		cout << endl;
		this->type = minn;
	}
	bool is_all_terminal(string s) {
		for (auto c : s) if (non_terminal.count(c)) return false;
		return true;//s=""
	}
	bool is_right_linear(string s) {
		assert(s.length());
		if (non_terminal.count(s[s.length() - 1]) && s.length()>=2 && is_all_terminal(s.substr(0,s.length()-1))) return true;
		return false;
	}
	bool is_left_linear(string s) {
		reverse(s.begin(), s.end());
		return is_right_linear(s);
	}
	short get_one_type(string L, string R) {
		// 3 2 1 0
		if (L.length() == 1) {
			assert(is_capital(L[0]));
			if (is_all_terminal(R) || is_right_linear(R) || is_left_linear(R)) return 3;
			return 2;
		}
		if (L.length() <= R.length()) return 1;
		return 0;
	}
	short get_type() {
		return this->type;
	}
};


vector<pss> read(string path)
{
	vector<pss> _pcs;
	ifstream read(path);
	string line;
	while (getline(read, line))
	{
		if (line == "") break;
		stringstream grammar_stream(line);
		string arrow, src, des;
		grammar_stream >> src;
		grammar_stream >> arrow;
		while (grammar_stream >> des) {
			if (des[0] == '|') continue;
			else _pcs.push_back(make_pair(src, des));
		}
	}
	return _pcs;
}


int main()
{
	vector<pss> vec_pss=read("in.txt");
	Grammar g1 = Grammar(vec_pss);
	g1.print_non_terminal();
	g1.print_terminal();
	cout <<"文法类型 :"<<g1.get_type() << endl;
}