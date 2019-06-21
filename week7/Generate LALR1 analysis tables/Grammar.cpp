#include "Grammar.h"

LALR_1::LALR_1()
{
	VN.clear();
	VT.clear();
	P.resize(maxn);
	for (int i = 0; i < maxn; ++i) P[i].clear();//不超过maxn个产生式
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
	S = 'S';//默认开始符
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
	//先对之前所有的产生式进行编号 (from 0 start)
	//为了hash方便   我们取 pair<left, right> 作为这个产生式的唯一标识
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
					if (s.length() == 1 && s[0] == 'e') { Empty[left] = 1; break; }// 不考虑多个e的情况
					Empty[left] = 1;
					for (auto ch : s) if (!Empty[ch]) { Empty[left] = 0; break; }// 这里只要有一个是推不出空的 就break
					if (Empty[left]) break;//如果能推出空 后面|的就不用看了
				}
				if (Empty[left]) update = true;//如果能推出空字就继续做
			}
		//cout<<update<<endl;
		if (!update) break;//这一轮没有新的推出空的非终结符出现
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
			if (Empty[left]) First[left].set('e');// 貌似可以删去
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
	ans.set('e');//如果上面没有return 则能推出空
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
	printf("共有%d个node\n", tot);
	for (int i = 0; i < tot; ++i) {
		assert(node1[i].size());
		printf("#%d\n", i);
		for (auto k : node1[i]) {
			string tp = PP[k.first.first].second;
			printf("%c -> %s, %s\n", PP[k.first.first].first, tp.insert(k.first.second, ".").c_str(), set2str(k.second).c_str());
		}
		if (G[i].size()) {
			printf("该点连接到:\n");
			for (auto k : G[i]) printf("%c -> %d\n", k.first, k.second);
		}
		else printf("该点出度为0\n");
		putchar('\n');
		putchar('\n');
	}
}


void LALR_1::display_FA_LR() {
	cout << "LR(1)" << endl;
	display_FA(G1, tot1);
}


void LALR_1::display_FA_LALR() {
	//输出对应关系
	cout << "LR1的状态对应LALR1的编号：" << endl;
	for (int i = 0; i < tot1; ++i) {
		cout << i << " -> " << G1_G2[i] << endl;
	}
	cout << "LALR1的编号包含之前的编号：" << endl;
	for (int i = 0; i < tot2; ++i) {
		cout << i << " -> ";
		for (auto k :G2_G1[i]) {
			cout << k << " ";
		}
		cout << endl;
	}
	cout << endl;
	//输出图
	cout << "LALR(1)" << endl;
	display_FA(G2, tot2);
	cout << endl;
}


// 检测一个状态是否已经存在
int LALR_1::check_exist(const set<iic> & node11, const int & tot)
{
	for (int i = 0; i < tot; ++i) {
		if (node1[i] == node11) return i;
	}
	return -1;//没有相同的
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
			char c = '@';//要保证@永远不会出现在VN / VT中
			if (k.first.second < PP[k.first.first].second.size()) c = PP[k.first.first].second[k.first.second];//point不在最后
			if (VN.count(c)) {//如果点后面是非终结符号
				for (auto right : P[c]) {//遍历对应的推导式
					int PPid = P2id[string(1, c) + '#' + right];//该非终结符号对应的表达式 的编号
					bitset<maxn> bs;
					bs.reset();
					string leave = PP[k.first.first].second.substr(k.first.second+1);//从非终结符号后开始 可能为空
					for (auto kk : k.second) {//求每种可能的first集取并
						bs |= first(leave + kk);
					}
					assert(bs['e'] == false);//由于加了kk 则保证first(leave+kk)中不包括空
					node1[now].insert(make_pair(make_pair(PPid, 0), bs2set(bs)));//得到id 且 小圆点为0
				}
			}
		}
		if (node1[now].size() == before_size) break;
	}
	//闭包之后将相同产生式的second(set)合并
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


