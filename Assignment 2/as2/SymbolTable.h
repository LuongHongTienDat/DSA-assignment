#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class TreeNode{
public:
    string idName;
    int scope;
    string type;
    string argLs;

    TreeNode* right,*left,*parent;

    TreeNode();
    TreeNode(string id, int sco, string ty, string aL);

};

class Node{
public:
    string idName;
    int scope;
    Node* next;
    Node(){
        idName="";
        scope=-1;
        next=NULL;
    }
    Node(string id, int sco, Node* n){
        idName=id;
        scope=sco;
        next=n;
        return;
    }

};
class Ls{
public:
    Node* head;
    Node* tail;
    Ls(){
        head=tail=NULL;
        return;
    }
    void push(string id, int sco){
        if (!head){
            head=new Node(id,sco,NULL);
            tail=head;
            return;
        }
        Node* newNode=new Node(id,sco,head);
        head=newNode;
        return;
    }
    void pop(){
        if (!head) return;
        if (!head->next){
            delete head;
            head=tail=NULL;
            return;
        }
        Node* temp=head;
        head=head->next;
        delete temp;
        return;
    }
    void clearLs(){
        while (head){
            Node* temp=head;
            head=head->next;
            delete temp;
        }
        head=tail=NULL;
        return;
    }
    ~Ls(){
        while (head){
            Node* temp=head;
            head=head->next;
            delete temp;
        }
        head=tail=NULL;
        return;
    }
};

bool morethan (TreeNode* node1, TreeNode* node2);
bool morethanorequal (TreeNode* node1, TreeNode* node2);
bool isequal (TreeNode* node1, TreeNode* node2);

void print(TreeNode* node, TreeNode* root);



class SymbolTable
{
public:
    TreeNode* root;
    Ls* lsSymbol;
    SymbolTable();

    void rightRotate(TreeNode* node);
    void leftRotate(TreeNode* node);
    void splayNode(TreeNode* node, int& num_splay);
    void deleteNode(TreeNode* node);


    void run(string filename);
    void insertElement(string cmd,string id, string ty, int sco);
    void insertFunction(string cmd,string id, string rawargLs, int sco);

    TreeNode* lookUp(string cmd,string id, int sco, int& num_comp, int& num_splay);

    void assignImValue(string cmd, string id, string value, int sco);
    void assignWithVariable(string cmd, string id, string variableID, int sco);
    void assignWithFunction(string cmd, string id, string func, int sco);

    void deleteScopeRecursion(string cmd,Node* node, int sco);


    void DestroyRecursive(TreeNode* node)
    {
        if (node)
        {
            DestroyRecursive(node->left);
            DestroyRecursive(node->right);
            delete node;
        }
        root=NULL;
    }

    ~SymbolTable()
    {
        //delete lsSymbol;
    }

};

#endif
