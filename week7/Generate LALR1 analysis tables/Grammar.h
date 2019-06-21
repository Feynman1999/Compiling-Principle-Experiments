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
	char S;//��ʼ����    $ means S'
	int num;//����ʽ������
	set<char> VN;//���ս��
	set<char> VT;//�ս��
	vector<vector<string>> P; //����ʽ
	vector<pair<char, string>> PP;//��ź�Ĳ���ʽ
	map<string, int> P2id;//�ɲ���ʽ��ñ��
	int tot1;//LR1һ����״̬�� ��0��ʼ����
	int tot2;//LALR1һ����״̬�� ��0��ʼ����
	vector<pair<char, int>> G1[maxn];//LR_1״̬ͼ
	vector<pair<char, int>> G2[maxn];//LALR_1״̬ͼ
	set<int> G2_G1[maxn];//G2�е�ÿ�������֮ǰ����Щ��
	int G1_G2[maxn]; // G1�е�ÿ����������G2�е��ĸ��㣨������  init -1
	set<iic> node1[maxn];//LR1 ÿ��״̬��һ������ʽ�ļ���  Ԫ��Ϊtriple<PP�е�id,СԲ���λ��,�ַ�����>
	set<iic> node2[maxn];//LALR1

	vector<bool> Empty;
	vector<bitset<maxn>> First;  //First
	void cal_Empty();//�� ���ս���ܷ��Ƴ���
	void cal_First();//�� ���з��ŵ�first��
	bitset<maxn> first(string s);//��һ���ַ�����first��
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
