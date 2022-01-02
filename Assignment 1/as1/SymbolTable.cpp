#include "SymbolTable.h"


using namespace std;

Node::Node(){
    idenName="";
    value="";
    type="";
    scope=0;
    next=NULL;
}
Node::Node(string id, int sco, string ty, string val, Node* n){
    this->idenName=id;
    this->scope=sco;
    this->type=ty;
    this->value=val;
    this->next=n;
    this->isactivated=true;
}


SymbolTable::SymbolTable(){
    this->head=NULL;
}

void SymbolTable::run(string filename) {
    ifstream f(filename);
    int curscope=0;
    while (!f.eof()){
        string cmd;
        getline(f,cmd);
        if (regex_match(cmd,regex("INSERT [a-z][a-zA-Z0-9_]* (number|string)"))){
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                    pos=i;
                    break;
                }
            }
            insertElement(cmd,cmd.substr(7,pos-7),cmd.substr(pos+1,lth-pos-1),curscope);

        }
        else if (regex_match(cmd,regex("ASSIGN [a-z][a-zA-Z0-9_]* [^]+"))){
                int pos=0,lth=cmd.length();
                for (int i=7;i<lth;i++){
                    if (cmd[i]==' ') {
                            pos=i;
                            break;
                    }
                }
                assignValue(cmd,cmd.substr(7,pos-7),cmd.substr(pos+1,lth-pos-1));
        }
        else if (cmd=="BEGIN") {
            curscope++;
        }
        else if (cmd=="END"){
            curscope--;
            if (curscope<0){

                Node* p = this->head;
                while (p){
                    Node*tem=p->next;
                    delete p;
                    p=tem;
                }
                this->head=NULL;
                UnknownBlock e;
                throw(e);
            }
            deleteScope(curscope+1);
        }
        else if (regex_match(cmd,regex("LOOKUP [a-z][a-zA-Z0-9_]*"))){
            lookUp(cmd, cmd.substr(7));
        }
        else if (cmd=="PRINT"){
            print();
        }
        else if (cmd=="RPRINT"){
            rprint();
        }
        else {
            Node* p = this->head;
            while (p){
                Node*tem=p->next;
                delete p;
                p=tem;
            }
            this->head=NULL;

            InvalidInstruction e(cmd);
            throw(e);
        }

    }
    if (curscope>0) {
        Node* p = this->head;
        while (p){
            Node*tem=p->next;
            delete p;
            p=tem;
        }
        this->head=NULL;

        UnclosedBlock e(curscope);
        throw(e);
    }
    f.close();
    Node* p = this->head;
    while (p){
        Node*tem=p->next;
        delete p;
        p=tem;
    }
    this->head=NULL;
    return;
}

void SymbolTable::insertElement(string cmd,string id, string ty, int sco){
    if (!this->head){
        this->head= new Node(id,sco,ty,"",NULL);

        cout<<"success"<<endl;

        return;
    }
    Node* cur=head;
    while(cur){
        if (cur->idenName==id && cur->scope==sco){

            Node* p = this->head;
            while (p){
                Node*tem=p->next;
                delete p;
                p=tem;
            }
            this->head=NULL;
            Redeclared e(cmd);
            throw(e);
        }
        if (cur->idenName==id){
            cur->isactivated=false;
        }
        cur=cur->next;
    }
    Node* newnode= new Node(id,sco,ty,"",this->head);
    this->head=newnode;
    cout<<"success"<<endl;
    return;
}

