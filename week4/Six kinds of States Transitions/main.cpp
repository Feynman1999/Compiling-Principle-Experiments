/***
*	main.cpp
*	Created by feynman1999 on 2019/4/8 19:43:11.
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
#include <unordered_map>
#include <unordered_set>
#include <bitset>
#include <cstring>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iterator>
#define debug cout<<"debug "<<++debug_num<<" :"
#define pb push_back
#define lson l,m,rt<<1
#define rson m+1,r,rt<<1|1
#define bit(a,b) ((a>>b)&1) //from 0
#define all(x) (x).begin(),(x).end()
#define umap unordered_map
using namespace std;
typedef long long ll;
typedef pair<char, string> pss;
typedef pair<int, int> PII;
int debug_num = 0;
const int maxn = 30;

char grammer_start;// 直接全局了

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

string conca_comma(set<string> s) {
	string ans = "";
	int num = 0;
	for (auto k : s) {
		if (num > 0) ans += ',';
		ans += k;
		num++;
	}
	//assert(ans != "");
	return ans;
}

string conca_or(set<string> s) {
	string ans = "";
	int num = 0;
	for (auto k : s) {
		if (k == "$") continue;
		if (num > 0) ans += '|';
		ans += k;
		num++;
	}
	return ans;
}

set<string> split_comma(string s)
{
	assert(s != ",");
	set<string> ans;
	string tp = "";
	for (int i = 0; i < s.length(); ++i) {
		if (s[i] == ',') {
			//ans.push_back(tp);
			assert(!ans.count(tp));
			ans.insert(tp);
			tp = "";
		}
		else tp += s[i];
	}
	if (tp.length() > 0) {
		assert(!ans.count(tp));
		ans.insert(tp);
	}
	return ans;
}

bool check_or(string s) {
	for (auto k : s) {
		if (k == '|') return true;
	}
	return false;
}

//class Grammar;

class FA {
private:
	bool is_derministic;//DFA or NFA
	bool is_simplified;//是否最小化
	string start;//开始状态
	set<string> terminal;//终态集 一个字符串表示终态的名字
	set<string> K;//状态集 一个字符串为一个状态的名字
	vector<char> Sigma_vector;// 字符集
	umap<string, umap<char, set<string> > > matrix;
public:
	FA(const umap<string, umap<char, set<string> > > & _matrix, const string & _start,
		const vector<string> & _terminal, const vector<char> & _Sigma_vector, bool simp = 0) {
		matrix = _matrix;
		start = _start;
		Sigma_vector = _Sigma_vector;
		K.clear(); terminal.clear();// init K, Sigma and terminal
		for (auto kk1 : _matrix) K.insert(kk1.first);
		for (auto kk : _terminal) terminal.insert(kk);
		this->is_derministic = false;
		this->is_simplified = false;
		//if (simp) this->Simplify();
	}
	void print_set(const set<string> & s) {
		cout << "{";
		for (auto k : s) {
			cout << k << " ";
		}
		cout << "}\n";
	}
	void print_setset(const set<set<string> > & ss) {
		cout << "{";
		for (auto k : ss) {
			cout << "     ";
			print_set(k);
		}
		cout << "}\n";
	}
	void print_mapp(set<string>(*mapp)[maxn], int n) {
		cout << "\n\n";
		for (int i = 0; i < 50; ++i) cout << "*";
		cout << "\n\n";
		for (int i = -1; i < n; ++i) {
			if(i==-1) printf("%-10s", "(#^.^#)");
			else printf("%-10d", i);
		}
		cout << endl;
		for (int i = 0; i < n; ++i) {
			printf("%-10d", i);
			for (int j = 0; j < n; ++j) {
				printf("%-10s", (conca_comma(mapp[i][j]) == "" ? "#" : conca_comma(mapp[i][j])).c_str());
			}
			cout << endl;
		}
		cout << "\n\n";
		for (int i = 0; i < 50; ++i) cout << "*";
		cout << "\n\n";
	}
	set<string> insert_set(const set<string> & s1, const set<string> & s2) {
		set<string> ans;
		set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(ans, ans.begin()));
		return ans;
	}
	set<string> union_set(const set<string> & s1, const set<string> & s2) {
		set<string> ans;
		set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(ans, ans.begin()));
		return ans;
	}
	set<string> closure(const set<string> & I) {
		// 表示状态集I中的每个状态经过任意条空线所能到达的状态的集合
		// solution: 不停用空去拓展当前集合  直到大小不再变化为止
		set<string> ans = I;
		while (1) {
			//对I所有状态进行一次拓展
			set<string> once_move = Move(union_set(I, ans), '$');
			int before_size = ans.size();
			ans = union_set(ans, once_move);
			if (ans.size() == before_size) break;//没有新的元素加入
		}
		return ans;
	}
	set<string> Move(const set<string> & I, char a) {
		// move操作 定义为状态集合J
		// 其中J是I中所有状态经过一条a弧而到达的状态的全体
		set<string> J;
		for (auto k : I) {
			/*
			if(!this->matrix.count(k)) print_set(I);
			*/
			assert(this->matrix.count(k));
			assert(this->matrix[k].count(a));
			if (conca_comma(this->matrix[k][a]) == "#" || this->matrix[k][a].size()==0) continue;//不可能: 既有# 又有其他
			J = union_set(J, this->matrix[k][a]);
		}
		return J;
	}
	void NFA2DFA() {
		/*
			使用子集法对有限自动机进行确定化
			两个运算：
			1.$闭包  注意在本实验中  $表示空
			2.集合I的a弧转换 表示为move(I,a)
		*/
		cout << "正在进行确定化\n";
		if (this->is_derministic) {
			cout << "已经是DFA!" << endl;
			return;
		}
		cout << "确定化之前matrix为：" << endl;
		this->print_matrix();
		//下面进行确定化
		/*
			NFA k,sigma,f,k0,kt
			构造一个DFA 他的元素是k的子集
			映射规则是集合 到 集合
		*/
		int m = 0;//状态数量 最终写作 A~Z
		set<string> _terminal;
		umap<string, umap<char, set<string> > > _matrix;
		map<set<string>, string> help;
		this->K.clear();

		set<set<string> > C;
		set<set<string> > unmark;
		set<set<string> > marked;
		set<string> C0;
		C0.insert(this->start);
		C0 = closure(C0);
		//debug<<endl;
		C.insert(C0);
		unmark.insert(C0);
		assert(m < 26);
		help[C0] = string(1, 'A' + m); m++;
		cout << "确定化过程表，其中 * 表示已经出现" << endl;
		// 确定起始态
		assert(C0.count(this->start));
		this->start = help[C0];
		while (!unmark.empty())
		{
			set<string> T = *(unmark.begin());
			printf("%-16s", conca_comma(T).c_str());
			unmark.erase(unmark.begin());
			marked.insert(T);
			// 检查T是否是终止态
			if (!insert_set(this->terminal, T).empty()) _terminal.insert(help[T]);
			for (auto ch : Sigma_vector) {
				if (ch == '$') continue;
				//上一次走过了空 这一次再move再closure即可
				set<string> U = closure(Move(T, ch));
				printf("%-16s", (conca_comma(U) + (C.count(U) ? "*" : "")).c_str());
				if (!C.count(U)) {
					unmark.insert(U);
					C.insert(U);
					assert(m < 26);
					help[U] = string(1, 'A' + m); m++;
				}
				//create new matrix
				_matrix[ help[T] ][ ch ].insert(help[U]);
			}
			printf("%-16s\n", help[T].c_str());
		}
		this->matrix = _matrix;
		for (auto kk1 : this->matrix) K.insert(kk1.first);
		// 去除空字
		if (find(this->Sigma_vector.begin(), this->Sigma_vector.end(), '$') != this->Sigma_vector.end()) {
			this->Sigma_vector.erase(find(this->Sigma_vector.begin(), this->Sigma_vector.end(), '$'));
		}
		is_derministic = true;  
		this->terminal = _terminal;
	}
	void Simplify() {
		cout << "正在进行最小化" << endl;
		this->NFA2DFA();//先进行确定化
		if (this->is_simplified) {
			cout << "已经最小化!" << endl;
			return;
		}
		is_simplified = true;
	}
	void print_matrix() {
		int n = Sigma_vector.size();
		int m = K.size();
		printf("\n\nprint_matrix\n");
		printf("******************************************************\n");
		printf("%-16s", "状态\\符号");
		for (auto kk : Sigma_vector) printf("%-16c", kk); cout << '\n';
		for (int i = 0; i < 50; ++i) cout << "-"; cout << '\n';
		for (auto kk1 : K) {
			printf("%-16s", (kk1 + (kk1 == start ? " *" : "")).c_str());
			for (auto kk2 : Sigma_vector) printf("%-16s", conca_comma(matrix[kk1][kk2]).c_str());
			printf("%-10d\n", terminal.count(kk1));
		}
		printf("******************************************************\n\n\n");
	}
	int get_edge_num(set<string> (*mapp)[maxn], int n) {
		int num = 0;
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				num += mapp[i][j].size();
			}
		}
		return num;
	}
	void deal_multiedges(set<string> (*mapp)[maxn], int n) {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				if (mapp[i][j].size() > 1) {
					string tp = conca_or(mapp[i][j]);
					mapp[i][j].clear();
					mapp[i][j].insert(tp);
				}
			}
		}
	}
	void deal_one_node(set<string> (*mapp)[maxn], int n, int mid) {
		vector<short> in_id;
		vector<short> out_id;
		for (int i = 0; i < n; ++i) {
			if (i == mid) continue;
			if (mapp[i][mid].size()) {
				assert(mapp[i][mid].size() == 1);//由于已经去除重边 则一定为1
				in_id.push_back(i);
			}
			if (mapp[mid][i].size()) {
				assert(mapp[mid][i].size() == 1);//由于已经去除重边 则一定为1
				out_id.push_back(i);
			}
		}
		for (auto k1:in_id) {
			for (auto k2:out_id) {
				//去除k1到id id到k2的边 以及id到id自己的边
				string l, r, m;
				m = "";
				l = conca_comma(mapp[k1][mid]); if (l == "$") l = "";
				r = conca_comma(mapp[mid][k2]); if (r == "$") r = "";
				if (mapp[mid][mid].size()) {
					assert(mapp[mid][mid].size() == 1);//由于已经去除重边 则一定为1
					m = conca_comma(mapp[mid][mid]);
					if (m == "$") m="";
					if (m.length() == 1) m = m + "*";
					else m="(" + m + ")" + "*" ;
				}
				mapp[k1][k2].insert(l+m+r);
				// deal multiedge   can reduce time        (: for ease
				deal_multiedges(mapp, n);
			}
		}
		//delete edge
		if (in_id.size() && out_id.size()) {
			for (auto k1 : in_id) mapp[k1][mid].clear();
			for (auto k2 : out_id) mapp[mid][k2].clear();
			if (mapp[mid][mid].size()) mapp[mid][mid].clear();
		}
	}
	string FA2RE() {
		//重新构造一个邻接矩阵
		set<string> mapp[maxn][maxn];// from 0   若未定义则set为空
		for (int i = 0; i < maxn; ++i) for (int j = 0; j < maxn; ++j) mapp[i][j].clear();
		//对于所有的状态进行hash
		umap<string, short> hash;
		vector<string> hash_back;
		hash_back.resize(maxn);
		short id = 0;
		//0 1分别是新加的开始状态(start)和终止状态(end)
		hash["Start"] = id;
		hash_back[id++] = "Start";
		hash["End"] = id;
		hash_back[id++] = "End";
		for (auto k : this->K) {
			hash[k] = id; 
			hash_back[id++] = k;
		}
		//Start连到之前的开始状态
		assert(hash.count(this->start));
		//cout << this->start << endl;
		mapp[ hash["Start"] ][ hash[this->start] ].insert("$");
		//之前的终止状态连到End
		for (auto k : this->terminal) {
			assert(hash.count(k));
			mapp[ hash[k] ][ hash["End"] ].insert("$");
			//cout << hash[k] << "->" << hash["End"] << endl;
		}
		//用matrix 更新 mapp
		for (auto k1 : this->K) {
			for (auto ch : this->Sigma_vector) {
				//assert(this->matrix[k][ch].size() == 1);  //确定化之后一定成立
				for (auto k2 : this->matrix[k1][ch]) {//如果没有则自动跳过
					//cout << k1 << "," << ch << "->" << k2 << endl;
					if (k2 == "#") continue;   //确定化之后不存在这种情况
					short help_id = hash[k2];
					mapp[ hash[k1] ][ help_id ].insert(string(1, ch));
				}
			}
		}

		//print_mapp(mapp, id);
		
		//solve
		short last_edge_num = 32700;
		short now_edge_num = get_edge_num(mapp, id);
		while (now_edge_num > 1) {
			//处理一次重边
			deal_multiedges(mapp, id);
			//枚举每个点作为中间点
			for (short i = 0; i < id; ++i) deal_one_node(mapp, id, i);
			//更新
			last_edge_num = now_edge_num;
			now_edge_num = get_edge_num(mapp, id);
			if (last_edge_num == now_edge_num) {
				cout << "something wrong!!! now_edge_num=last_edge_num=" << now_edge_num << endl;
				print_mapp(mapp, id);
				exit(233);
			}
		}//over
		//取start -> end 上的值即可
		assert(mapp[0][1].size()==1);
		return conca_comma(mapp[0][1]);
	}
	//friend class Grammer;
	vector<pss> FA2RG() {
		vector<pss> _pcs;
		//for (auto sigma : this->Sigma_vector) cout << sigma << endl;
		for (auto status : this->K) {
			//cout << status << endl;
			assert(status.size() == 1);//字符
			// assert(this->start == "S");// 确定化之后 开始符号不一定是S了
			for (auto sigma : this->Sigma_vector) {//确定化之后 sigma中无空
				assert(this->matrix[status][sigma].size()==1);//由于确定化了 则一定有定义(不为"#") 而且只有一个
				string tp = conca_comma(matrix[status][sigma]);// 注意由于确定化之后重新编号了
				assert(tp.size() == 1 && is_capital(tp[0]));//由于是RG转来 则一定长度为1且为大写字符
				_pcs.push_back(make_pair(status[0], string(1, sigma)+tp));
			}
		}
		for (auto status : this->terminal) _pcs.push_back(make_pair(status[0], "$"));
		assert(this->start.size() == 1);
		grammer_start = this->start[0];
		return _pcs;
	}
};

