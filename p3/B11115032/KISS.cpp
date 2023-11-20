#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//state minimization


template <typename T>
void push_back(T*& array, int& size, T value);

class state{
public:
    string name="";
    int inputAmount=0;
    int termsAmount=0;
    string* inputs = new string[1<<inputAmount];
    string* outputs = new string[1<<inputAmount];
    string* nexts = new string[1<<inputAmount];
    bool exist=true;
    
    state(string name = "", int inputAmount=1)
        :name(name), inputAmount(inputAmount)
    {
        this->termsAmount=1<<inputAmount;
        this->inputs=new string[termsAmount];
        this->outputs=new string[termsAmount];
        this->nexts=new string[termsAmount];
    }
};

class impTable{
public:
    struct cell{
        int termsAmount=0;
        bool incompatible=false;
        string row="", column="";
        string* eachNexts = nullptr;
    };
    
    int stateAmount=0;
    state* states=nullptr;
    cell** table=nullptr;
    
    impTable(state* states, int stateAmount)
        :states(states), stateAmount(stateAmount)
    {
        this->init_table();
    }
    
    void init_table();
    int getStateIndex(string name);
    bool sameOuputs(cell c);
    void fillEachNexts(cell &c);
    string getRow(string eachNext, bool col=false);
    void setStateIncompatible(int stateIdx);
    void changeStateName(string oldName, string newName);
    bool findIncompatible();
    bool findMerge();
};

string readFile(string filename, state*& states, int& stateAmount);
void toDotFile(string filename, state* states, int stateAmount, string resetState);

int main()
{
    string filename="kiss1.kiss";
    state* states=new state[0];
    int stateAmount=0;
    string resetState=readFile(filename, states, stateAmount);
    
    // cout<<"inputAmount: "<<states[0].inputAmount<<endl;
    // cout<<"stateAmount: "<<stateAmount<<endl;
    
    // for(int i=0;i<stateAmount;i++)
    // {
    //     cout<<states[i].name<<" | ";
    //     for(int j=0;j<states[i].termsAmount;j++)
    //     {
    //         cout<<states[i].nexts[j]<<" "<<states[i].outputs[j]<<" | ";
    //     }
    //     cout<<endl;
    // }
    
    impTable *table=new impTable(states, stateAmount);
    bool changed=false;
    bool merged=false;
    do{
        do{
            changed=table->findIncompatible();
        }while(changed);
        merged=table->findMerge();
    }while(merged);
    
    cout<<"-------------------merged-------------------"<<endl;
    for(int i=0;i<stateAmount;i++)
    {
        if(states[i].exist)
        {
            cout<<states[i].name<<" | ";
            for(int j=0;j<states[i].termsAmount;j++)
            {
                cout<<states[i].nexts[j]<<" "<<states[i].outputs[j]<<" | ";
            }
            cout<<endl;
        }
    }
    
    int resetStateIdx=table->getStateIndex(resetState);
    while(!states[resetStateIdx].exist)
        resetStateIdx++;
        
    toDotFile(filename+".dot", states, stateAmount, resetState);
}

template <typename T>
void push_back(T*& array, int& size, T value)
{
    T* temp=new T[size+1];
    for(int i=0;i<size;i++)
    {
        temp[i]=array[i];
    }
    temp[size]=value;
    delete[] array;
    array=temp;
    size++;
}

string readFile(string filename, state*& states, int& stateAmount)
{
    ifstream file(filename, ios::in);
    int inputAmount=0, outputAmount=0, termsAmount=0;
    string resetState="";
    char point='.';
    string prefix="";
    while(file>>point)
    {
        if(point=='.')
        {
            int t=0;
            file>>prefix;
            if(prefix=="i") file>>inputAmount;
            if(prefix=="o") file>>outputAmount;
            if(prefix=="p") file>>termsAmount;
            if(prefix=="s") file>>t;
            if(prefix=="r") file>>resetState;
            if(prefix=="end_kiss") break;
        }
        else
        {
            state* st;
            for(int i=0;i<1<<inputAmount;i++)
            {
                string input="", name="", nextname="", output="";
                if(i==0)
                {
                    if(inputAmount==1)
                    {
                        input=point;
                        file>>name>>nextname>>output;
                    }
                    else
                    {
                        file>>input>>name>>nextname>>output;
                        input=point+input;
                    }
                    st=new state(name, inputAmount);
                }
                else{
                    file>>input>>name>>nextname>>output;
                }
                st->inputs[i]=input;
                st->outputs[i]=output;
                st->nexts[i]=nextname;
            }
            push_back(states, stateAmount, *st);
        }
    }
    file.close();
    return resetState;
}

