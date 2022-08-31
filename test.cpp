#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <stack>
#include <queue>
#include <limits.h>
#include <algorithm>
#include <unordered_map>
// #pragma pack(8)
using namespace std;
// int main(){
//     vector<int> vec({1,2,3,4});
//     auto vec_it = vec.begin()+1;
//     const type_info &vectype = typeid(&*vec.begin());
//     cout<<"end_iter\n"<<&*vec.end()<<endl;
//     cout<<"vectype\n"<<vectype.name()<<endl;
//     string s = "asdfgh";
//     auto end_iter = s.begin()+1;
//     const type_info &stype = typeid(&*s.begin());
//     cout<<"end_iter\n"<<&*end_iter<<endl;
//     cout<<"stype\n"<<stype.name()<<endl;
//     cout<<string::npos<<endl;

//     char ch[5] = "asdf";
//     for (int i = 0; i<5; ++i)
//         cout<<ch[i];
//     cout<<endl;
//     cout<<(void*)&ch[0]<<endl;
//     cout<<(void*)&ch[1]<<endl;
//     cout<<(void*)&ch[2]<<endl;
//     return 0;
// }

class A {
public:
	string* id;
    int capacity;
	A(int cap = 5)  { 
        capacity = cap;
        printf("cons\n");
		id = new string[capacity];
	}
	virtual ~A() { 
        printf("decons\n");
		delete[] id;
	}
    int bfunc(){
        printf("afunc\n");
    }
};
class B : public A{
public:
    int capacity;
    int bbb;
    B(){
        printf("Bcons\n");
        capacity = 10;
        bbb = 456;
    }
    ~B(){
        printf("BBBdecons\n");
    }
    void bfunc (){
        printf("bfunc\n");
    }
    void InB(){
        bfunc();
    }
};

A* reint(){
    A *n = new A();
    printf("reint\n");
    return n;
}
struct S1{
    int i:8;
    char j:4;
    int a:4;
    double b;
};
struct S2{
    int i:8;
    char j:4;
    double b;
    int a:4;
    
};
struct S3{
    int i;
    char j;
    double b;
    int a;
    
};

 
int main(){

    // A* p = new B();
    // dynamic_cast<B*> (p)->InB();
    // vector<int> vec;
    // vec.resize(5);

    // auto begin = vec.begin();
    // cout<<typeid(*begin).name()<<endl;
    // cout<<&begin<<endl;
    // auto add = vec.end();
    // cout<<&add<<endl;

    // delete p;
    // return 0;
    // cout<<sizeof(char)<<endl;
    // cout<<sizeof(int)<<endl;
    // cout<<sizeof(double)<<endl;
    
    // cout<<sizeof(S1)<<endl;
    // cout<<sizeof(S2)<<endl;
    // cout<<sizeof(S3)<<endl;
//////大小端和静态变量地址增长顺序的问题/////////////////////////////////////////////
    // int val = 1024;
    // // int val2 = 1024;
    // // int val3 = 1598;
    // // bool b = true;
    // // cout<<&val<<endl;
    // // cout<<&val2<<endl;
    // // cout<<&val3<<endl;
    // // cout<<&b<<endl;


    // bool b = *((bool*)(&val));
    // cout<<sizeof(bool)<<endl;
    // cout<<b<<endl;
    // cout<<&b<<endl;
    // cout<<&val<<endl;
    // cout<<(char)val<<endl;
    // cout<<(bool*)(&val)<<endl;

    // val++;
    // cout<<val<<endl;
    // b = *((bool*)(&val));

    // cout<<b<<endl;
    //     cout<<&val<<endl;
    //     cout<<(char)val<<endl;
        
    // cout<<(bool*)(&val)<<endl;
///////////////////////////////////////////////////////////////////////////////////
    // string s("abcdef");
    // auto iter = s.begin();

    // s.erase(s[1]);
    // cout<<s<<endl;
    vector<string> words({"a","b","ba","bca","bda","bdca"});
        sort(words.begin(),words.end(),[](string s1, string s2)->bool{return s1.size()<s2.size();});
        unordered_map<string, int> dp;
        int res = 0;
        for (string word : words){
            dp[word] = 1;
            if (word.size()==1)
                continue;
            
            for (auto iter = word.begin(); iter != word.end(); iter++){
                string temp = word;
                iter = temp.erase(iter);
                if (dp.count(temp)!=0){
                    dp[word] = max(dp[word], dp[temp]+1);
                }
                
            }
            res = max(res,dp[word]);
        }
        cout<<res<<endl;
}