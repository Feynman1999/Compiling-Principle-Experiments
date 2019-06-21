#include "LL1_Grammar.hpp"
using namespace std;
LL1_G G;
int main()
{
    freopen("Grammar.txt","r",stdin);
    int T;
    cin>>T;
    while(T--)
    {
        string s;
        cin>>s;
        G.add_grammar(s[0],s.substr(3));
    }
    G.set_S('S');
    G.init();
    if(!G.is_LL1()) cout<<"Error! This grammar is not LL(1)!"<<endl;
    else G.Gen_Parser("Parser.c");
    return 0;
}
