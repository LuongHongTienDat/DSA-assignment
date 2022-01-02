#include "SymbolTable.h"


bool morethan (TreeNode* node1, TreeNode* node2){
    if (node1->scope>node2->scope) return true;
    else if (node1->scope<node2->scope) return false;
    else {
        if ((node1->idName).compare(node2->idName)<=0) return false;
        else return true;
    }
    return false;
}
bool morethanorequal (TreeNode* node1, TreeNode* node2){
    if (node1->scope>node2->scope) return true;
    else if (node1->scope<node2->scope) return false;
    else {
        if ((node1->idName).compare(node2->idName)<0) return false;
        else return true;
    }
    return false;

};
bool isequal (TreeNode* node1, TreeNode* node2){
    if (node1->scope==node2->scope && node1->idName==node2->idName)
        return true;
    return false;
}


void SymbolTable::rightRotate(TreeNode* node){
    TreeNode* temp=node->left;
    node->left=temp->right;
    if (temp->right){
        temp->right->parent=node;
    }
    temp->parent=node->parent;
    if (node->parent==NULL){
        this->root=temp;
    }
    else if (node==node->parent->right){
        node->parent->right=temp;
    }
    else {
        node->parent->left=temp;
    }
    temp->right=node;
    node->parent=temp;
}
void SymbolTable::leftRotate(TreeNode* node){
    TreeNode* temp=node->right;
    node->right=temp->left;
    if (temp->left){
        temp->left->parent=node;
    }
    temp->parent=node->parent;
    if (node->parent==NULL){
        this->root=temp;
    }
    else if (node==node->parent->left){
        node->parent->left=temp;
    }
    else {
        node->parent->right=temp;
    }

    temp->left=node;
    node->parent=temp;

    return;
}


void SymbolTable::splayNode(TreeNode*node, int& num_splay){
    num_splay++;
    while (node->parent!=NULL){

        if (node->parent==this->root){
            if (node==node->parent->left){
                rightRotate(node->parent);
            }
            else {
                leftRotate(node->parent);
            }
        }
        else {
            TreeNode* par=node->parent;
            TreeNode* gran=par->parent;
            if (node->parent->left==node && par->parent->left==par){
                rightRotate(gran);
                rightRotate(par);
            }
            else if (node->parent->right==node && par->parent->right==par){
                leftRotate(gran);
                leftRotate(par);
            }
            else if (node->parent->right==node && par->parent->left==par){
                leftRotate(par);
                rightRotate(gran);
            }
            else if (node->parent->left == node && par->parent->right == par){
                rightRotate(par);
                leftRotate(gran);
            }
        }
    }
    return;
}


TreeNode::TreeNode(string id, int sco, string ty, string aL){
    this->idName=id;
    this->scope=sco;
    this->type=ty;
    this->argLs=aL;
    this->right=NULL;
    this->left=NULL;
    this->parent=NULL;
    return;
}

SymbolTable::SymbolTable(){
    this->root=NULL;
    this->lsSymbol=new Ls();
    return;
}

