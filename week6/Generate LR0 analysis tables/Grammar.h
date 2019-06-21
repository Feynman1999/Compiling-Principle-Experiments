#pragma once
#include<set>
#include<vector>
#include<map>
#include<queue>
#include<assert.h>
#include<iostream>
using namespace std;

const int maxn = 128;

string int2str(int x);

class LR_0
{
private: 
	char S;//开始符号    $ means S'
	int num;//产生式的数量
	set<char> VN;//非终结符
	set<char> VT;//终结符
	vector<vector<string>> P; //产生式
	vector<pair<char, string>> PP;//编号后的产生式
	map<string, int> P2id;
	int tot;//一共的状态数 从0开始计数
	vector<pair<char, int>> G[maxn];//状态图
	set<pair<int, int>> node[maxn];//每个状态是一个产生式的集合  元素为pair<PP中的id,小圆点的位置>
	int check_exist(const set<pair<int, int>> & node, const int & tot);
	void closure(int now);
	bool check_acc(int now);

public:
	LR_0();
	void add_grammar(char left, string s);
	void set_S(char c);
	void display_G();
	void get_Analysis_table();
	void display_FA();
};