class Grammar {
private:
	map<char, vector<string> > mp;
	set<char> non_terminal;
	set<char> terminal;
	char start;
public:
	//Grammar() {}
	Grammar(vector<pss> _pcs, char _start) {
		start = _start;
		mp.clear();
		non_terminal.clear();
		terminal.clear();
		for (auto k : _pcs) this->add(k.first, k.second);
		//遍历键，加入non_terminal;
		for (auto k : mp) {
			if (is_capital(k.first)) non_terminal.insert(k.first);
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
	}
	void add(char S, string D)
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
	void print_G() {
		cout << "\n\n-------------------------文法如下-----------------------\n";
		for (auto k : mp) {
			cout << ( (k.first==this->start) ? (string(1,k.first)+"*") : string(1,k.first)) << " -> ";
			assert(k.second.size());
			for (int i = 0; i < k.second.size(); ++i) {
				if (i == 0) cout << k.second[i];
				else cout << " | " << k.second[i];
			}
			cout << endl;
		}
		cout << "\n\n";
	}
	FA RG2FA() {
		umap<string, umap<char, set<string> > > _matrix;
		string _start;
		vector<string> _terminal;
		vector<char> _Sigma_vector;
		// algorithm
		bool terminal_flag = false;
		for (auto k : this->terminal) _Sigma_vector.push_back(k);
		for (auto k : this->mp) {//注意没定义的情况 要加上#  好像不加也没事...
			for (auto kk : k.second) {
				assert(kk.size() <= 2 && kk.size()>0);
				if (kk.size() == 1) {
					//cout << kk << endl;
					terminal_flag = true;
					_matrix[string(1, k.first)][kk[0]].insert("Z");
				}
				else if(kk.size() == 2){
					//cout << kk << endl;
					assert(is_capital(kk[1]));
					assert(is_lowercase(kk[0]));
					_matrix[string(1, k.first)][kk[0]].insert(string(1, kk[1]));
				}
			}
		}
		_start = "S";//RG默认是S开始
		assert(terminal_flag);
		_terminal.push_back("Z");
		for (auto k : _Sigma_vector) {
			for (auto kk : _matrix) if (!_matrix[kk.first].count(k)) _matrix[kk.first][k].insert("#");
			_matrix["Z"][k].insert("#");
		}
		return FA(_matrix, _start, _terminal, _Sigma_vector);
	}
};

FA init_FA(string path) {
	umap<string, umap<char, set<string> > > _matrix;
	vector<string> _terminal;//开始状态 结束状态
	vector<char> char_vec;

	ifstream read(path);
	int n, m;
	read >> n;
	cout << n << endl;
	for (int i = 0; i < n; ++i) {
		char tp;
		read >> tp;
		char_vec.push_back(tp);
		cout << tp << " ";
	}
	cout << "\n\n";
	read >> m;
	cout << m << endl;
	for (int i = 0; i < m; ++i) {
		bool flag; string tp, tp1;
		read >> tp;
		cout << tp << " ";
		for (int j = 0; j < n; ++j) {
			read >> tp1;
			cout << tp1 << " ";
			// abc,efg,dsf
			_matrix[tp][char_vec[j]] = split_comma(tp1);
		}
		read >> flag;
		cout << flag << " ";
		if (flag) _terminal.push_back(tp);
		cout << endl;
	}
	string _start;
	read >> _start;
	read.close();
	cout << "\n" << endl;
	return FA(_matrix, _start, _terminal, char_vec, find(char_vec.begin(), char_vec.end(), '$') == char_vec.end() ? 0 : 1);
}

void test_FA2RE() {
	FA FA1 = init_FA("in1.txt");// p49
	FA1.print_matrix();
	//FA1.NFA2DFA();
	FA1.print_matrix();
	cout << FA1.FA2RE() << endl;
}

Grammar init_G(string path) {
	vector<pss> _pcs;
	ifstream read(path);
	string line;
	while (getline(read, line))
	{
		if (line == "") break;
		stringstream grammar_stream(line);
		string arrow, des;
		char src;
		grammar_stream >> src;
		grammar_stream >> arrow;
		while (grammar_stream >> des) {
			if (des[0] == '|') continue;
			else _pcs.push_back(make_pair(src, des));
		}
	}
	return Grammar(_pcs, 'S');
}

void test_RG2FA() {//RG2FA  FA2RG
	Grammar G1 = init_G("in2.txt");//p58
	G1.print_G();
	
	FA FA1 = G1.RG2FA();
	FA1.print_matrix();
	FA1.NFA2DFA();
	FA1.print_matrix();
	vector<pss> _pcs = FA1.FA2RG();//注意要先生成_pcs 再调用构造函数 否则grammer_start未更新
	Grammar G2 = Grammar(_pcs, grammer_start);
	G2.print_G();
}

int main()
{
	//freopen("out.txt", "w", stdout);
	//test_FA2RE();
	test_RG2FA();
	return 0;
}