void SymbolTable::run(string filename)
{
    ifstream f(filename);
    int curscope=0;
    while (!f.eof()){
        string cmd;
        getline(f,cmd);
        cmd.erase( remove(cmd.begin(), cmd.end(), '\r'), cmd.end() );
        if (regex_match(cmd,regex("INSERT [a-z][a-zA-Z0-9_]* (number|string) (true|false)")))
        {
            int pos[2]={0,0},counter=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                    pos[counter++]=i;
                }
            }
            string name=cmd.substr(7,pos[0]-7);
            if (name=="string" || name=="number" || name=="true" || name=="false") {
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                InvalidInstruction e(cmd);
                throw(e);
            }
            int sco=(cmd[pos[1]+1]=='t')?0:curscope;
            insertElement(cmd,name,cmd.substr(pos[0]+1,pos[1]-pos[0]-1),sco);
        }
        else if (regex_match(cmd,regex("INSERT [a-z][a-zA-Z0-9_]* \\(((number|string)((,number)|(,string))*|)\\)->(number|string) (true|false)")))
        {
            int pos[2]={0,0},counter=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                    pos[counter++]=i;
                }
            }
            int sco=(cmd[pos[1]+1]=='t')?0:curscope;
            string name=cmd.substr(7,pos[0]-7);
            if (name=="string" || name=="number" || name=="true" || name=="false") {
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                InvalidInstruction e(cmd);
                throw(e);
            }
            if (sco!=0) {
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                InvalidDeclaration e(cmd);
                throw(e);
            }
            insertFunction(cmd,name,cmd.substr(pos[0]+1,pos[1]-pos[0]-1),sco);
        }
        else if (regex_match(cmd,regex("ASSIGN [a-z][a-zA-Z0-9_]* ([0-9]+|\'[a-zA-Z0-9 ]*\')"))){
            //Assign a variable with immediate value
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            string name=cmd.substr(7,pos-7);
            if (name=="string" || name=="number" || name=="true" || name=="false") {
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                InvalidInstruction e(cmd);
                throw(e);
            }
            assignImValue(cmd,name,cmd.substr(pos+1,lth-pos-1),curscope);
        }
        else if (regex_match(cmd,regex("ASSIGN [a-z][a-zA-Z0-9_]* [a-z][a-zA-Z0-9_]*")))
        {
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            string name=cmd.substr(7,pos-7);
            if (name=="string" || name=="number" || name=="true" || name=="false") {
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                InvalidInstruction e(cmd);
                throw(e);
            }
            assignWithVariable(cmd,name,cmd.substr(pos+1,lth-pos-1),curscope);
        }
        else if (regex_match(cmd,regex("ASSIGN [a-z][a-zA-Z0-9_]* [a-z][a-zA-Z0-9_]*\\((|(([0-9]+|\'[a-zA-Z0-9 ]*\'|[a-z][a-zA-Z0-9_]*)((,[0-9]+)|(,\'[a-zA-Z0-9 ]*\')|(,[a-z][a-zA-Z0-9_]*))*))\\)"))){
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            string name=cmd.substr(7,pos-7);
            if (name=="string" || name=="number" || name=="true" || name=="false") {
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                InvalidInstruction e(cmd);
                throw(e);
            }
           assignWithFunction(cmd,name,cmd.substr(pos+1,lth-pos-1),curscope);
        }
        else if (regex_match(cmd,regex("LOOKUP [a-z][a-zA-Z0-9_]*"))){
            int temp=0;
            string name=cmd.substr(7);
            if (name=="string" || name=="number" || name=="true" || name=="false") {
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                InvalidInstruction e(cmd);
                throw(e);
            }
            TreeNode *foundnode=lookUp(cmd,name,curscope,temp,temp);
            cout<<foundnode->scope<<endl;
        }
        else if (cmd=="BEGIN"){
            curscope++;
        }
        else if (cmd=="END"){
            curscope--;
            if (curscope<0){
                DestroyRecursive(root);
                lsSymbol->clearLs();
                delete lsSymbol;
                throw (UnknownBlock());
            }
            deleteScopeRecursion(cmd,lsSymbol->head,curscope+1);

        }
        else if (cmd=="PRINT"){
            if (root){
                print(root,root);
                cout<<endl;
            }
        }
        else{
            InvalidInstruction e(cmd);
            DestroyRecursive(root);
            lsSymbol->clearLs();
            delete lsSymbol;
           throw(e);
        }
    }
    if (curscope!=0){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw (UnclosedBlock(curscope));
    }
    lsSymbol->clearLs();
    delete lsSymbol;
    DestroyRecursive(root);
    f.close();
}

