/*
	文法：
	S -> B | S+B
	B -> C | B*C
	C -> (S) | i
*/

#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <assert.h>
#include <fstream>
using namespace std;
typedef pair<char, string> pcs;
typedef pair<string, string> pss;


class Expression {
private:
	string exp;
	int solution_num = 0;
	static const int maxn = 10;
	vector<pcs> solution1[maxn];
	vector<pss> solution2[maxn];
public:
	Expression() {}
	Expression(string s) :exp(s) {}
	void set_exp(string s) {
		this->exp = s;
	}
	void print_exp() {
		cout << "此表达式的内容为 " << this->exp << endl;
	}
	string get_exp() {
		return this->exp;
	}
	int get_solution_num() {
		return this->solution_num;
	}
	void add_solution(pcs tp1, pss tp2) {
		if (this->solution_num >= this->maxn) {
			cout << "wrong!! 开辟的空间不够了" << endl;
			return;
		}
		this->solution1[solution_num].push_back(tp1);
		this->solution2[solution_num].push_back(tp2);
	}
	void add_solution_num() {
		this->solution_num++;
	}
	int get_maxn() {
		return this->maxn;
	}
	void reverse_last_solution() {
		reverse(solution1[solution_num].begin(), solution1[solution_num].end());
		reverse(solution2[solution_num].begin(), solution2[solution_num].end());
	}
	void print_solutions() {
		if (this->solution_num == 0) {
			cout << "表达式 " << this->exp << "没有方案！";
			cout << "\n\n";
			return;
		}
		cout << "表达式 " << this->exp << "共有" << this->solution_num << "条方案：\n";
		for (int i = 0; i < this->solution_num; ++i) {
			cout << "方案" << i + 1 << "：\n";
			int j = 0;
			for (auto k : this->solution1[i]) {
				cout << k.first << " -> " << k.second;
				cout << "      ";
				cout << this->solution2[i][j].first << " -> " << this->solution2[i][j].second << endl;
				++j;
			}
			cout << "\n\n";
		}
	}
	void print_shortest_solutions() {
		if (this->solution_num == 0) {
			cout << "表达式 " << this->exp << "没有方案！";
			cout << "\n\n";
			return;
		}
		cout << "表达式 " << this->exp << "最短推导路径为：\n";
		int minn_id = 0;
		for (int i = 1; i < this->solution_num; ++i) {
			if (this->solution1[i].size() < this->solution1[minn_id].size()) minn_id = i;
		}
		int j = 0;
		for (auto k : this->solution1[minn_id]) {
			cout << k.first << " -> " << k.second;
			cout << "      ";
			cout << this->solution2[minn_id][j].first << " -> " << this->solution2[minn_id][j].second << endl;
			++j;
		}
		cout << "\n\n";
	}
};

map<char, vector<string > > mp;
set<char> non_terminal;
set<char> terminal;
stack<pcs> sta1;
stack<pss> sta2;
Expression desti;

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


void add(char S, string D)
{
	mp[S].push_back(D);
}


void init()
{
	add('S', "B");
	add('S', "S+B");
	add('B', "C");
	add('B', "B*C");
	add('C', "(S)");
	add('C', "i");
	//遍历键，加入non_terminal;
	for (auto k : mp) {
		non_terminal.insert(k.first);
	}
	//遍历值(vector)，凡是目前不在non_terminal里的为terminal
	for (auto k : mp) {
		for (auto str : k.second) {
			for (int i = 0; i < str.length();++i) {
				if (non_terminal.count(str[i]) == false) {
					terminal.insert(str[i]);
				}
			}
		}
	}

	print_non_terminal();
	print_terminal();
}


void dfs(string now) {
	if (desti.get_solution_num() >= desti.get_maxn()) return;
	if (now.length() > desti.get_exp().length()) return;
	if (now == desti.get_exp()) {
		stack<pcs> tmp1 = sta1;
		stack<pss> tmp2 = sta2;
		while (!tmp1.empty())
		{
			desti.add_solution(tmp1.top(), tmp2.top());
			tmp1.pop();
			tmp2.pop();
		}
		desti.reverse_last_solution();
		desti.add_solution_num();
		return;
	}
	for (int i = 0; i < now.length() ; ++i) {
		if (non_terminal.count(now[i])) {//非终结符号
			for (auto k : mp[now[i]]) {
				string tp = now;
				tp.replace(i, 1, k);// 这里是直接在原串中替换了
				sta1.push(make_pair(now[i], k));
				sta2.push(make_pair(now, tp));
				dfs(tp);
				sta1.pop();
				sta2.pop();
			}
		}
	}
}

vector<Expression> exps;

int main()
{
	ifstream read("in.txt");
	init();
	string tp;
	while (getline(read, tp))
	{
		if (tp == "") continue;
		desti = Expression(tp);
		dfs("S");
		exps.push_back(desti);
		//desti.print_solutions();
		desti.print_shortest_solutions();
	}
}