/***
*	main.cpp
*	Created by feynman1999 on 2019/3/5 13:38:25.
*   in machine: FEYNMAN
***/
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <set>
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <bitset>
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


vector<string> ans;
vector<string> thought_opera = { "+", "-", "*", "/", "(", ")" };
short Priority[5] = { 0,0,1,1,-1 };

bool is_opera(string tp) {
	if (tp == "+" || tp == "-" || tp == "*" || tp == "/" || tp == "(" || tp == ")")  return true;
	return false;
}

ll string_to_num(string tp) {
	ll flag = 1;
	if (tp[0] == '-') {
		flag = -1;
		tp = tp.substr(1);
	}
	ll num = 0;
	for (int i = 0; i < tp.length(); ++i) {
		num *= 10;
		num += tp[i] - '0';
	}
	return flag * num;
}

int main()
{
	freopen("in.txt", "r", stdin);
	string s;
	while (getline(cin, s)) {
		if (s == "0") break;
		ans.clear();
		stack<string> opera;
		stringstream scin(s);
		string tmp;
		while (scin >> tmp)
		{
			if (is_opera(tmp)) {
				if (tmp == ")") {
					while (opera.top() != "(")//数据保证一定有(
					{
						ans.push_back(opera.top());
						opera.pop();
					}
					opera.pop();//括号不用输出
				}
				else if (tmp == "(") {
					opera.push(tmp);
				}
				else {
					short id1 = find(thought_opera.begin(), thought_opera.end(), tmp) - thought_opera.begin();
					while (!opera.empty())
					{
						short id2 = find(thought_opera.begin(), thought_opera.end(), opera.top()) - thought_opera.begin();
						if (Priority[id1] <= Priority[id2]) {
							ans.push_back(opera.top());
							opera.pop();
						}
						else {
							break;
						}
					}
					opera.push(tmp);
				}
			}
			else {
				ans.push_back(tmp);// is number
			}
		}
		while (!opera.empty())
		{
			ans.push_back(opera.top());
			opera.pop();
		}
		cout << "原式子（中缀表达式）:\n";
		cout << s << endl;
		cout << "对应的逆波兰表达式 :\n";
		for (auto k : ans) {
			cout << k << " ";
		}
		cout << endl;
		stack<double> number;//由逆波兰表达式求值
		for (auto k : ans) {
			if (is_opera(k)) {
				double number1 = number.top();
				number.pop();
				double number2 = number.top();
				number.pop();
				if (k == "-") {
					number.push(number2 - number1);
				}
				else if (k == "+") {
					number.push(number2 + number1);
				}
				else if (k == "*") {
					number.push(number2 * number1);
				}
				else {//  "/"
					number.push(number2 / number1);
				}
			}
			else {
				number.push(string_to_num(k));
			}
		}
		printf("表达式的值为：%.2f\n\n", number.top());
	}
}
