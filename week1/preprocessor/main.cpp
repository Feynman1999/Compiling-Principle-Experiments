/***
*	main.cpp
*	Created by feynman1999 on 2019/3/11 12:10:00.
*   in machine: FEYNMAN
	C语言 预处理
	没有考虑字符串
***/
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
#include <assert.h>
#define debug cout<<"debug "<<++debug_num<<" :"
#define pb push_back
#define mp make_pair
#define lson l,m,rt<<1
#define rson m+1,r,rt<<1|1
#define bit(a,b) ((a>>b)&1) //from 0
#define all(x) (x).begin(),(x).end()
using namespace std;
typedef long long ll;
typedef pair<int, int> PII;
int debug_num = 0;

vector<string> lines;
string ans;// 预处理后的结果


// 判断一个字符串是否均为空格或者为空
bool is_none_or_blank(string tmp)
{
	if (tmp == "") return true;
	for (int i = 0; i < tmp.length(); ++i) {
		if (tmp[i] != ' ') return false;
	}
	return true; // all is blank
}


// 从文件读
void read(string file_name)
{
	freopen(file_name.c_str(), "r", stdin);
	string tmp;
	while (getline(cin, tmp))
	{
		if (is_none_or_blank(tmp)) continue;// 如果为空行则跳过
		lines.push_back(tmp);
	}
}


//从行下标为k 行内下标为i处开始搜索*/
vector<int> find_multiline_annotation_over(int k, int i) {
	vector<int> id; id.clear();
	//先把当前行搜完
	for (; i < lines[k].length() - 1; ++i) {
		if (lines[k][i] == '*' && lines[k][i + 1] == '/') {
			id.push_back(k);
			id.push_back(i + 1);
			return id;
		}
	}
	//搜下一行
	k += 1;
	for (; k<int(lines.size()); ++k) {
		string line = lines[k];
		for (i = 0; i < line.length() - 1; ++i) {
			if (line[i] == '*' && line[i + 1] == '/') {
				id.push_back(k);
				id.push_back(i + 1);//+1细节
				return id;
			}
		}
	}
	return id;//为空也返回
}


// 行内和行间注释
// 如果遇到行间注释则找到下一个为止(不考虑行内注释) 如果到最后都找不到则全部丢弃
// 如果遇到行内注释则直接放弃本行
string deal_annotation() {
	string ans = "";
	for (int k = 0; k<int(lines.size()); ++k) {
		string line = lines[k];
		for (int i = 0; i < line.length(); ++i) {
			if (line[i] == '/' && i + 1 < line.length() && line[i + 1] == '/') { break; }
			else if (line[i] == '/' && i + 1 < line.length() && line[i + 1] == '*') {
				// 搜索到下一个*/为止
				vector<int> id = find_multiline_annotation_over(k, i + 2);
				if (id.size() == 0) return ans;//剩下的不要了
				assert(id.size() == 2);
				k = id[0];
				line = lines[k];
				i = id[1];
			}
			else { ans.push_back(line[i]); }
		}
	}
	return ans;
}


// tab->space & 将多个空格缩为单个空格  
string deal_multispace() {
	string bns;
	for (int i = 0; i < ans.length(); ++i) { //将tab替换为空格
		if (ans[i] == '\t') ans[i] = ' ';
	}
	for (int i = 0; i < ans.length(); ++i) {
		if (ans[i] == ' ' && i < ans.length() - 1 && ans[i + 1] == ' ')continue;
		else bns += ans[i];
	}
	return bns;
}


// 输出
void out() {
	freopen("out.txt", "w", stdout);
	cout << "源代码: " << endl;
	for (auto k : lines) { cout << k << endl; }
	cout << "\n\n\n\n\n预处理后: \n\n";
	cout << ans << endl;
}


int main()
{
	string file_name = "in.txt";
	read(file_name);
	ans = deal_annotation();
	ans = deal_multispace();
	out();
	return 0;
}