void SymbolTable::insertElement(string cmd,string id, string ty, int sco){
    TreeNode* newNode= new TreeNode(id,sco,ty,"");

    if (!this->root) {
        root=newNode;
        newNode->parent=NULL;
        if (sco>0){
            lsSymbol->push(id,sco);
        }
        cout<<0<<" "<<0<<endl;
        return;
    }
    int num_comp=0, num_splay=0;
    TreeNode*cur=root, *pre=NULL;
    while (cur){
        pre=cur;
        num_comp++;
        if (morethan(newNode,cur)){
            cur=cur->right;
        }
        else if (isequal(newNode,cur)){
            delete newNode;
            Redeclared e(cmd);
            DestroyRecursive(root);
            lsSymbol->clearLs();
            delete lsSymbol;
            throw(e);
        }
        else {
            cur=cur->left;
        }
    }

    if (morethan(newNode,pre)){
        pre->right=newNode;
        newNode->parent=pre;
    } else{
        pre->left=newNode;
        newNode->parent=pre;
    }
    if (sco>0){
        lsSymbol->push(id,sco);
    }
    splayNode(newNode, num_splay);
    cout<<num_comp<<" "<<num_splay<<endl;
    return;
}

void SymbolTable::insertFunction(string cmd,string id, string rawargLs, int sco){
    TreeNode* newNode= new TreeNode(id,sco,"function","");
    if (!this->root) {
        int lth=rawargLs.length();
        string ar="";
        for (int i=0;i<lth;i++){
            if (rawargLs[i]=='m'){
                ar+='0';
            }
            else if (rawargLs[i]=='s'){
                ar+='1';
            }
        }
        newNode->argLs=ar;
        newNode->parent=NULL;
        root=newNode;
        if (sco>0){
            lsSymbol->push(id,sco);
        }
        cout<<0<<" "<<0<<endl;
        return;
    }
    int num_comp=0, num_splay=0;
    TreeNode*cur=root, *pre=NULL;
    while (cur){
        pre=cur;
        num_comp++;
        if (morethan(newNode,cur)){
            cur=cur->right;
        }
        else if (isequal(newNode,cur)){
            delete newNode;
            Redeclared e(cmd);
            DestroyRecursive(root);
            lsSymbol->clearLs();
            delete lsSymbol;
            throw(e);
        }
        else {
            cur=cur->left;
        }
    }
    int lth=rawargLs.length();
    string ar="";
    for (int i=0;i<lth;i++){
        if (rawargLs[i]=='m'){
            ar+='0';
        }
        else if (rawargLs[i]=='s'){
            ar+='1';
        }
    }
    newNode->argLs=ar;
    if (morethan(newNode,pre)){
        pre->right=newNode;
        newNode->parent=pre;
    } else{
        pre->left=newNode;
        newNode->parent=pre;
    }
    if (sco>0){
        lsSymbol->push(id,sco);
    }
    splayNode(newNode, num_splay);
    cout<<num_comp<<" "<<num_splay<<endl;
    return;
}

void SymbolTable::assignImValue(string cmd, string id, string value, int sco){
    TreeNode* foundNode=NULL;
    int num_comp=0, num_splay=0;
    while (sco>=0){
        TreeNode* checkNode= new TreeNode(id,sco,"","");
        num_comp=0;
        TreeNode*cur=root;
        while (cur && !isequal(checkNode,cur)){
            num_comp++;
            if (morethan(cur,checkNode)){
                cur=cur->left;
            }
            else {
                cur=cur->right;
            }
        }
        delete checkNode;
        if (cur) {
            foundNode=cur;
            num_comp++;
            break;
        }
        sco--;
    }
    if (!foundNode){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw(Undeclared(cmd));
    }
    if (regex_match(value,regex("[0-9]+"))){
        if (foundNode->type!="number"){
            DestroyRecursive(root);
            lsSymbol->clearLs();
            delete lsSymbol;
            throw(TypeMismatch(cmd));
        }
    }
    else if (regex_match(value,regex("\'[a-zA-Z0-9 ]*\'"))){
        if (foundNode->type!="string"){
            DestroyRecursive(root);
            lsSymbol->clearLs();
            delete lsSymbol;
            throw(TypeMismatch(cmd));
        }
    }
    if (foundNode!=root) {
        splayNode(foundNode, num_splay);
    }
    cout<<num_comp<<" "<<num_splay<<endl;
    return;
}

