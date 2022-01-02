#include "SymbolTable.h"

void SymbolTable::run(string filename)
{

    ifstream f(filename);
    int curscope=0;
    if (!f.eof()){
        string cmd;
        getline(f,cmd);
        if (regex_match(cmd,regex("LINEAR [0-9]+ [0-9]+"))){
            hf=hashLinear;
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            m=stoi(cmd.substr(7,pos-7));
            data=new Data[m];
            c1=stoi(cmd.substr(pos+1,lth-pos-1));

        }
        else if (regex_match(cmd,regex("QUADRATIC [0-9]+ [0-9]+ [0-9]+"))){
            hf=hashQuadratic;
            int pos[2]={0,0},counter=0,lth=cmd.length();
            for (int i=10;i<lth;i++){
                if (cmd[i]==' ') {
                    pos[counter++]=i;
                }
            }
            m=stoi(cmd.substr(10,pos[0]-10));
            data=new Data[m];
            c1=stoi(cmd.substr(pos[0]+1,pos[1]-pos[0]-1));
            c2=stoi(cmd.substr(pos[1]+1,lth-pos[1]-1));

        }
        else if (regex_match(cmd,regex("DOUBLE [0-9]+ [0-9]+"))){
            hf=hashDouble;
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            m=stoi(cmd.substr(7,pos-7));
            data=new Data[m];
            c1=stoi(cmd.substr(pos+1,lth-pos-1));
        }
        else {
            throw(InvalidInstruction(cmd));
        }
    }
    while (!f.eof()){
        string cmd;
        getline(f,cmd);
        if (regex_match(cmd,regex("INSERT [a-z][a-zA-Z0-9_]*")))
        {
            string id=cmd.substr(7,cmd.length()-7);
            if (id=="string" || id=="number" || id=="void") {
                throw(InvalidInstruction(cmd));
            }
            insertElement(cmd,id,curscope);
        }
        else if (regex_match(cmd,regex("INSERT [a-z][a-zA-Z0-9_]* [0-9]+")))
        {
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            string id=cmd.substr(7,pos-7);
            if (id=="string" || id=="number" || id=="void") {
                throw(InvalidInstruction(cmd));
            }
            if (curscope!=0) {
                throw(InvalidDeclaration(id));
            }
            insertFunction(cmd, id,stoi(cmd.substr(pos+1,lth-pos-1)));
        }
        else if (regex_match(cmd,regex("ASSIGN [a-z][a-zA-Z0-9_]* ([0-9]+|\'[a-zA-Z0-9 ]*\')")))
        {
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            string id=cmd.substr(7,pos-7);
            if (id=="string" || id=="number" || id=="void") {
                throw(InvalidInstruction(cmd));
            }
            assignImValue(cmd,id,cmd.substr(pos+1,lth-pos-1),curscope);

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
            string id=cmd.substr(7,pos-7),var=cmd.substr(pos+1,lth-pos-1);
            if (id=="string" || id=="number" || var=="string" || var=="number" || id=="void" || var=="void") {
                throw(InvalidInstruction(cmd));
            }
            assignWithVariable(cmd,id,var,curscope);

        }
        else if (regex_match(cmd,regex("ASSIGN [a-z][a-zA-Z0-9_]* [a-z][a-zA-Z0-9_]*\\((|(([0-9]+|\'[a-zA-Z0-9 ]*\'|[a-z][a-zA-Z0-9_]*)((,[0-9]+)|(,\'[a-zA-Z0-9 ]*\')|(,[a-z][a-zA-Z0-9_]*))*))\\)"))){
            int pos=0,lth=cmd.length();
            for (int i=7;i<lth;i++){
                if (cmd[i]==' ') {
                        pos=i;
                        break;
                }
            }
            assignWithFunction(cmd,cmd.substr(7,pos-7),cmd.substr(pos+1,lth-pos-1),curscope);
        }
        else if (regex_match(cmd,regex("LOOKUP [a-z][a-zA-Z0-9_]*")))
        {
            int lth=cmd.length(), temp;
            string id=cmd.substr(7,lth-7);
            if (id=="string" || id=="number" || id=="void") {
                throw(InvalidInstruction(cmd));
            }
            Data* res=lookUp(cmd,id,curscope,temp);
            cout<<(int)(res-data)<<endl;
        }
        else if (regex_match(cmd,regex("CALL [a-z][a-zA-Z0-9_]*\\((|(([0-9]+|\'[a-zA-Z0-9 ]*\'|[a-z][a-zA-Z0-9_]*)((,[0-9]+)|(,\'[a-zA-Z0-9 ]*\')|(,[a-z][a-zA-Z0-9_]*))*))\\)")))
        {
            callFunction(cmd,cmd.substr(5,cmd.length()-5),curscope);
        }
        else if (regex_match(cmd,regex("PRINT")))
        {
            print();
        }
        else if (cmd=="BEGIN"){
            curscope++;
        }
        else if (cmd=="END"){
            if (curscope-1<0){
                throw(UnknownBlock());
            }
            deleteScope(cmd,curscope);
            curscope--;
        }
        else {
            throw(InvalidInstruction(cmd));
        }

    }
    if (curscope!=0){
        throw(UnclosedBlock(curscope));
    }
    f.close();
}

