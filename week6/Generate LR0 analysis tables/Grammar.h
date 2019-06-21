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
	char S;//��ʼ����    $ means S'
	int num;//����ʽ������
	set<char> VN;//���ս��
	set<char> VT;//�ս��
	vector<vector<string>> P; //����ʽ
	vector<pair<char, string>> PP;//��ź�Ĳ���ʽ
	map<string, int> P2id;
	int tot;//һ����״̬�� ��0��ʼ����
	vector<pair<char, int>> G[maxn];//״̬ͼ
	set<pair<int, int>> node[maxn];//ÿ��״̬��һ������ʽ�ļ���  Ԫ��Ϊpair<PP�е�id,СԲ���λ��>
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
