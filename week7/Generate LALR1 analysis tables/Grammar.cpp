#include "Grammar.h"

LALR_1::LALR_1()
{
	VN.clear();
	VT.clear();
	P.resize(maxn);
	for (int i = 0; i < maxn; ++i) P[i].clear();//������maxn������ʽ
	for (int i = 0; i < maxn; ++i) node1[i].clear();
	Empty.resize(maxn);
	for (int i = 0; i < maxn; ++i) Empty[i] = 0;
	First.resize(maxn);
	for (int i = 0; i < maxn; ++i) First[i].reset();
	for (int i = 0; i < maxn; ++i) G1_G2[i] = -1;
	for (int i = 0; i < maxn; ++i) {
		G1[i].clear();
		G2[i].clear();
	}
	S = 'S';//Ĭ�Ͽ�ʼ��
	num = 0;
	tot1 = 0;
	tot2 = 0;
}


void LALR_1::add_grammar(char left, string s)
{
	++num;
	assert(left >= 'A' && left <= 'Z' || left == '$');
	VN.insert(left);
	for (auto ch : s) {
		if (ch >= 'A'&&ch <= 'Z') VN.insert(ch);
		else VT.insert(ch);
	}
	P[left].push_back(s);
}


void LALR_1::set_S(char c)
{
	assert(VN.count(c));
	add_grammar('$', string(1, c));
	VN.insert('$');
	S = '$';
}


void LALR_1::init()
{
	cal_Empty();
	cal_First();
	//�ȶ�֮ǰ���еĲ���ʽ���б�� (from 0 start)
	//Ϊ��hash����   ����ȡ pair<left, right> ��Ϊ�������ʽ��Ψһ��ʶ
	PP.clear();
	for (auto left : VN) for (auto right : P[left]) PP.push_back(make_pair(left, right));
	int tp = 0;
	for (auto k : PP) { P2id[string(1, k.first) + '#' + k.second] = tp; tp++; }
}


void LALR_1::cal_Empty()
{
	while (true)
	{
		bool update = false;
		for (auto left : VN)
			if (!Empty[left])
			{
				for (auto s : P[left])
				{
					if (s.length() == 1 && s[0] == 'e') { Empty[left] = 1; break; }// �����Ƕ��e�����
					Empty[left] = 1;
					for (auto ch : s) if (!Empty[ch]) { Empty[left] = 0; break; }// ����ֻҪ��һ�����Ʋ����յ� ��break
					if (Empty[left]) break;//������Ƴ��� ����|�ľͲ��ÿ���
				}
				if (Empty[left]) update = true;//������Ƴ����־ͼ�����
			}
		//cout<<update<<endl;
		if (!update) break;//��һ��û���µ��Ƴ��յķ��ս������
	}
}


void LALR_1::cal_First()
{
	//for(auto x:VT) cout<<x<<" : "<<Empty[x]<<endl;
	//for(auto x:VN) cout<<x<<" : "<<Empty[x]<<endl;
	for (auto x : VT) First[x].set(x);
	First['#'].set('#');
	while (true)
	{
		bool update = false;
		for (auto left : VN)
		{
			int last = First[left].count();
			for (auto s : P[left]) First[left] |= first(s);
			if (Empty[left]) First[left].set('e');// ò�ƿ���ɾȥ
			if (First[left].count() != last) update = true;
		}
		if (!update) break;
	}
}


bitset<maxn> LALR_1::first(string s)
{
	bitset<maxn> ans;
	ans.reset();
	for (auto ch : s)
		if (!Empty[ch])
		{
			ans |= First[ch];
			return ans;
		}
		else
		{
			ans |= First[ch];
			ans['e'] = 0;
		}
	ans.set('e');//�������û��return �����Ƴ���
	return ans;
}


void LALR_1::display_G()
{
	cout << "This Grammar has " << VN.size() << " VN and " << VT.size() << " VT and " << num << " productions." << endl;
	cout << "Productions : (the 'e' denote Epsilon)" << endl;
	int tp = 0;
	for (auto left : VN)
		for (auto right : P[left]) printf("%d:  %c -> %s\n", tp++, left, right.c_str());
	cout << "The start word is " << S << endl;
	putchar('\n');
}


