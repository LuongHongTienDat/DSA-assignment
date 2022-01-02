#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"


class Node{
public:
    int pos;
    Node* next;
    Node(){
        pos=-1;
        next=NULL;
    }
    Node(int p, Node* n){
        pos=p;
        next=n;
        return;
    }

};
class Ls{
public:
    Node* head;
    Ls(){
        head=NULL;
        return;
    }
    void push(int pos){
        if (!head){
            head=new Node(pos,NULL);
            return;
        }
        Node* newNode=new Node(pos,head);
        head=newNode;
        return;
    }
    void pop(){
        if (!head) return;
        Node* temp=head;
        head=head->next;
        delete temp;
        return;
    }

    ~Ls(){
        while (head){
            Node* temp=head;
            head=head->next;
            delete temp;
        }
        head=NULL;
        return;
    }
};



class Data{
public:
    string idName,type,argL;
    int scope;
    Data(){
        idName=type=argL="";
        scope=-2;
        // scope = -2: there is no element and tombstone here.
    }
    Data(string id,int sco, string ty, string L){
        idName=id;
        scope=sco;
        type=ty;
        argL=L;
    }


};

class SymbolTable
{
public:
    Ls  nodeL;
    int m,c1,c2,size;
    int (*hf)(string,int, int, int, int);
    Data* data;
    SymbolTable() {
        m=c1=c2=size=0;
        hf=NULL;
        data=NULL;
        nodeL= Ls();
    }
    ~SymbolTable(){
        if (data) delete[] data;
    }
    void run(string filename);
    void insertElement(string cmd,string id,int curscope);
    void insertFunction(string cmd, string id, int numberArg);

    void assignImValue(string cmd,string id,string val,int curscope);
    void assignWithVariable(string cmd, string id, string variable, int curscope);
    void assignWithFunction(string cmd, string id, string func, int curscope);
    void callFunction(string cmd,string func, int curscope);

    void deleteScope(string cmd, int scope);

    Data* lookUp(string cmd,string id, int curscope, int& numberPass);
    void print();


};
int modString(string s, int mod){
    int n=s.length();
    int i=n%7;
    long long res=(i==0)?0:stol(s.substr(0,i))%mod;
    for (;i<n;i+=7){
        res=((res*10000000)%mod + stol(s.substr(i,7)))%mod;
    }
    return res;
}

int hashLinear(string k,int i, int m,int c1, int c2){
    return (modString(k,m) + c1*i)%m;
}
int hashQuadratic(string k,int i, int m,int c1, int c2){
    return (modString(k,m) + c1*i + c2*i*i)%m;


}
int hashDouble(string k,int i, int m,int c1, int c2){
    int t1=modString(k,m), t2=1+ modString(k,m-2);
    return (t1+c1*t2*i)%m;
}


string encodeKey(string s, int scope){
    string res=to_string(scope);
    int lth=s.length();
    for (int i=0;i<lth;i++){
        res+=to_string((int)(s[i]-48));
    }
    return res;
}

#endif