void SymbolTable::assignWithVariable(string cmd, string id, string variableID, int sco)
{
    int num_comp=0, num_splay=0, cursco=sco;
    TreeNode* variableNode=NULL, *assignedNode=NULL;

    ////Check for the node of the variable
    while (cursco>=0){
        TreeNode* checkNode= new TreeNode(variableID,cursco,"","");
        num_comp=0;
        TreeNode*cur=root;
        while (cur && !isequal(checkNode,cur)){
            num_comp++;
            if (morethan(cur,checkNode)){
                cur=cur->left;
            }
            else {
                cur=cur->right;
            }
        }
        delete checkNode;
        if (cur) {
            variableNode=cur;
            num_comp++;
            break;
        }
        cursco--;
    }
    if (!variableNode){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw(Undeclared(cmd));
    }
    if (variableNode->type=="function"){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw(TypeMismatch(cmd));
    }
    if (variableNode!=root) {
        splayNode(variableNode, num_splay);
    }

    //////////Check for the node of assigned variable
    cursco=sco;
    int temp=num_comp;
    while (cursco>=0){
        TreeNode* checkNode= new TreeNode(id,cursco,"","");
        num_comp=0;
        TreeNode*cur=root;
        while (cur && !isequal(checkNode,cur)){
            num_comp++;
            if (morethan(cur,checkNode)){
                cur=cur->left;
            }
            else {
                cur=cur->right;
            }
        }
        delete checkNode;
        if (cur) {
            assignedNode=cur;
            num_comp++;
            break;
        }
        cursco--;
    }
    if (!assignedNode){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw(Undeclared(cmd));
    }
    if (assignedNode!=root) {
        splayNode(assignedNode, num_splay);
    }
    num_comp+=temp;

    if (variableNode->type!=assignedNode->type){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw(TypeMismatch(cmd));
    }
    cout<<num_comp<<" "<<num_splay<<endl;
    return;

}

void SymbolTable::assignWithFunction(string cmd, string id, string func, int sco){
    int start_arg=0,end_arg=0,lth=func.length();
    int num_comp=0,num_splay=0;
    for (int i=0;i<lth;i++){
        if (func[i]=='(') {
                start_arg=i;
                break;
            }
    }
    string functionName=func.substr(0,start_arg);
    TreeNode* funcNode=lookUp(cmd,functionName,sco,num_comp,num_splay);
    if (funcNode->type!="function") {
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw (TypeMismatch(cmd));
    }
    string argls=funcNode->argLs;
    int     lthargls=argls.length(), index=0, counter=0;
    for (int i=index+1;i<lth;i++){
        if (func[i]==',') {
            counter++;
        }
    }
    if (counter==0){
        if (func[start_arg+1]==')') counter=0;
        else counter=1;
    }
    else counter=counter+1;
    if (counter!=lthargls-1) {
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw (TypeMismatch(cmd));
    }
    end_arg=start_arg+1;
    if (lthargls!=1){
        while (end_arg!=lth){
            if (func[end_arg]==')' || func[end_arg]==','){
                string argstr=func.substr(start_arg+1,end_arg-start_arg-1);
                if (index>=lthargls) {
                    DestroyRecursive(root);
                    lsSymbol->clearLs();
                    delete lsSymbol;
                    throw (TypeMismatch(cmd));
                }
                if (regex_match(argstr,regex("[a-z][a-zA-Z0-9_]*"))){
                    TreeNode* temp=lookUp(cmd,argstr,sco,num_comp,num_splay);
                    if ((temp->type!="number" && argls[index]=='0')
                        || (temp->type!="string" && argls[index]=='1'))
                    {
                        DestroyRecursive(root);
                        lsSymbol->clearLs();
                        delete lsSymbol;
                        throw (TypeMismatch(cmd));
                    }
                }
                else if (regex_match(argstr,regex("[0-9]+"))){
                    if (argls[index]!='0'){
                        DestroyRecursive(root);
                        lsSymbol->clearLs();
                        delete lsSymbol;
                        throw (TypeMismatch(cmd));
                    }
                }
                else if (regex_match(argstr,regex("\'[a-zA-Z0-9 ]*\'"))){
                    if (argls[index]!='1'){
                        DestroyRecursive(root);
                        lsSymbol->clearLs();
                        delete lsSymbol;
                        throw (TypeMismatch(cmd));
                    }
                }
                start_arg=end_arg;
                index++;
            }
            end_arg++;
        }
    }
    if (index!=lthargls-1){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw (TypeMismatch(cmd));
    }
    TreeNode* node=lookUp(cmd,id,sco,num_comp,num_splay);
    if ((argls[lthargls-1]=='0' && node->type!="number") || (argls[lthargls-1]=='1' && node->type!="string") ){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw (TypeMismatch(cmd));
    }
    cout<<num_comp<<" "<<num_splay<<endl;
    return;

}

