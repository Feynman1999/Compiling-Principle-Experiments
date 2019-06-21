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

char grammer_start;// ֱ��ȫ����

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
	bool is_simplified;//�Ƿ���С��
	string start;//��ʼ״̬
	set<string> terminal;//��̬�� һ���ַ�����ʾ��̬������
	set<string> K;//״̬�� һ���ַ���Ϊһ��״̬������
	vector<char> Sigma_vector;// �ַ���
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
		// ��ʾ״̬��I�е�ÿ��״̬�����������������ܵ����״̬�ļ���
		// solution: ��ͣ�ÿ�ȥ��չ��ǰ����  ֱ����С���ٱ仯Ϊֹ
		set<string> ans = I;
		while (1) {
			//��I����״̬����һ����չ
			set<string> once_move = Move(union_set(I, ans), '$');
			int before_size = ans.size();
			ans = union_set(ans, once_move);
			if (ans.size() == before_size) break;//û���µ�Ԫ�ؼ���
		}
		return ans;
	}
	set<string> Move(const set<string> & I, char a) {
		// move���� ����Ϊ״̬����J
		// ����J��I������״̬����һ��a���������״̬��ȫ��
		set<string> J;
		for (auto k : I) {
			/*
			if(!this->matrix.count(k)) print_set(I);
			*/
			assert(this->matrix.count(k));
			assert(this->matrix[k].count(a));
			if (conca_comma(this->matrix[k][a]) == "#" || this->matrix[k][a].size()==0) continue;//������: ����# ��������
			J = union_set(J, this->matrix[k][a]);
		}
		return J;
	}
	void NFA2DFA() {
		/*
			ʹ���Ӽ����������Զ�������ȷ����
			�������㣺
			1.$�հ�  ע���ڱ�ʵ����  $��ʾ��
			2.����I��a��ת�� ��ʾΪmove(I,a)
		*/
		cout << "���ڽ���ȷ����\n";
		if (this->is_derministic) {
			cout << "�Ѿ���DFA!" << endl;
			return;
		}
		cout << "ȷ����֮ǰmatrixΪ��" << endl;
		this->print_matrix();
		//�������ȷ����
		/*
			NFA k,sigma,f,k0,kt
			����һ��DFA ����Ԫ����k���Ӽ�
			ӳ������Ǽ��� �� ����
		*/
		int m = 0;//״̬���� ����д�� A~Z
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
		cout << "ȷ�������̱����� * ��ʾ�Ѿ�����" << endl;
		// ȷ����ʼ̬
		assert(C0.count(this->start));
		this->start = help[C0];
		while (!unmark.empty())
		{
			set<string> T = *(unmark.begin());
			printf("%-16s", conca_comma(T).c_str());
			unmark.erase(unmark.begin());
			marked.insert(T);
			// ���T�Ƿ�����ֹ̬
			if (!insert_set(this->terminal, T).empty()) _terminal.insert(help[T]);
			for (auto ch : Sigma_vector) {
				if (ch == '$') continue;
				//��һ���߹��˿� ��һ����move��closure����
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
		// ȥ������
		if (find(this->Sigma_vector.begin(), this->Sigma_vector.end(), '$') != this->Sigma_vector.end()) {
			this->Sigma_vector.erase(find(this->Sigma_vector.begin(), this->Sigma_vector.end(), '$'));
		}
		is_derministic = true;  
		this->terminal = _terminal;
	}
	void Simplify() {
		cout << "���ڽ�����С��" << endl;
		this->NFA2DFA();//�Ƚ���ȷ����
		if (this->is_simplified) {
			cout << "�Ѿ���С��!" << endl;
			return;
		}
		is_simplified = true;
	}
	void print_matrix() {
		int n = Sigma_vector.size();
		int m = K.size();
		printf("\n\nprint_matrix\n");
		printf("******************************************************\n");
		printf("%-16s", "״̬\\����");
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
				assert(mapp[i][mid].size() == 1);//�����Ѿ�ȥ���ر� ��һ��Ϊ1
				in_id.push_back(i);
			}
			if (mapp[mid][i].size()) {
				assert(mapp[mid][i].size() == 1);//�����Ѿ�ȥ���ر� ��һ��Ϊ1
				out_id.push_back(i);
			}
		}
		for (auto k1:in_id) {
			for (auto k2:out_id) {
				//ȥ��k1��id id��k2�ı� �Լ�id��id�Լ��ı�
				string l, r, m;
				m = "";
				l = conca_comma(mapp[k1][mid]); if (l == "$") l = "";
				r = conca_comma(mapp[mid][k2]); if (r == "$") r = "";
				if (mapp[mid][mid].size()) {
					assert(mapp[mid][mid].size() == 1);//�����Ѿ�ȥ���ر� ��һ��Ϊ1
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
		//���¹���һ���ڽӾ���
		set<string> mapp[maxn][maxn];// from 0   ��δ������setΪ��
		for (int i = 0; i < maxn; ++i) for (int j = 0; j < maxn; ++j) mapp[i][j].clear();
		//�������е�״̬����hash
		umap<string, short> hash;
		vector<string> hash_back;
		hash_back.resize(maxn);
		short id = 0;
		//0 1�ֱ����¼ӵĿ�ʼ״̬(start)����ֹ״̬(end)
		hash["Start"] = id;
		hash_back[id++] = "Start";
		hash["End"] = id;
		hash_back[id++] = "End";
		for (auto k : this->K) {
			hash[k] = id; 
			hash_back[id++] = k;
		}
		//Start����֮ǰ�Ŀ�ʼ״̬
		assert(hash.count(this->start));
		//cout << this->start << endl;
		mapp[ hash["Start"] ][ hash[this->start] ].insert("$");
		//֮ǰ����ֹ״̬����End
		for (auto k : this->terminal) {
			assert(hash.count(k));
			mapp[ hash[k] ][ hash["End"] ].insert("$");
			//cout << hash[k] << "->" << hash["End"] << endl;
		}
		//��matrix ���� mapp
		for (auto k1 : this->K) {
			for (auto ch : this->Sigma_vector) {
				//assert(this->matrix[k][ch].size() == 1);  //ȷ����֮��һ������
				for (auto k2 : this->matrix[k1][ch]) {//���û�����Զ�����
					//cout << k1 << "," << ch << "->" << k2 << endl;
					if (k2 == "#") continue;   //ȷ����֮�󲻴����������
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
			//����һ���ر�
			deal_multiedges(mapp, id);
			//ö��ÿ������Ϊ�м��
			for (short i = 0; i < id; ++i) deal_one_node(mapp, id, i);
			//����
			last_edge_num = now_edge_num;
			now_edge_num = get_edge_num(mapp, id);
			if (last_edge_num == now_edge_num) {
				cout << "something wrong!!! now_edge_num=last_edge_num=" << now_edge_num << endl;
				print_mapp(mapp, id);
				exit(233);
			}
		}//over
		//ȡstart -> end �ϵ�ֵ����
		assert(mapp[0][1].size()==1);
		return conca_comma(mapp[0][1]);
	}
	//friend class Grammer;
	vector<pss> FA2RG() {
		vector<pss> _pcs;
		//for (auto sigma : this->Sigma_vector) cout << sigma << endl;
		for (auto status : this->K) {
			//cout << status << endl;
			assert(status.size() == 1);//�ַ�
			// assert(this->start == "S");// ȷ����֮�� ��ʼ���Ų�һ����S��
			for (auto sigma : this->Sigma_vector) {//ȷ����֮�� sigma���޿�
				assert(this->matrix[status][sigma].size()==1);//����ȷ������ ��һ���ж���(��Ϊ"#") ����ֻ��һ��
				string tp = conca_comma(matrix[status][sigma]);// ע������ȷ����֮�����±����
				assert(tp.size() == 1 && is_capital(tp[0]));//������RGת�� ��һ������Ϊ1��Ϊ��д�ַ�
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
		//������������non_terminal;
		for (auto k : mp) {
			if (is_capital(k.first)) non_terminal.insert(k.first);
		}
		//����ֵ(vector)������Ŀǰ����non_terminal���Ϊterminal
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
		cout << "���ս��ַ��У�\n";
		for (auto k : non_terminal) {
			cout << k << " ";
		}
		cout << "\n\n";
	}
	void print_terminal() {
		cout << "�ս��ַ��У�\n";
		for (auto k : terminal) {
			cout << k << " ";
		}
		cout << "\n\n";
	}
	void print_G() {
		cout << "\n\n-------------------------�ķ�����-----------------------\n";
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
		for (auto k : this->mp) {//ע��û�������� Ҫ����#  ���񲻼�Ҳû��...
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
		_start = "S";//RGĬ����S��ʼ
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
	vector<string> _terminal;//��ʼ״̬ ����״̬
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
	vector<pss> _pcs = FA1.FA2RG();//ע��Ҫ������_pcs �ٵ��ù��캯�� ����grammer_startδ����
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