void LALR_1::display_FA(const vector<pair<char, int>> G[maxn], int tot)
{
	printf("����%d��node\n", tot);
	for (int i = 0; i < tot; ++i) {
		assert(node1[i].size());
		printf("#%d\n", i);
		for (auto k : node1[i]) {
			string tp = PP[k.first.first].second;
			printf("%c -> %s, %s\n", PP[k.first.first].first, tp.insert(k.first.second, ".").c_str(), set2str(k.second).c_str());
		}
		if (G[i].size()) {
			printf("�õ����ӵ�:\n");
			for (auto k : G[i]) printf("%c -> %d\n", k.first, k.second);
		}
		else printf("�õ����Ϊ0\n");
		putchar('\n');
		putchar('\n');
	}
}


void LALR_1::display_FA_LR() {
	cout << "LR(1)" << endl;
	display_FA(G1, tot1);
}


void LALR_1::display_FA_LALR() {
	//�����Ӧ��ϵ
	cout << "LR1��״̬��ӦLALR1�ı�ţ�" << endl;
	for (int i = 0; i < tot1; ++i) {
		cout << i << " -> " << G1_G2[i] << endl;
	}
	cout << "LALR1�ı�Ű���֮ǰ�ı�ţ�" << endl;
	for (int i = 0; i < tot2; ++i) {
		cout << i << " -> ";
		for (auto k :G2_G1[i]) {
			cout << k << " ";
		}
		cout << endl;
	}
	cout << endl;
	//���ͼ
	cout << "LALR(1)" << endl;
	display_FA(G2, tot2);
	cout << endl;
}


// ���һ��״̬�Ƿ��Ѿ�����
int LALR_1::check_exist(const set<iic> & node11, const int & tot)
{
	for (int i = 0; i < tot; ++i) {
		if (node1[i] == node11) return i;
	}
	return -1;//û����ͬ��
}


void LALR_1::node_merge(int id) {
	set<iic> ans; ans.clear();
	set<iic> tp = node1[id];
	assert(tp.size());
	vector<iic> vec;
	set<char> hp;
	for (auto k : tp) {vec.push_back(k);}
	hp = vec[0].second;
	for (int i = 1; i < vec.size(); ++i) {
		if (vec[i].first == vec[i - 1].first) hp = union_set(hp, vec[i].second);
		else {
			ans.insert(make_pair(vec[i-1].first, hp));
			hp = vec[i].second;
		}
	}
	ans.insert(make_pair(vec[vec.size()-1].first, hp));
	node1[id] = ans;
}


void LALR_1::closure(int now) {
	while (1) {
		int before_size = node1[now].size();
		for (auto k : node1[now]) {// k is pair<PP_id,point location>
			char c = '@';//Ҫ��֤@��Զ���������VN / VT��
			if (k.first.second < PP[k.first.first].second.size()) c = PP[k.first.first].second[k.first.second];//point�������
			if (VN.count(c)) {//���������Ƿ��ս����
				for (auto right : P[c]) {//������Ӧ���Ƶ�ʽ
					int PPid = P2id[string(1, c) + '#' + right];//�÷��ս���Ŷ�Ӧ�ı��ʽ �ı��
					bitset<maxn> bs;
					bs.reset();
					string leave = PP[k.first.first].second.substr(k.first.second+1);//�ӷ��ս���ź�ʼ ����Ϊ��
					for (auto kk : k.second) {//��ÿ�ֿ��ܵ�first��ȡ��
						bs |= first(leave + kk);
					}
					assert(bs['e'] == false);//���ڼ���kk ��֤first(leave+kk)�в�������
					node1[now].insert(make_pair(make_pair(PPid, 0), bs2set(bs)));//�õ�id �� СԲ��Ϊ0
				}
			}
		}
		if (node1[now].size() == before_size) break;
	}
	//�հ�֮����ͬ����ʽ��second(set)�ϲ�
	node_merge(now);
}

