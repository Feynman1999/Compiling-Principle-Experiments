#pragma once
#include<set>
#include<vector>
#include<map>
#include<queue>
#include<bitset>
#include<string>
#include<assert.h>
#include<iostream>
using namespace std;
typedef pair<pair<int, int>, set<char>> iic;

const int maxn = 128;

string int2str(int x);

string set2str(const set<char> & st);

set<char> bs2set(const bitset<maxn> & bs);

set<char> union_set(set<char> s1, set<char> s2);

class LALR_1
{
private:
	char S;//开始符号    $ means S'
	int num;//产生式的数量
	set<char> VN;//非终结符
	set<char> VT;//终结符
	vector<vector<string>> P; //产生式
	vector<pair<char, string>> PP;//编号后的产生式
	map<string, int> P2id;//由产生式获得编号
	int tot1;//LR1一共的状态数 从0开始计数
	int tot2;//LALR1一共的状态数 从0开始计数
	vector<pair<char, int>> G1[maxn];//LR_1状态图
	vector<pair<char, int>> G2[maxn];//LALR_1状态图
	set<int> G2_G1[maxn];//G2中的每个点包含之前的哪些点
	int G1_G2[maxn]; // G1中的每个点现在在G2中的哪个点（集）中  init -1
	set<iic> node1[maxn];//LR1 每个状态是一个产生式的集合  元素为triple<PP中的id,小圆点的位置,字符集合>
	set<iic> node2[maxn];//LALR1

	vector<bool> Empty;
	vector<bitset<maxn>> First;  //First
	void cal_Empty();//求 非终结符能否推出空
	void cal_First();//求 所有符号的first集
	bitset<maxn> first(string s);//求一个字符串的first集
	int check_exist(const set<iic> & node1, const int & tot);
	void closure(int now);
	void display_FA(const vector<pair<char, int>> G[maxn], int tot);
	void node_merge(int id);
	bool check_sameheart(int id1, int id2);
	void merge_sameheart(set<iic> & s1, const set<iic> & s2);

public:
	LALR_1();
	void init();
	void add_grammar(char left, string s);
	void set_S(char c);
	void display_G();
	void get_Analysis_table();
	void display_FA_LR();
	void display_FA_LALR();
};
