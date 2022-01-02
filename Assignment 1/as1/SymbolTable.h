#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"



class Node{
public:
    string idenName;
    int scope;
    string type;
    string value;
    bool isactivated;
    Node* next;
    Node();
    Node(string id, int sco, string ty, string val, Node* n);

};

class SymbolTable {
public:
    Node* head;
    SymbolTable();
    void run(string filename);
    void insertElement(string cmd,string id, string ty, int sco);
    void assignValue(string cmd,string id, string val);
    void deleteScope(int sco);
    void lookUp(string cmd, string id);
    void print();
    void rprint();

};
void printNode(Node* cur);
void rprintNode(Node* cur);
#endif
