#include<bits/stdc++.h>
#include<sstream>
using namespace std;

char lookahead;
string s;
stringstream scin(s);
struct node{
    int f;
    double v;
};

void error(){
    cout<<"syntax error!"<<endl;
    exit(1);
}

void parse_N();

void MatchToken(char expected){
    if(lookahead != expected){
        error();
    }
    else{
        //lookahead = getchar();
        scin>>lookahead;
    }
}

node parse_B(node B){
    switch(lookahead){
        case '0':
            MatchToken('0');
            B.v = 0;//
            return B;
        case '1':
            MatchToken('1');
            B.v = 1.0/pow(2,B.f);
            return B;
        default:
            error();
    }
}

node parse_S(node S){
    node B;
    switch(lookahead){
        case '0':
        case '1':
            B.f = S.f;//
            B = parse_B(B);
            S.f += 1;//
            S = parse_S(S);
            S.v += B.v;//
            return S;
        case '#':
            S.v = 0;//
            return S;
        default:
            error();
    }
}


int main()
{
    cout<<"please input one line"<<endl;
    getline(cin, s);
    if(s[s.length()-1]!='#') s+='#';
    scin = stringstream(s);
    scin>>lookahead;
    parse_N();
    if(lookahead == '#') cout<<"OK!"<<endl;
    else error();
    return 0;
}


void parse_N(){
    node S;S.v=0;
    switch (lookahead){
        case '.':
            MatchToken('.');
            S.f = 1;//
            S = parse_S(S);
            cout<<S.v<<endl;//
            break;
        default:
            error();
    }
}