void SymbolTable::insertElement(string cmd,string id,int curscope){
    string key = encodeKey(id,curscope);
    int i=0;
    for (;i<m;i++){
        int posHash=hf(key,i,m,c1,c2);
        if (data[posHash].idName==id && data[posHash].scope==curscope){
            throw(Redeclared(id));
        }
        if (data[posHash].scope<0){
            nodeL.push(posHash);
            data[posHash].idName=id;
            data[posHash].scope=curscope;
            break;
        }
    }
    if (i==m) {
        throw(Overflow(cmd));
    }
    cout<<i<<endl;
    size++;
    return;
}

void SymbolTable::insertFunction(string cmd, string id, int numberArg){
    string key = encodeKey(id,0);
    int i=0;
    for (;i<m;i++){
        int posHash=hf(key,i,m,c1,c2);
        if (data[posHash].idName==id && data[posHash].scope==0){
            throw(Redeclared(id));
        }
        if (data[posHash].scope<0){
            data[posHash].idName=id;
            data[posHash].scope=0;
            data[posHash].argL=string(numberArg+1,'N');
            data[posHash].type="function";
            break;
        }
    }
    if (i==m) {
        throw(Overflow(cmd));
    }

    cout<<i<<endl;
    size++;
    return;



}

void SymbolTable::assignImValue(string cmd,string id,string val,int curscope){
    int numberPass=0;
    Data* foundElement=lookUp(cmd,id,curscope,numberPass);
    string valtype=(regex_match(val,regex("[0-9]+")))?"number":"string";
    if (foundElement->type==""){
        foundElement->type= valtype;
        cout<<numberPass<<endl;
        return;
    }
    if (foundElement->type!=valtype) {
        throw(TypeMismatch(cmd));
    }
    return;
}

void SymbolTable::assignWithVariable(string cmd, string id, string variable, int curscope){
    int numberPass=0;
    Data* foundVariable=lookUp(cmd,variable,curscope,numberPass);
    if (foundVariable->type=="function" ){
        throw(InvalidInstruction(cmd));
    }
    Data* foundID=lookUp(cmd,id,curscope,numberPass);
    if (foundVariable->type=="" && foundID->type==""){
        throw (TypeCannotBeInferred(cmd));
    }
    else if (foundID->type=="function"){
        throw(TypeMismatch(cmd));
    }
    else if (foundVariable->type!="" && foundID->type==""){
        foundID->type=foundVariable->type;
    }
    else if (foundVariable->type=="" && foundID->type!=""){
        foundVariable->type=foundID->type;
    }
    else if (foundVariable->type!=foundID->type){
        throw(TypeMismatch(cmd));
    }
    cout<<numberPass<<endl;
    return;
}


