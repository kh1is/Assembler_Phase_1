#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <cstring>
#include <numeric>
#include <cmath>
#include <iomanip>
using namespace std;

// Assumption 1 : no label with reserved words can be used
// Assumption 2 : comments in one separate line
// Assumption 3 : one error per line
// Assumption 4 : Program name must be found
// Assumption 5 : No 'ORG' or 'USE' can be used


int readFile(string path);
void lineParser(char str[255]);
void buildStruct(string result[] , int index);
bool searchOpTab(string s);
void checkErrors();
void labelCheck();
void opCodeCheck();
void calculateDisplacement();
int stringToInt(string s);
void calculateAddresse();
int charToValue(char c);
void printTable();
void printSympolTable();

struct line {
   int  lineNum;
   int address = 0;
   string  label = "....";
   string opCode = "....";
   string operand = "....";
   string comment = "....";
   int format = 0 ;
   string error = "....";
};
string opTab[] = {"ADD" ,"ADDR" ,"AND","CLEAR" , "COMP" ,"COMPR","DIV" ,"DIVR",
    "EQU" , "J", "JEQ" ,"JGT" ,"JLT" , "JSUB" , "LDA" ,"LDB" , "LDCH", "LDL" ,"LDS" ,
     "LDT" ,"LDX","LTORG" ,"MUL" , "MULR" ,"ORG" ,"RD" ,"RMO" ,"RSUB" ,"SHIFTL", "SHIFTR",
      "STA","STB" ,"STCH" , "STL" ,"STS" , "STT", "STX" ,"SUB" , "SUBR", "TD" ,"TIX" , "TIXR","WD","BYTE",
      "WORD","RESW","RESB", "START" , "END"};
string registers[] = {"A" ,"S" ,"T","PC" , "B" ,"X","L" ,"F", "SW"};
std::vector<line> table;
int lineNum = 1;
int locationCounter = 0 ;

int main()
{

    cout << "Please enter the target path :" << endl;

    string path ;

    cin >> path;

    while (readFile(path) == -1){

        cout << "Cannot open input file.\n";

        cout << "Please enter the target path :" << endl;

        cin >> path;

    }

    int work = 1;

    while ( work != -1){

        checkErrors();

        calculateDisplacement();

        calculateAddresse();

        cout << "Please Choose what to print and enter the number:" << endl;
        cout << "-------------------------------------------------" << endl;
        cout << "1-Print all the table" << endl;
        cout << "2-Print the symbolic table" << endl;
        int x ;
        cin >> x;
        cout << "-------------------------------------------------" << endl;

        if(x==1){
           printTable();
           cout << "-------------------------------------------------" << endl;
           cout << "-------------------------------------------------" << endl;
        }else if ( x ==2 ){
            printSympolTable();
            cout << "-------------------------------------------------" << endl;
            cout << "-------------------------------------------------" << endl;
        }else{
            cout << "Please try again!!!" << endl;
            cout << "-------------------------------------------------" << endl;
            cout << "-------------------------------------------------" << endl;
        }

        cout << "Enter '-1' to end" << endl;
        cin >> work;
        cout << "-------------------------------------------------" << endl;

    }

    return 0;
}

int readFile(string path){

    // Get File
    ifstream in(path);

    // Check File is found
    if(!in) {
        return -1;
    }

    // Line range
    char str[255];

    // Read line line
    while(in) {
        in.getline(str, 255);  // delim defaults to '\n'
        if(!in){
           break;
        }
        lineParser(str); // Parse Line
        lineNum++;
    }

    // Close file
    in.close();

    return 1;
}

void lineParser(char line[255]){

    string s = line ;
    //vector<string> result;
    string result[20];
    int index = 0 ;
    std::istringstream iss(s);
    for(string s; iss >> s; ){
        //result.push_back(s);
        result[index] = s;
        index++;
    }

    buildStruct(result,index);

}


void buildStruct(string result[] , int index){

    // Comment Lines
    if(result[0].at(0) == '.'){
        line s ;
        vector<string> temp;
        for(int i = 0 ; i < index ; i++){
            temp.push_back(result[i]);
            temp.push_back(" ");
        }
        s.comment = accumulate(begin(temp), end(temp), s.comment);
        s.lineNum = lineNum;
        table.push_back(s);
    }else{
        if(result[1] == "START"){
            line s ;
            if(index < 3){
                s.error = "   ** Error : Start Statement Syntax **   ";
            }
            s.label = result[0];
            s.opCode = result[1];
            s.operand = result[2];
            s.lineNum = lineNum;
            table.push_back(s);
        }else if (result[0] == "END"){
           line s ;
           s.opCode = result[0];
           // To Do : Handel no operand must be found
           if(index < 2){
                s.error = "   ** Error : End Statement Syntax **   ";
            }
           s.operand = result[1];
           s.lineNum = lineNum;
           table.push_back(s);
        }else{
            line s ;
            // Check if no label
            if(searchOpTab(result[0])){
                s.opCode = result[0];
                if( 1 < index){
                  s.operand = result[1];
                }
                s.lineNum = lineNum;
                table.push_back(s);
            }else{
              s.label = result[0];
              s.opCode = result[1];
              if( 2 < index){
                s.operand = result[2];
              }
              s.lineNum = lineNum;
              table.push_back(s);
            }
        }
    }
}

