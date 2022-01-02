#include "SymbolTable.h"
#include "SymbolTable.cpp"
using namespace std;


void test(string filename)
{
    SymbolTable *st = new SymbolTable();
    try
    {
        st->run(filename);
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
    }
    delete st;
}

string templateStr(int index) {
    return "testcase/test" + to_string(index) + ".txt";
}

void exportExpected(int start, int end) {
    // redirect cout to file
    ofstream out("expected.txt");
    auto coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    for(int i = start; i <= end; i++) {
        string textFile = templateStr(i);
        cout << "Test " << i << ": Start\n";
        test(textFile);
        cout << "Test " << i << ": End\n";
    }

    cout.rdbuf(coutbuf);
}

void checkAnswer(int start, int end) {
    // redirect cout to file
    ofstream out("got.txt");
    auto coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    ifstream expected("expected.txt");
    ifstream answer("got.txt");
    ofstream summary("summary.txt");

    for(int i = start; i <= end; i++) {
        string openAns = "Test " + to_string(i) + ": Start";
        string endAns = "Test " + to_string(i) + ": End";
        string textFile = templateStr(i);

        cout << openAns << endl;
        test(textFile);
        cout << endAns << endl;

        string resStr, ansStr;
        getline(expected, resStr);
        getline(answer, ansStr);

        // summary << resStr << ' ' << ansStr << endl;
        while(resStr != openAns && !expected.eof()) {
            getline(expected, resStr);
            // summary << resStr << ' ' << openAns << endl;
        }
        while(ansStr != openAns && !answer.eof()) {
            getline(answer, ansStr);
            // summary << resStr << ' ' << ansStr << endl;
        }
        if(expected.eof()) {
            summary << "Test " << i << ": Cannot find expected !!!\n";
            break;
        }

        bool isPassed = true;
        while(!answer.eof()) {
            // summary << resStr << ' ' << ansStr << endl;
            if(resStr != ansStr) {
                isPassed = false;
                break;
            }
            if(resStr == endAns)
                break;
            getline(expected, resStr);
            getline(answer, ansStr);
        }
        if(isPassed)
            summary << "Test " << i << ": Passed" << endl;
        else
            summary << "Test " << i << ": Failed" << endl;
    }

    cout.rdbuf(coutbuf);
}

int main() {
    //exportExpected(1,60);
    checkAnswer(1,64);
    cout << "Pass!!!" << endl;
}