void SymbolTable::assignWithFunction(string cmd, string id, string func, int curscope){
    int start_arg=0,end_arg=0,lth=func.length();
    int num_pass=0;
    for (int i=0;i<lth;i++){
        if (func[i]=='(') {
                start_arg=i;
                break;
            }
    }
    string functionName=func.substr(0,start_arg);
    Data *funcElement=lookUp(cmd,functionName,curscope,num_pass);
    string argls=funcElement->argL;
    int     lthargls=funcElement->argL.length(), index=0, counter=0;
    if (funcElement->type!="function" ){
        throw(TypeMismatch(cmd));
    }


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
        throw (TypeMismatch(cmd));
    }
    end_arg=start_arg+1;

    if (lthargls!=1){
        while (end_arg!=lth){
            if (func[end_arg]==')' || func[end_arg]==','){
                string argstr=func.substr(start_arg+1,end_arg-start_arg-1);
                if (index>=lthargls) {
                    throw (TypeMismatch(cmd));
                }
                if (regex_match(argstr,regex("[a-z][a-zA-Z0-9_]*"))){
                    Data* temp=lookUp(cmd,argstr,curscope,num_pass);
                    if (temp->type=="" && argls[index]=='N'){
                        throw(TypeCannotBeInferred(cmd));
                    }
                    else if (temp->type=="function"){
                        throw (TypeMismatch(cmd));
                    }
                    else if (temp->type!="" && argls[index]=='N'){
                        funcElement->argL[index]=(temp->type=="number")?'0':'1';
                    }
                    else if (temp->type=="" && argls[index]!='N'){
                        temp->type=(argls[index]=='0')?"number":"string";
                    }
                    else if ((temp->type=="number" && argls[index]=='1') || (temp->type=="string" && argls[index]=='0') ){
                        throw(TypeMismatch(cmd));
                    }
                }
                else if (regex_match(argstr,regex("[0-9]+"))){
                    if (argls[index]=='N') {
                        funcElement->argL[index]='0';
                    }
                    else if (argls[index]=='1'){
                        throw(TypeMismatch(cmd));
                    }
                }
                else if (regex_match(argstr,regex("\'[a-zA-Z0-9 ]*\'"))){
                    if (argls[index]=='N') {
                        funcElement->argL[index]='1';
                    }
                    else if (argls[index]=='0'){
                        throw(TypeMismatch(cmd));
                    }
                }
                start_arg=end_arg;
                index++;
            }
            end_arg++;
        }
    }
    if (index!=lthargls-1){
        throw (TypeMismatch(cmd));
    }
    if (funcElement->argL[lthargls-1]=='2'){
        throw (TypeMismatch(cmd));
    }
    Data *foundID=lookUp(cmd,id,curscope,num_pass);

    if (foundID->type=="function"){
        throw (TypeMismatch(cmd));
    }
    if (argls[lthargls-1]=='N' && foundID->type==""){
        throw(TypeCannotBeInferred(cmd));
    }
    else if ((argls[lthargls-1]=='0' && foundID->type=="string")||(argls[lthargls-1]=='1' && foundID->type=="number")){
        throw(TypeMismatch(cmd));
    }
    else if (argls[lthargls-1]=='N'){
        funcElement->argL[lthargls-1]=(foundID->type=="string")?'1':'0';
    }
    else if (foundID->type==""){
        foundID->type=(argls[lthargls-1]=='0')?"number":"string";
    }

    cout<<num_pass<<endl;
    return;
}