void SymbolTable::assignValue(string cmd,string id, string val){
    Node* cur=this->head;
    while (cur){
        if (cur->idenName==id) break;
        cur=cur->next;
    }
    if (!cur) {

        Node* p = this->head;
        while (p){
            Node*tem=p->next;
            delete p;
            p=tem;
        }
        this->head=NULL;
        Undeclared e(cmd);
        throw(e);
    }
    if ((cur->type=="number" && regex_match(val,regex("[0-9]+"))) ||
        (cur->type=="string" && regex_match(val,regex("\'[a-zA-Z0-9 ]*\'"))))
    {
        cur->value=val;
        cout<<"success"<<endl;
        return;
    }
    if ((cur->type=="number" && regex_match(val,regex("\'[a-zA-Z0-9 ]*\'"))) ||
        (cur->type=="string" && regex_match(val,regex("[0-9]+"))))
    {
        TypeMismatch e(cmd);

        Node* p = this->head;
        while (p){
            Node*tem=p->next;
            delete p;
            p=tem;
        }
        this->head=NULL;

        throw(e);
    }
    if (!regex_match(val,regex("[a-z][a-zA-Z0-9_]*"))){

            Node* p = this->head;
            while (p){
                Node*tem=p->next;
                delete p;
                p=tem;
            }
            this->head=NULL;
            InvalidInstruction e(cmd);
            throw(e);
    }
    Node* cur2=this->head;
    while (cur2){
        if (cur2->idenName==val){
            if (cur2->type==cur->type){
                cur->value=cur2->value;
                cout<<"success"<<endl;
                return;
            }
            else{
                TypeMismatch e(cmd);

                Node* p = this->head;
                while (p){
                    Node*tem=p->next;
                    delete p;
                    p=tem;
                }
                this->head=NULL;

                throw(e);
            }
        }
        cur2=cur2->next;
    }
    if (cur2==NULL) {

        Node* p = this->head;
        while (p){
            Node*tem=p->next;
            delete p;
            p=tem;
        }
        this->head=NULL;
        Undeclared e(cmd);
        throw(e);
    }

    return;
}

void SymbolTable::deleteScope(int sco){
    if (!this->head) return;
    Node*cur=head, *pre=NULL;
    while (cur && cur->scope!=sco){
        pre=cur;
        cur=cur->next;
    }
    if (cur==this->head){
        while (cur && cur->scope==sco){
            Node* act=cur->next;
            while (act){
                if (act->idenName==cur->idenName) {
                    act->isactivated=true;
                    break;
                }
                act=act->next;
            }
            this->head=this->head->next;
            delete cur;
            cur=this->head;
        }
        return;
    }
    if (cur==NULL) return;
    while (cur && cur->scope==sco){
        Node* act=cur->next;
        while (act){
            if (act->idenName==cur->idenName) {
                act->isactivated=true;
                break;
            }
            act=act->next;
        }
        pre->next=cur->next;
        delete cur;
        cur=pre->next;
    }
    return;
}

void SymbolTable::lookUp(string cmd, string id){
    Node* cur=this->head;
    while (cur && cur->idenName!=id){
        cur=cur->next;
    }
    if (!cur) {
        Undeclared e(cmd);

        Node* p = this->head;
        while (p){
            Node*tem=p->next;
            delete p;
            p=tem;
        }
        this->head=NULL;

        throw(e);
    }

    cout<<cur->scope<<endl;

    return;


}

void SymbolTable::rprint(){
    if (this->head==NULL){
        return;
    }
    cout<<this->head->idenName<<"//"<<this->head->scope;
    rprintNode(this->head->next);
    cout<<endl;
    return;
}
void rprintNode(Node* cur){
    if (!cur){
        return;
    }
    if (cur->isactivated){
        cout<<" "<<cur->idenName<<"//"<<cur->scope;
    }
    rprintNode(cur->next);
    return;
}
void SymbolTable::print(){
    if (this->head==NULL){
        return;
    }
    printNode(this->head->next);
    cout<<this->head->idenName<<"//"<<this->head->scope<<endl;
    return;
}
void printNode(Node* cur){
    if (!cur){
        return;
    }
    printNode(cur->next);
    if (cur->isactivated){
        cout<<cur->idenName<<"//"<<cur->scope<<" ";
    }
    return;
}