//注意这里可以直接求LALR1 判等时直接合并即可
void LALR_1::get_Analysis_table()
{
	//下面开始建LR1图
	set<char> st;
	st.insert('#');
	queue<int> que;  while (!que.empty()) { que.pop(); }
	node1[0].insert(make_pair(make_pair(0, 0), st));	// $ -> S 编号为0  小圆点也一定在零位置  搜索符集合为'#' 即一开始的核
	que.push(0); closure(0); tot1 = 1;
	while (!que.empty()) {
		int now = que.front();
		que.pop();
		//下面由当前节点进行拓展  注意push时要先判断是否是已经出现过的状态  并且更新G
		vector<char> vn_vt;
		for (auto vn : VN) vn_vt.push_back(vn);
		for (auto vt : VT) vn_vt.push_back(vt);
		for (auto vnt : vn_vt) {
			if (vnt == '$') continue;//辅助的这个开始符不用考虑
			assert(node1[tot1].size() == 0);//目前还没有产生式
			for (auto k : node1[now]) {
				char c = '@';
				assert(k.first.second >= 0);
				if (k.first.second < PP[k.first.first].second.size()) c = PP[k.first.first].second[k.first.second];//point不在最后
				if (c == vnt) node1[tot1].insert(make_pair(make_pair(k.first.first, k.first.second + 1),k.second));//这个式子可以移进
			}
			if (node1[tot1].size()) {
				closure(tot1);//由核拓展
				int check_return = check_exist(node1[tot1], tot1);
				if (check_return == -1) { //之前没有
					que.push(tot1);
					G1[now].push_back(make_pair(vnt, tot1));//f(now,vn) = tot
					tot1++;
				}
				else { // 之前有 于是不用新加点 编号为check_return
					G1[now].push_back(make_pair(vnt, check_return));//f(now,vn) = tot
					node1[tot1].clear();
				}
			}
		}
	}
	//下面对G1进行合并
	for (int i = 0; i < tot1; ++i) {
		if (G1_G2[i] == -1) {
			//遍历之后的状态
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
	//对新的G2进行建图，每个等价类取一个代表点即可
	for (int i = 0; i < tot2; ++i) {
		assert(G2_G1[i].size());
		int represent = *G2_G1[i].begin();
		//看在G1中该点连到哪些点
		set<int> tp;//test  理论上G1中一个点连到的所有点互不同心
		for (auto k : G1[represent]) {
			//看k现在是G2的哪个点 连上去
			assert(tp.count(G1_G2[k.second]) == 0);
			G2[i].push_back(make_pair(k.first, G1_G2[k.second]));
			tp.insert(G1_G2[k.second]);
		}
	}

	/*
		输出分析表   在这之前应该先看一下有没有移进-规约、规约-规约冲突
	*/
	cout << "LALR(1)分析表" << endl;
	//输出表头
	printf("\n\n\n%-12s%-24s%-18s\n", "Status", "Action", "GOTO");
	printf("%-12s", "");
	for (auto k : VT) printf("%-6c", k);
	printf("%-6c", '#');
	for (auto k : VN) { if (k == '$') continue; printf("%-6c", k); }
	putchar('\n');
	for (int i = 0; i < 62; ++i) printf("-");
	putchar('\n');
	//输出内容
	for (int i = 0; i < tot2; ++i) {
		printf("%-12d", i);
		//vt
		for (auto k : VT) {
			//find k in G
			bool flag = 0;
			for (auto kk : G2[i]) {//检查移进
				if (kk.first == k) {//有这条边
					printf("%-6s", (string(1, 'S') + int2str(kk.second)).c_str());
					flag = 1;
					break;
				}
			}
			if (!flag) {//检查规约
				bool flag1 = 0;
				for (auto kk : node2[i]) {
					int PPid = kk.first.first;
					if (kk.second.count(k) && kk.first.second == PP[kk.first.first].second.length()) {
						flag1 = 1;//  不会冲突 所以直接break 只有这一种规约可能
						printf("%-6s", ("r" + int2str(kk.first.first)).c_str());//规约
						break;
					}
				}
				if(!flag1) printf("%-6s", "\\");//error
			}
		}
		//# 没有移进
		bool flag1 = 0;
		for (auto kk : node2[i]) {
			int PPid = kk.first.first;
			if (kk.second.count('#') && kk.first.second == PP[kk.first.first].second.length()) {
				flag1 = 1;
				if (kk.first.second == 1 && PP[kk.first.first].second[0] == 'S') {
					printf("%-6s", "ACC");
					break;//只有这一种规约可能
				}
				else{
					printf("%-6s", ("r" + int2str(kk.first.first)).c_str());//规约
					break;//只有这一种规约可能
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
				if (kk.first == k) {//有这条边
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