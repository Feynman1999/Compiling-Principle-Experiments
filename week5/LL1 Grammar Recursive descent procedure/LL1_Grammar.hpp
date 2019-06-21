#include<vector>
#include<string>
#include<bitset>
#include<set>
#include<iostream>
#include<fstream>
using namespace std;
#ifndef _LL1_
#define _LL1_
//LL(1)文法

const int maxn = 128;

class LL1_G
{
private:
    set<char> VN;               //非终结符
    set<char> VT;               //终结符
    vector<vector<string>> P;   //产生式
    vector<bool> Empty;         //Empty
    vector<bitset<maxn>> First;  //First
    vector<bitset<maxn>> Follow; //Follow
    vector<vector<bitset<maxn>>> Select; //Select  对应上面
    char S;                     //开始符
    int num=0;                  //产生式个数

    void add_VN(char c);
    void add_VT(char c);
    void cal_Empty();
    void cal_First();
    void cal_Follow();
    void cal_Select();
    bitset<maxn> first(string s);

public:
    LL1_G();
    void set_S(char c);
    void add_grammar(char left,string s);
    void display();
    void init();
    bool is_LL1();
    void Gen_Parser(string path);
};

LL1_G::LL1_G()
{
    VN.clear();
    VT.clear();
    P.resize(maxn);
    for(int i=0;i<maxn;++i) P[i].clear();
    First.resize(maxn);
    for(int i=0;i<maxn;++i) First[i].reset();
    Follow.resize(maxn);
    for(int i=0;i<maxn;++i) Follow[i].reset();
    Select.resize(maxn);
    for(int i=0;i<maxn;++i) Select[i].clear();
    Empty.resize(maxn);
    for(int i=0;i<maxn;++i) Empty[i]=0;
    S=0;
    num=0;
}
void LL1_G::add_VN(char c)
{
    VN.insert(c);
}
void LL1_G::add_VT(char c)
{
    VT.insert(c);
}
void LL1_G::set_S(char c)
{
    add_VN(c);
    S=c;
}
void LL1_G::add_grammar(char left,string s)
{
    ++num;
    add_VN(left);
    for(auto ch:s)
        if(ch>='A'&&ch<='Z') add_VN(ch);else add_VT(ch);
    P[left].push_back(s);
}
void LL1_G::display()
{
    cout<<"This LL(1) Grammar has "<<VN.size()<<" VN and "<<VT.size()<<" VT and "<<num<<" productions."<<endl;
    cout<<"Productions : (the 'e' denote Epsilon)"<<endl;
    for(auto left:VN)
        for(auto right:P[left]) cout<<left<<"->"<<right<<endl;
    cout<<"The start word is "<<S<<endl;
}
void LL1_G::init()
{
    cal_Empty();
    cal_First();
    /*for(auto x:VT)
    {
        cout<<x<<" : ";
        for(auto y:VT)
            if(First[x][y]) cout<<y<<" ";
        cout<<endl;
    }
    for(auto x:VN)
    {
        cout<<x<<" : ";
        for(auto y:VT)
            if(First[x][y]) cout<<y<<" ";
        cout<<endl;
    }*/
    cal_Follow();
    /*for(auto x:VN)
    {
        cout<<x<<" (follow) : ";
        for(int i=0;i<128;++i)
            if(Follow[x][i]) cout<<char(i)<<" ";
        cout<<endl;
    }*/
    cal_Select();
    /*for(auto left:VN)
    {
        cout<<left<<" (select) : ";
        for(int i=0;i<Select[left].size();++i)
        {
            cout<<P[left][i]<<" : ";
            for(int j=0;j<128;++j)
                if(Select[left][i][j]) cout<<char(j)<<" ";
            cout<<endl;
        }
        cout<<endl;
    }*/
}
bitset<maxn> LL1_G::first(string s)
{
    bitset<maxn> ans;
    ans.reset();
    for(auto ch:s)
        if(!Empty[ch])
        {
            ans|=First[ch];
            return ans;
        }
        else
        {
            ans|=First[ch];
            ans['e']=0;
        }
    ans.set('e');//如果没有return 则能推出空
    return ans;
}
bool LL1_G::is_LL1()
{
    for(auto left:VN)
    {
        int n=P[left].size();
        for(int i=1;i<n;++i)//枚举所有可能的产生式pair
            for(int j=0;j<i;++j)
                if((Select[left][i]&Select[left][j]).count()!=0) return false;
    }
    return true;
}
void LL1_G::cal_Empty()// 预处理每个非终结符能否推出空字
{
    while(true)
    {
        bool update=false;
        for(auto left:VN)
            if(!Empty[left])
            {
                for(auto s:P[left])
                {
                    if(s.length()==1&&s[0]=='e') {Empty[left]=1;break;}// 不考虑多个e的情况
                    Empty[left]=1;
                    for(auto ch:s) if(!Empty[ch]){Empty[left]=0;break;}// 这里只要有一个是推不出空的 就break
                    if(Empty[left]) break;//如果能推出空 后面|的就不用看了
                }
                if(Empty[left]) update=true;//如果能推出空字就继续做
            }
        //cout<<update<<endl;
        if(!update) break;//这一轮没有新的推出空的非终结符出现
    }
}
void LL1_G::cal_First()
{
    //for(auto x:VT) cout<<x<<" : "<<Empty[x]<<endl;
    //for(auto x:VN) cout<<x<<" : "<<Empty[x]<<endl;
    for(auto x:VT) First[x].set(x);
    while(true)
    {
        bool update=false;
        for(auto left:VN)
        {
            int last=First[left].count();
            for(auto s:P[left]) First[left]|=first(s);
            if(Empty[left]) First[left].set('e');// 貌似可以删去
            if(First[left].count()!=last) update=true;
        }
        if(!update) break;
    }
}
void LL1_G::cal_Follow()
{
    Follow[S].set('#');
    vector<int> last;
    last.resize(maxn);

    while(true)
    {
        bool update=false;
        for(auto left:VN) last[left]=Follow[left].count();
        for(auto left:VN)
            for(auto s:P[left])
                for(int i=0;i<s.length();++i)
                    if(VN.find(s[i])!=VN.end())//是非终结符
                    {
                        bitset<maxn> tmp=first(s.substr(i+1));// 取到最后   如果是空串也没关系  即返回tmp['e']为1 且只有e
                        if(tmp['e']) Follow[s[i]]|=Follow[left];
                        tmp['e']=0;
                        Follow[s[i]]|=tmp;
                    }
        for(auto left:VN)
            if(Follow[left].count()!=last[left]) update=true;
        if(!update) break;
    }
}
void LL1_G::cal_Select()
{
    for(auto left:VN)
    {
        Select[left].resize(P[left].size());
        for(int i=0;i<P[left].size();++i)
        {
            string s=P[left][i];
            bitset<maxn> tmp=first(s);
            if(tmp['e'])
            {
                tmp['e']=0;
                Select[left][i]=tmp|Follow[left];
            }
            else Select[left][i]=tmp;
        }
    }
}
void LL1_G::Gen_Parser(string path)
{
    ofstream out(path);
    out<<"#include<stdio.h>"<<endl;
    out<<"char ch;"<<endl;
    out<<"char get_char()"<<endl;
    out<<"{"<<endl;
    out<<"\treturn getchar();"<<endl;
    out<<"}"<<endl;
    out<<"void error()"<<endl;
    out<<"{"<<endl;
    out<<"\tprintf(\"该字符串是不合法的句子 \\n\");"<<endl;
    out<<"\texit(0);"<<endl;
    out<<"}"<<endl;

    for(auto left:this->VN)
    {
        out<<"void Parser_"<<char(left)<<"()"<<endl;
        out<<"{"<<endl;
        out<<"\tswitch(ch)"<<endl;
        out<<"\t{"<<endl;
        for(int i=0;i<P[left].size();++i)
        {
            string tmp="";
            for(auto ch:P[left][i])
                if(ch=='e') ;
                else if(this->VN.find(ch)!=this->VN.end()) tmp+="\t\t\tParser_"+string(1,char(ch))+"();\n";
                else tmp+="\t\t\tif(ch=='"  + string(1,ch)  +  "') ch=get_char(); else error();\n";
            tmp+="\t\t\tbreak;\n";
            //cout<<tmp<<endl;
            bitset<maxn> sec=this->Select[left][i];//第i个产生式
            for(int ch=sec._Find_first(); ch != sec.size(); ch = sec._Find_next(ch))//这一个产生式所有的select集合中的元素都对应一个case 且他们的tmp一样
            {
                //cout<<char(ch)<<endl;
                out<<"\t\tcase '"<<char(ch)<<"':"<<endl;
                out<<tmp;
            }
        }
        //out<<"\tcase '#': return;"<<endl;
        out<<"\t\tdefault: error();"<<endl;
        out<<"\t}"<<endl;
        out<<"}"<<endl;
    }

    out<<"int main()"<<endl;
    out<<"{"<<endl;
    out<<"\tch=get_char();"<<endl;
    out<<"\tParser_"<<char(this->S)<<"();"<<endl;
    out<<"\tif(ch!='#') error();"<<endl;
    out<<"\tprintf(\"该字符串是合法的句子 \\n\");"<<endl;
    out<<"\treturn 0;"<<endl;
    out<<"}"<<endl;

}
#endif // _LL1_