TreeNode* SymbolTable::lookUp(string cmd,string id, int sco, int& num_comp, int& num_splay){
    TreeNode* foundNode=NULL;
    int num=num_comp;
    ////Check for the node of the variable
    while (sco>=0){
        TreeNode* checkNode= new TreeNode(id,sco,"","");
        num=0;
        TreeNode*cur=root;
        while (cur && !isequal(checkNode,cur)){
            num++;
            if (morethan(cur,checkNode)){
                cur=cur->left;
            }
            else {
                cur=cur->right;
            }
        }
        delete checkNode;
        if (cur) {
            foundNode=cur;
            num++;
            break;
        }
        sco--;
    }
    if (!foundNode){
        DestroyRecursive(root);
        lsSymbol->clearLs();
        delete lsSymbol;
        throw(Undeclared(cmd));
    }
    if (foundNode!=root) {
        splayNode(foundNode, num_splay);


    }

    num_comp+=num;
    return foundNode;
}


void SymbolTable::deleteNode(TreeNode* node){
    int t=0;
    splayNode(node,t);
    SymbolTable left_subtree= SymbolTable();

    left_subtree.root=this->root->left;
    if (left_subtree.root!=NULL){
        left_subtree.root->parent=NULL;
    }

    SymbolTable right_subtree=  SymbolTable();
    right_subtree.root=this->root->right;
    if (right_subtree.root!=NULL){
        right_subtree.root->parent=NULL;
    }

    delete node;
    this->root=NULL;

    if (left_subtree.root!=NULL){
        if (!right_subtree.root){
            this->root=left_subtree.root;

        }
        else{
            TreeNode* maxNode=left_subtree.root;
            while (maxNode->right){
                maxNode=maxNode->right;
            }
            int t=0;
            left_subtree.splayNode(maxNode,t);
            left_subtree.root->right=right_subtree.root;
            right_subtree.root->parent=left_subtree.root;
            this->root=left_subtree.root;
        }
    }
    else {
        this->root=right_subtree.root;
    }
    delete left_subtree.lsSymbol;
    delete right_subtree.lsSymbol;

    return;
}


void SymbolTable::deleteScopeRecursion(string cmd,Node* node, int sco){
    if (!node || node->scope!=sco) return;
    string id=node->idName;
    deleteScopeRecursion(cmd,node->next,sco);
    lsSymbol->pop();
    int t=0;
    TreeNode* temp = lookUp(cmd,id,sco,t,t);
    deleteNode(temp);
    return;
}

void print(TreeNode* node, TreeNode* root){
    if (!node) {
        return;
    }
    if (node==root) cout<<node->idName<<"//"<<node->scope;
    else cout<<" "<<node->idName<<"//"<<node->scope;
    print(node->left, root);
    print(node->right, root);
    return;

}

