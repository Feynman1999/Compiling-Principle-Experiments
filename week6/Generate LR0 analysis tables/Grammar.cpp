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
	printf("����%d��node\n", tot);
	for (int i = 0; i < tot;++i) {
		assert(node[i].size());
		printf("#%d\n", i);
		for (auto k : node[i]) {
			string tp = PP[k.first].second;
			printf("%c -> %s\n", PP[k.first].first, tp.insert(k.second, "*").c_str());
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

// ���һ��״̬�Ƿ��Ѿ�����
int LR_0::check_exist(const set<pair<int, int>> & node1,const int & tot)
{
	for (int i = 0; i < tot; ++i) {
		if (node[i] == node1) return i;
	}
	return -1;//û����ͬ��
}

void LR_0::closure(int now) {
	while (1) {
		int before_size = node[now].size();
		for (auto k : node[now]) {// k is pair<PP_id,point location>
			char c = '@';//Ҫ��֤@��Զ�������
			if (k.second < PP[k.first].second.size()) c = PP[k.first].second[k.second];//point�������
			if (VN.count(c)) {//���������Ƿ��ս����
				for (auto right : P[c]) {//������Ӧ���Ƶ�ʽ
					int PPid = P2id[string(1, c) + '#' + right];//�÷��ս���Ŷ�Ӧ�ı��ʽ �ı��
					node[now].insert(make_pair(PPid, 0));//�õ�id �� СԲ��Ϊ0
				}
			}
		}
		if (node[now].size() == before_size) break;
	}
}

bool LR_0::check_acc(int now) {//���ĳ��״̬�Ƿ�Ϊacc״̬
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
	//�ȶ�֮ǰ���еĲ���ʽ���б��  ���������ӡ��˳��
	//Ϊ��hash����   ����ȡ pair<left, right> ��Ϊ�������ʽ��Ψһ��ʶ
	PP.clear();
	for(auto left : VN) for (auto right : P[left]) PP.push_back(make_pair(left, right));
	int tp = 0;
	for (auto k : PP) {P2id[string(1,k.first) + '#' + k.second] = tp;tp++;}

	//���濪ʼ��ͼ
	queue<int> que;  while (!que.empty()) {que.pop();}
	node[0].insert(make_pair(0, 0));	// $ -> E Ϊ0  СԲ��Ҳһ������λ��  ��һ��ʼ�ĺ�
	que.push(0); closure(0); tot=1;
	while (!que.empty()) {
		int now = que.front();
		que.pop();
		//�����ɵ�ǰ�ڵ������չ  ע��pushʱҪ���ж��Ƿ����Ѿ����ֹ���״̬  ���Ҹ���G
		vector<char> vn_vt;
		for (auto vn : VN) vn_vt.push_back(vn);
		for (auto vt : VT) vn_vt.push_back(vt);
		for (auto vnt : vn_vt) {
			if (vnt == '$') continue;
			assert(node[tot].size() == 0);
			for (auto k : node[now]) {
				char c = '@';
				assert(k.second >= 0);
				if (k.second < PP[k.first].second.size()) c = PP[k.first].second[k.second];//point�������
				if (c == vnt) node[tot].insert(make_pair(k.first, k.second + 1));//���ʽ�ӿ����ƽ�
			}
			if (node[tot].size()) {
				closure(tot);//�ɺ���չ
				int check_return = check_exist(node[tot], tot);
				if (check_return == -1) { //֮ǰû��
					que.push(tot);
					G[now].push_back(make_pair(vnt, tot));//f(now,vn) = tot
					tot++;
				}
				else { // ֮ǰ�� ���ǲ����¼ӵ� ���Ϊcheck_return
					G[now].push_back(make_pair(vnt, check_return));//f(now,vn) = tot
					node[tot].clear();
				}
			}
		}
	}

	//���������
	
	//�����ͷ
	printf("\n\n\n%-12s%-30s%-18s\n", "Status","Action","GOTO");
	printf("%-12s", "");
	for (auto k : VT) printf("%-6c", k);
	printf("%-6c", '#');
	for (auto k : VN) {if (k == '$') continue;printf("%-6c", k);}
	putchar('\n');
	for (int i = 0; i < 62; ++i) printf("-");
	putchar('\n');
	//�������
	for (int i = 0; i < tot; ++i) {
		printf("%-12d", i);
		//vt
		for (auto k : VT) {
			//find k in G
			bool flag = 0;
			for (auto kk : G[i]) {
				if (kk.first == k) {//��������
					printf("%-6s", (string(1,'S')+int2str(kk.second)).c_str());
					flag = 1;
					break;
				}
			}
			if (!flag) {
				if(G[i].size()==0 && !check_acc(i)) printf("%-6s", ("r"+int2str((*node[i].begin()).first)).c_str());//��Լ
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
