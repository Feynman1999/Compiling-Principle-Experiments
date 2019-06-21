#include "Grammar.h"

LR_0::LR_0()
{
	VN.clear();
	VT.clear();
	P.resize(maxn);
	for (int i = 0; i < maxn; ++i) P[i].clear();
	for (int i = 0; i < maxn; ++i) node[i].clear();
	S = 'S';
	num = 0;
	tot = 0;
}

void LR_0::add_grammar(char left, string s)
{
	++num;
	assert(left >= 'A' && left <= 'Z' || left=='$');
	VN.insert(left);
	for (auto ch : s) {
		if (ch >= 'A'&&ch <= 'Z') VN.insert(ch);
		else VT.insert(ch);
	}
	P[left].push_back(s);
}

void LR_0::set_S(char c)
{
	assert(VN.count(c));
	add_grammar('$', string(1,c));
	VN.insert('$');
	S = '$';
}

void LR_0::display_G()
{
	cout << "This LR(0) Grammar has " << VN.size() << " VN and " << VT.size() << " VT and " << num << " productions." << endl;
	cout << "Productions : (the 'e' denote Epsilon)" << endl;
	int tp = 0;
	for (auto left : VN)
		for (auto right : P[left]) printf("%d:  %c -> %s\n", tp++, left, right.c_str());
	cout << "The start word is " << S << endl;
	putchar('\n');
}

void LR_0::display_FA()
{
	printf("共有%d个node\n", tot);
	for (int i = 0; i < tot;++i) {
		assert(node[i].size());
		printf("#%d\n", i);
		for (auto k : node[i]) {
			string tp = PP[k.first].second;
			printf("%c -> %s\n", PP[k.first].first, tp.insert(k.second, "*").c_str());
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

// 检测一个状态是否已经存在
int LR_0::check_exist(const set<pair<int, int>> & node1,const int & tot)
{
	for (int i = 0; i < tot; ++i) {
		if (node[i] == node1) return i;
	}
	return -1;//没有相同的
}

void LR_0::closure(int now) {
	while (1) {
		int before_size = node[now].size();
		for (auto k : node[now]) {// k is pair<PP_id,point location>
			char c = '@';//要保证@永远不会出现
			if (k.second < PP[k.first].second.size()) c = PP[k.first].second[k.second];//point不在最后
			if (VN.count(c)) {//如果点后面是非终结符号
				for (auto right : P[c]) {//遍历对应的推导式
					int PPid = P2id[string(1, c) + '#' + right];//该非终结符号对应的表达式 的编号
					node[now].insert(make_pair(PPid, 0));//得到id 且 小圆点为0
				}
			}
		}
		if (node[now].size() == before_size) break;
	}
}

bool LR_0::check_acc(int now) {//检测某个状态是否为acc状态
	if (G[now].size() == 0) {
		assert(node[now].size() == 1);
		int ppid = (*node[now].begin()).first;
		int loc = (*node[now].begin()).second;
		if (PP[ppid].first == '$' && loc == PP[ppid].second.length()) return true;
	}
	return false;
}	

void LR_0::get_Analysis_table()
{
	//先对之前所有的产生式进行编号  按照上面打印的顺序
	//为了hash方便   我们取 pair<left, right> 作为这个产生式的唯一标识
	PP.clear();
	for(auto left : VN) for (auto right : P[left]) PP.push_back(make_pair(left, right));
	int tp = 0;
	for (auto k : PP) {P2id[string(1,k.first) + '#' + k.second] = tp;tp++;}

	//下面开始建图
	queue<int> que;  while (!que.empty()) {que.pop();}
	node[0].insert(make_pair(0, 0));	// $ -> E 为0  小圆点也一定在零位置  即一开始的核
	que.push(0); closure(0); tot=1;
	while (!que.empty()) {
		int now = que.front();
		que.pop();
		//下面由当前节点进行拓展  注意push时要先判断是否是已经出现过的状态  并且更新G
		vector<char> vn_vt;
		for (auto vn : VN) vn_vt.push_back(vn);
		for (auto vt : VT) vn_vt.push_back(vt);
		for (auto vnt : vn_vt) {
			if (vnt == '$') continue;
			assert(node[tot].size() == 0);
			for (auto k : node[now]) {
				char c = '@';
				assert(k.second >= 0);
				if (k.second < PP[k.first].second.size()) c = PP[k.first].second[k.second];//point不在最后
				if (c == vnt) node[tot].insert(make_pair(k.first, k.second + 1));//这个式子可以移进
			}
			if (node[tot].size()) {
				closure(tot);//由核拓展
				int check_return = check_exist(node[tot], tot);
				if (check_return == -1) { //之前没有
					que.push(tot);
					G[now].push_back(make_pair(vnt, tot));//f(now,vn) = tot
					tot++;
				}
				else { // 之前有 于是不用新加点 编号为check_return
					G[now].push_back(make_pair(vnt, check_return));//f(now,vn) = tot
					node[tot].clear();
				}
			}
		}
	}

	//输出分析表
	
	//输出表头
	printf("\n\n\n%-12s%-30s%-18s\n", "Status","Action","GOTO");
	printf("%-12s", "");
	for (auto k : VT) printf("%-6c", k);
	printf("%-6c", '#');
	for (auto k : VN) {if (k == '$') continue;printf("%-6c", k);}
	putchar('\n');
	for (int i = 0; i < 62; ++i) printf("-");
	putchar('\n');
	//输出内容
	for (int i = 0; i < tot; ++i) {
		printf("%-12d", i);
		//vt
		for (auto k : VT) {
			//find k in G
			bool flag = 0;
			for (auto kk : G[i]) {
				if (kk.first == k) {//有这条边
					printf("%-6s", (string(1,'S')+int2str(kk.second)).c_str());
					flag = 1;
					break;
				}
			}
			if (!flag) {
				if(G[i].size()==0 && !check_acc(i)) printf("%-6s", ("r"+int2str((*node[i].begin()).first)).c_str());//规约
				else printf("%-6s", "\\");//error
			}
		}
		//#
		if (G[i].size() == 0) {
			if (check_acc(i)) printf("%-6s", "ACC");
			else printf("%-6s", ("r" + int2str((*node[i].begin()).first)).c_str());//error
		}
		else printf("%-6s", "\\");//error
		//vn
		for (auto k : VN) {
			//find k in G
			if (k == '$') continue;
			bool flag = 0;
			for (auto kk : G[i]) {
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
}

string int2str(int x)
{
	if (x == 0) return "0";
	string ans;
	while (x) {
		ans += ((x % 10)+'0');
		x /= 10;
	}
	reverse(ans.begin(), ans.end());
	return ans;
}