bool LALR_1::check_sameheart(int id1,int id2) {
	if (node1[id1].size() != node1[id2].size()) return false;
	vector<iic> vec1, vec2;
	for (auto k : node1[id1]) vec1.push_back(k);
	for (auto k : node1[id2]) vec2.push_back(k);
	for (int i = 0; i < vec1.size(); ++i) {
		if (vec1[i].first != vec2[i].first) return false;
	}
	return true;
}

void LALR_1::merge_sameheart(set<iic> & s1, const set<iic> & s2) {
	set<iic> st;
	vector<iic> vec1, vec2;
	for (auto k : s1) vec1.push_back(k);
	for (auto k : s2) vec2.push_back(k);
	for (int i = 0; i < vec1.size(); ++i) {
		assert(vec1[i].first == vec2[i].first);
		st.insert(make_pair(vec1[i].first, union_set(vec1[i].second, vec2[i].second)));
	}
	s1 = st;
}


//ע���������ֱ����LALR1 �е�ʱֱ�Ӻϲ�����
void LALR_1::get_Analysis_table()
{
	//���濪ʼ��LR1ͼ
	set<char> st;
	st.insert('#');
	queue<int> que;  while (!que.empty()) { que.pop(); }
	node1[0].insert(make_pair(make_pair(0, 0), st));	// $ -> S ���Ϊ0  СԲ��Ҳһ������λ��  ����������Ϊ'#' ��һ��ʼ�ĺ�
	que.push(0); closure(0); tot1 = 1;
	while (!que.empty()) {
		int now = que.front();
		que.pop();
		//�����ɵ�ǰ�ڵ������չ  ע��pushʱҪ���ж��Ƿ����Ѿ����ֹ���״̬  ���Ҹ���G
		vector<char> vn_vt;
		for (auto vn : VN) vn_vt.push_back(vn);
		for (auto vt : VT) vn_vt.push_back(vt);
		for (auto vnt : vn_vt) {
			if (vnt == '$') continue;//�����������ʼ�����ÿ���
			assert(node1[tot1].size() == 0);//Ŀǰ��û�в���ʽ
			for (auto k : node1[now]) {
				char c = '@';
				assert(k.first.second >= 0);
				if (k.first.second < PP[k.first.first].second.size()) c = PP[k.first.first].second[k.first.second];//point�������
				if (c == vnt) node1[tot1].insert(make_pair(make_pair(k.first.first, k.first.second + 1),k.second));//���ʽ�ӿ����ƽ�
			}
			if (node1[tot1].size()) {
				closure(tot1);//�ɺ���չ
				int check_return = check_exist(node1[tot1], tot1);
				if (check_return == -1) { //֮ǰû��
					que.push(tot1);
					G1[now].push_back(make_pair(vnt, tot1));//f(now,vn) = tot
					tot1++;
				}
				else { // ֮ǰ�� ���ǲ����¼ӵ� ���Ϊcheck_return
					G1[now].push_back(make_pair(vnt, check_return));//f(now,vn) = tot
					node1[tot1].clear();
				}
			}
		}
	}
	//�����G1���кϲ�
	for (int i = 0; i < tot1; ++i) {
		if (G1_G2[i] == -1) {
			//����֮���״̬
			vector<int> vec; vec.push_back(i); G1_G2[i] = tot2;
			set<iic> st = node1[i];
			for (int j = i + 1; j < tot1; ++j) {
				if (check_sameheart(i, j)) {
					merge_sameheart(st, node1[j]);
					vec.push_back(j);
					G1_G2[j] = tot2;
				}
			}
			node2[tot2] = st;
			for (auto k : vec) G2_G1[tot2].insert(k);
			tot2++;
		}
	}
	//���µ�G2���н�ͼ��ÿ���ȼ���ȡһ������㼴��
	for (int i = 0; i < tot2; ++i) {
		assert(G2_G1[i].size());
		int represent = *G2_G1[i].begin();
		//����G1�иõ�������Щ��
		set<int> tp;//test  ������G1��һ�������������е㻥��ͬ��
		for (auto k : G1[represent]) {
			//��k������G2���ĸ��� ����ȥ
			assert(tp.count(G1_G2[k.second]) == 0);
			G2[i].push_back(make_pair(k.first, G1_G2[k.second]));
			tp.insert(G1_G2[k.second]);
		}
	}

	/*
		���������   ����֮ǰӦ���ȿ�һ����û���ƽ�-��Լ����Լ-��Լ��ͻ
	*/
	cout << "LALR(1)������" << endl;
	//�����ͷ
	printf("\n\n\n%-12s%-24s%-18s\n", "Status", "Action", "GOTO");
	printf("%-12s", "");
	for (auto k : VT) printf("%-6c", k);
	printf("%-6c", '#');
	for (auto k : VN) { if (k == '$') continue; printf("%-6c", k); }
	putchar('\n');
	for (int i = 0; i < 62; ++i) printf("-");
	putchar('\n');
	//�������
	for (int i = 0; i < tot2; ++i) {
		printf("%-12d", i);
		//vt
		for (auto k : VT) {
			//find k in G
			bool flag = 0;
			for (auto kk : G2[i]) {//����ƽ�
				if (kk.first == k) {//��������
					printf("%-6s", (string(1, 'S') + int2str(kk.second)).c_str());
					flag = 1;
					break;
				}
			}
			if (!flag) {//����Լ
				bool flag1 = 0;
				for (auto kk : node2[i]) {
					int PPid = kk.first.first;
					if (kk.second.count(k) && kk.first.second == PP[kk.first.first].second.length()) {
						flag1 = 1;//  �����ͻ ����ֱ��break ֻ����һ�ֹ�Լ����
						printf("%-6s", ("r" + int2str(kk.first.first)).c_str());//��Լ
						break;
					}
				}
				if(!flag1) printf("%-6s", "\\");//error
			}
		}
		//# û���ƽ�
		bool flag1 = 0;
		for (auto kk : node2[i]) {
			int PPid = kk.first.first;
			if (kk.second.count('#') && kk.first.second == PP[kk.first.first].second.length()) {
				flag1 = 1;
				if (kk.first.second == 1 && PP[kk.first.first].second[0] == 'S') {
					printf("%-6s", "ACC");
					break;//ֻ����һ�ֹ�Լ����
				}
				else{
					printf("%-6s", ("r" + int2str(kk.first.first)).c_str());//��Լ
					break;//ֻ����һ�ֹ�Լ����
				}
			}
		}
		if (!flag1) printf("%-6s", "\\");//error

		//vn
		for (auto k : VN) {
			//find k in G
			if (k == '$') continue;
			bool flag = 0;
			for (auto kk : G2[i]) {
				if (kk.first == k) {//��������
					printf("%-6s", (string(1, 'S') + int2str(kk.second)).c_str());
					flag = 1;
					break;
				}
			}
			if (!flag) printf("%-6s", "\\");//error
		}
		putchar('\n');
	}
	putchar('\n\n');
}


string int2str(int x)
{
	if (x == 0) return "0";
	string ans;
	while (x) {
		ans += ((x % 10) + '0');
		x /= 10;
	}
	reverse(ans.begin(), ans.end());
	return ans;
}


string set2str(const set<char> & st) {
	string ans;
	for (auto k : st) {
		ans += k;
		ans += '/';
	}
	if (ans == "") return "";
	if (ans[ans.length() - 1] == '/') {
		ans = ans.substr(0, ans.length() - 1);
	}
	return ans;
}


set<char> bs2set(const bitset<maxn> & bs) {
	set<char> st;
	for (int i = 0; i < maxn; ++i) {
		if (bs[i]) {
			st.insert(char(i));
		}
	}
	return st;
}


set<char> union_set(set<char> s1, set<char> s2) {
	set<char> st;
	for (auto k : s1) st.insert(k);
	for (auto k : s2) st.insert(k);
	return st;
}