bool searchOpTab(string s){
    for(int i = 0 ; i < sizeof(opTab)/ sizeof(opTab[0]) ; i++){
        if(s == opTab[i]){
            return true;
        }
    }
    return false;
}

void checkErrors(){
    labelCheck();
    opCodeCheck();
}

void labelCheck(){

    for(int i = 0; i< lineNum-1 ; i++){
       string temp = table[i].label;
       for(int j = i+1 ; j < lineNum-1 ; j++){
            if(temp == table[j].label && temp != "...."){
                table[j].error = "   ** Error : Symbol already defined **   ";
            }
        }
    }
}

void opCodeCheck(){
    for(int i = 0 ; i < lineNum-1 ; i++){
        if(!searchOpTab(table[i].opCode) && table[i].opCode != "...."){
            table[i].error = "   ** Error : Incorrect Operation **   ";
        }
    }
}

void calculateDisplacement(){
    for(int i = 0 ; i < lineNum-2 ; i++){
        if(table[i].opCode == "...."){
            table[i].format = 0;
        }else{
            if(table[i].opCode == "START"){
                table[i].format = 0;
                locationCounter = stringToInt(table[i].operand);
                table[i].address = locationCounter;
            }else if(table[i].opCode == "BASE" || table[i].opCode == "NOBASE" || table[i].opCode == "EQU"){
                table[i].format = 0;
            }else if(table[i].opCode == "WORD"){
                table[i].format = 3;
            }else if(table[i].opCode == "RESW"){
                table[i].format = 3 * stringToInt(table[i].operand);
            }else if(table[i].opCode == "BYTE"){
                if(table[i].operand.at(0) == '='){
                    table[i].format = table[i].operand.length()-4;
                    if(table[i].operand.at(1) == 'X'){
                        table[i].format = ceil(table[i].format/2);
                    }
                }else{
                    table[i].format = table[i].operand.length()-3;
                    if(table[i].operand.at(0) == 'X'){
                        table[i].format = ceil(table[i].format/2);
                    }
                }
            }else if(table[i].opCode == "RESB"){
                table[i].format = stringToInt(table[i].operand);
            }else{
                if(table[i].opCode.at(0) == '+'){
                    table[i].format = 4;
                }else if(table[i].opCode.at(table[i].opCode.length()-1) == 'R' || table[i].opCode == "RMO"){
                    table[i].format = 2;
                }else{
                    table[i].format = 3;
                }
            }
        }
    }
}

int stringToInt(string s){
    int x = 0;
    for(int i=0;i<s.size();i++){
        x = x + (charToValue(s.at(i))) * pow(16.0,(double)s.size()-1-i);
    }
    return x;
}

int charToValue(char c){
    if(c == 'A'){
        return 10;
    }else if(c == 'B'){
        return 11;
    }else if(c == 'C'){
        return 12;
    }else if(c == 'D'){
        return 13;
    }else if(c == 'E'){
        return 14;
    }else if(c == 'F'){
        return 15;
    }else{
        return c - '0';
    }
}

void calculateAddresse(){
    for(int i = 0 ; i < lineNum - 1 ; i++){
        table[i].address = locationCounter;
        locationCounter = locationCounter + table[i].format;
    }
}

void printTable(){
    cout << "Line NO." << "\t" << "Address" << "\t\t" << "Label" << "\t\t" << "OP-Code" << "\t\t" << "Operand" <<
       "\t\t" << "Comments" << "\t" << "Format" << endl;

    for(int i = 0 ; i < table.size() ; i++){
       cout << table[i].lineNum << "\t\t" << hex << table[i].address << "\t\t"  << table[i].label << "\t\t" << table[i].opCode << "\t\t" << table[i].operand <<
       "\t\t" << table[i].comment << "\t\t" << table[i].format <<endl;
       if(table[i].error != "...."){
        cout << table[i].error << endl;
       }
    }
}

void printSympolTable(){

    cout << "address" << "\t\t" << "label" << endl;
    cout << endl;
    for(int i=0;i<lineNum-1;i++){

        if(table[i].label != "...."){

            cout << hex << table[i].address << "\t\t" << table[i].label << endl;
        }

    }

}