void SymbolTable::callFunction(string cmd,string func, int curscope){
    int start_arg=0,end_arg=0,lth=func.length();
    int num_pass=0;
    for (int i=0;i<lth;i++){
        if (func[i]=='(') {
                start_arg=i;
                break;
            }
    }
    string functionName=func.substr(0,start_arg);
    Data *funcElement=lookUp(cmd,functionName,curscope,num_pass);
    string argls=funcElement->argL;
    int     lthargls=funcElement->argL.length(), index=0, counter=0;
    if (funcElement->type!="function"){
        throw(TypeMismatch(cmd));
    }
    if (funcElement->argL[lthargls-1]=='N'){
        funcElement->argL[lthargls-1]='2';
    }



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
        throw (TypeMismatch(cmd));
    }
    end_arg=start_arg+1;

    if (lthargls!=1){
        while (end_arg!=lth){
            if (func[end_arg]==')' || func[end_arg]==','){
                string argstr=func.substr(start_arg+1,end_arg-start_arg-1);
                if (index>=lthargls) {
                    throw (TypeMismatch(cmd));
                }
                if (regex_match(argstr,regex("[a-z][a-zA-Z0-9_]*"))){
                    Data* temp=lookUp(cmd,argstr,curscope,num_pass);
                    if (temp->type=="" && argls[index]=='N'){
                        throw(TypeCannotBeInferred(cmd));
                    }
                    else if (temp->type=="function"){
                        throw (TypeMismatch(cmd));
                    }
                    else if (temp->type!="" && argls[index]=='N'){
                        funcElement->argL[index]=(temp->type=="number")?'0':'1';
                    }
                    else if (temp->type=="" && argls[index]!='N'){
                        temp->type=(argls[index]=='0')?"number":"string";
                    }
                    else if ((temp->type=="number" && argls[index]=='1') || (temp->type=="string" && argls[index]=='0') ){
                        throw(TypeMismatch(cmd));
                    }
                }
                else if (regex_match(argstr,regex("[0-9]+"))){
                    if (argls[index]=='N') {
                        funcElement->argL[index]='0';
                    }
                    else if (argls[index]=='1'){
                        throw(TypeMismatch(cmd));
                    }
                }
                else if (regex_match(argstr,regex("\'[a-zA-Z0-9 ]*\'"))){
                    if (argls[index]=='N') {
                        funcElement->argL[index]='1';
                    }
                    else if (argls[index]=='0'){
                        throw(TypeMismatch(cmd));
                    }
                }
                start_arg=end_arg;
                index++;
            }
            end_arg++;
        }
    }
    if (index!=lthargls-1){
        throw (TypeMismatch(cmd));
    }
    if ( funcElement->argL[lthargls-1]!='2'){
        throw(TypeMismatch(cmd));
    }
    cout<<num_pass<<endl;
    return;
}

void SymbolTable::deleteScope(string cmd, int scope){
   Node* temp=nodeL.head;
   while (temp && data[temp->pos].scope==scope){
        data[temp->pos].idName="";
        data[temp->pos].type="";
        data[temp->pos].scope=-1;
        size--;
        temp=temp->next;
        nodeL.pop();
   }
   return;
}

Data* SymbolTable::lookUp(string cmd,string id, int curscope, int& numberPass){
    int counter;
    while (curscope>-1){
        string key=encodeKey(id,curscope);
        int i=0;
        counter=0;
        while (i<m){
            int posHash=hf(key,i,m,c1,c2);
            if (data[posHash].scope==-2) break;
            else if (data[posHash].idName==id && data[posHash].scope==curscope){
                numberPass+=counter;
                return (data+posHash);
            }
            counter+=1;
            i++;
        }
        curscope--;
    }
    throw (Undeclared(id));
    return NULL;
}

void SymbolTable::print(){
    int counter=0;
    for (int i=0;i<m;i++){
        if (data[i].scope>=0 && counter<size-1){
            counter++;
            cout<<i<<" "<<data[i].idName<<"//"<<data[i].scope<<";";
        }
        else if (data[i].scope>=0){
            cout<<i<<" "<<data[i].idName<<"//"<<data[i].scope<<endl;
        }
    }
    return;
}