void toDotFile(string filename, state* states, int stateAmount, string resetState)
{
    ofstream file(filename, ios::out);
    file<<"digraph STG {"<<endl<<"rankdir=LR;"<<endl<<"INIT[shape=point];"<<endl;
    for(int i=0;i<stateAmount;i++)
    {
        if(!states[i].exist) continue;
        file<<states[i].name<<"[label=\""<<states[i].name<<"\"];"<<endl;
    }
    file<<"INIT->"<<resetState<<";"<<endl;
    for(int i=0;i<stateAmount;i++)
    {
        if(!states[i].exist) continue;
        for(int j=0;j<states[i].termsAmount;j++)
        {
            file<<states[i].name<<"->"<<states[i].nexts[j]<<"[label=\""<<states[i].inputs[j]<<"/"<<states[i].outputs[j]<<"\"];"<<endl;
        }
    }
    file<<"}"<<endl;
    
    string command="dot -T png "+filename+" > "+filename+".png";
    system(command.c_str());
}

void impTable::init_table()
{
    table = new cell*[stateAmount];
    for(int i=0;i<stateAmount;i++)
    {
        table[i] = new cell[i];
        for(int j=0;j<i;j++)
        {
            table[i][j].termsAmount=states[0].termsAmount;
            table[i][j].row=states[i].name;
            table[i][j].column=states[j].name;
            table[i][j].eachNexts=new string[table[i][j].termsAmount];
            if(!sameOuputs(table[i][j])) table[i][j].incompatible=true;
            else fillEachNexts(table[i][j]);
        }
    }
}

int impTable::getStateIndex(string name)
{
    for(int i=0;i<stateAmount;i++) if(states[i].name==name) return i;
    return -1;
}

bool impTable::sameOuputs(cell c)
{
    state s1=states[getStateIndex(c.row)];
    state s2=states[getStateIndex(c.column)];
    for(int i=0;i<s1.termsAmount;i++)
    {
        if(s1.outputs[i]!=s2.outputs[i]) return false;
    }
    return true;
}

void impTable::fillEachNexts(cell &c)
{
    for(int i=0;i<c.termsAmount;i++)
    {
        state rowState=this->states[getStateIndex(c.row)];
        state columnState=this->states[getStateIndex(c.column)];
        string each=rowState.nexts[i];
        if(getStateIndex(rowState.nexts[i])>=getStateIndex(columnState.nexts[i])) 
            each+=" "+columnState.nexts[i];
        else 
            each=columnState.nexts[i]+" "+each;
        c.eachNexts[i]=each;
    }
}

string impTable::getRow(string eachNext, bool isCol)
{
    string row="", col="";
    row=eachNext.substr(0, eachNext.find(" "));
    col=eachNext.substr(eachNext.find(" ")+1);
    if(isCol) return col;
    else return row;
}

bool impTable::findIncompatible()
{
    bool changed=false;
    for(int i=0;i<stateAmount;i++)
    {
        for(int j=0;j<i;j++)
        {
            if(table[i][j].incompatible) continue;
            for(int k=0;k<table[i][j].termsAmount;k++)
            {
                string eachNext=table[i][j].eachNexts[k];
                string row=getRow(eachNext);
                string col=getRow(eachNext, true);
                int rowIdx=getStateIndex(row);
                int colIdx=getStateIndex(col);
                if(rowIdx==colIdx) continue;
                if(table[rowIdx][colIdx].incompatible)
                {
                    table[i][j].incompatible=true;
                    changed=true;
                    break;
                }
            }
        }
    }
    return changed;
}

bool impTable::findMerge()
{
    for(int i=0;i<stateAmount;i++)
    {
        for(int j=0;j<i;j++)
        {
            if(table[i][j].incompatible) continue;
            this->states[i].exist=false;
            this->setStateIncompatible(i);
            this->changeStateName(this->states[i].name, this->states[j].name);
            table[i][j].incompatible=true;
            return true;
        }
    }
    return false;
}

void impTable::setStateIncompatible(int stateIdx)
{
    for(int i=0;i<stateAmount;i++)
        for(int j=0;j<i;j++)
            if(i==stateIdx || j==stateIdx) table[i][j].incompatible=true;
}

void impTable::changeStateName(string oldName, string newName)
{
    for(int i=0;i<stateAmount;i++)
    {
        //states
        for(int j=0;j<states[i].termsAmount;j++)
        {
            if(states[i].nexts[j]==oldName) this->states[i].nexts[j]=newName;
        }
        
        //table
        for(int j=0;j<i;j++)
        {
            if(table[i][j].incompatible) continue;
            for(int k=0;k<table[i][j].termsAmount;k++)
            {
                string eachNext=table[i][j].eachNexts[k];
                string row=getRow(eachNext);
                string col=getRow(eachNext, true);
                if(row==oldName) row=newName;
                if(col==oldName) col=newName;
                if(this->getStateIndex(row)>=this->getStateIndex(col)) 
                    eachNext=row+" "+col;
                else 
                    eachNext=col+" "+row;
                table[i][j].eachNexts[k]=eachNext;
            }
        }
    }
}