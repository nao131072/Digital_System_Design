// BDD PLA節點電路圖之化簡

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct node{
    int id=0;
    char variable='0';
    int elseEdge=-1;
    int thenEdge=-1;
    string comment="";
};

int main(int argc, char *argv[]){
    
    string strBinary(int num, int digit);
    void toTruthTable(int inputCount, char *inputNames, int plaCount, string *plaInputs, bool *plaOutputs, bool *truthTable);
    void initNodes(node *nodes, int nodeCount);
    void fillNodes(node *nodes, int nodeCount, int inputCount, char *inputNames, bool *truthTable);
    void replaceId(node *nodes, int nodeCount, int id, int newId);
    bool checkSame(char checkVariable, node *nodes, int nodeCount);
    bool checkUnneed(char checkVariable, node *nodes, int nodeCount);
    void printNodes(node *nodes, int nodeCount);
    void generateGraphviz(node *nodes, int nodeCount, char *inputNames, int inputCount, fstream &outputFile);
    void generatePNG(string dotFileName, string pngFileName);
    
    // pla file example:
    /*
        .i 3
        .o 1
        .ilb a b c
        .ob F
        .p 2
        11- 1
        --1 1
        .e
    */
    fstream plaFile;
    fstream outputFile;
    
    if(argc!=3){
        cout << "Error: execute format error\n./BDD {input pla file name} {output dot file name}" << endl;
        return 0;
    }
    string plaFileName=argv[1];
    string outputFileName=argv[2];
    
    plaFile.open(plaFileName, ios::in);
    if(!plaFile){
        cout << "Error: pla file not found." << endl;
        return 0;
    }
    outputFile.open(outputFileName, ios::out);
    
    int inputCount=0, outputCount=0;
    char *inputNames=new char[inputCount];
    char *outputNames=new char[outputCount];
    
    int plaCount=0;
    string *plaInputs=new string[plaCount];
    bool *plaOutputs=new bool[plaCount];
        
    bool *truthTable= new bool[1<<inputCount]; 
    
    node *nodes=nullptr;
    
    string prefix="";
    plaFile >> prefix;
    while(prefix != ".e"){
        if(prefix == ".i"){
            plaFile >> inputCount;
        }
        else if(prefix == ".o"){
            plaFile >> outputCount;
        }
        else if(prefix == ".ilb"){
            for(int i = 0; i < inputCount; i++){
                plaFile >> inputNames[i];
            }
        }
        else if(prefix == ".ob"){
            for(int i = 0; i < outputCount; i++){
                plaFile >> outputNames[i];
            }
        }
        else if(prefix == ".p"){
            plaFile >> plaCount;
            plaInputs = new string[plaCount];
            plaOutputs = new bool[plaCount];
            for (int i = 0; i < plaCount; i++){
                string input;
                bool output;
                plaFile >> input >> output;
                plaInputs[i] = input;
                plaOutputs[i] = output;
            }
        }
        else{
            // no .p, direct to pla string and bool
            plaCount++;
            cout<<"plaCount: "<<plaCount<<"\n";
            string *tempInputs = new string[plaCount];
            bool *tempOutputs = new bool[plaCount];
            for (int i = 0; i < plaCount-1; i++){
                tempInputs[i] = plaInputs[i];
                tempOutputs[i] = plaOutputs[i];
            }
            string input="";
            bool output=false;
            input = prefix;
            plaFile >> output;
            tempInputs[plaCount-1] = input;
            tempOutputs[plaCount-1] = output;
            delete[] plaInputs;
            delete[] plaOutputs;
            plaInputs = tempInputs;
            plaOutputs = tempOutputs;
        }
        plaFile >> prefix;
    }
    
    //start here
            
    truthTable=new bool[1<<inputCount];
            
    toTruthTable(inputCount, inputNames, plaCount, plaInputs, plaOutputs, truthTable);
            
    int nodeCount=0;
    for (int i = 0; i < inputCount; i++){
        nodeCount += 1<<i;
    }
    nodeCount+=2;
    nodes=new node[nodeCount];
            
    initNodes(nodes, nodeCount);
            
    fillNodes(nodes, nodeCount, inputCount, inputNames, truthTable);
            
    for(int i=inputCount-1; i>=0; i--){
        char checkVariable=inputNames[i];
        bool changed=false;
        changed+=checkSame(checkVariable, nodes, nodeCount);
        changed+=checkUnneed(checkVariable, nodes, nodeCount);
        if(changed) printNodes(nodes, nodeCount);
    }
            
            
    generateGraphviz(nodes, nodeCount, inputNames, inputCount, outputFile);
            
    generatePNG(outputFileName, outputFileName+".png");
            
    delete[] plaInputs;
    delete[] plaOutputs;
    delete[] truthTable;
    delete[] nodes;
    
    plaFile.close();
}

string strBinary(int num, int digit){
    string binary="";
    while(num>0){
        binary=(num%2==0?"0":"1")+binary;
        num/=2;
    }
    while(binary.length()<digit){
        binary="0"+binary;
    }
    return binary;
}

void toTruthTable(int inputCount, char *inputNames, int plaCount, string *plaInputs, bool *plaOutputs, bool *truthTable){
    for(int i=0;i<(1<<inputCount);i++){
        bool F=0;
        string input=strBinary(i, inputCount);
        cout<<"input: "<<input<<" ";
        for(int j=0;j<plaCount;j++){
            if(plaOutputs[j]==0)
                continue;
            bool f=1;
            string plaInput=plaInputs[j];
            for(int d=0;d < inputCount;d++){
                if(plaInput[d]=='-') continue;
                if(plaInput[d] != input[d])
                    {f=0;break;}
            }
            F=F+f;
            cout<<"f: "<<f<<" ";
        }
        truthTable[i]=F;
        cout<<"F:"<<F<<endl;
    }
}

void initNodes(node *nodes, int nodeCount){
    nodes[0].id=0;
    nodes[0].variable='0';
    nodes[0].elseEdge=-1;
    nodes[0].thenEdge=-1;
    nodes[0].comment="Boolean 0";
    nodes[nodeCount-1].id=nodeCount-1;
    nodes[nodeCount-1].variable='1';
    nodes[nodeCount-1].elseEdge=-1;
    nodes[nodeCount-1].thenEdge=-1;
    nodes[nodeCount-1].comment="Boolean 1";
}

void fillNodes(node *nodes, int nodeCount, int inputCount, char *inputNames, bool *truthTable){
    int currentIndex=1;
    int edgeCount=2;
    int truthCount=0;
    for(int i=0;i<inputCount;i++){
        int sameNameCount=1<<i;
        for(int j=0;j<sameNameCount;j++){
            nodes[currentIndex].id=currentIndex;
            cout<<"id: "<<nodes[currentIndex].id<<" ";
            nodes[currentIndex].variable=inputNames[i];
            cout<<"variable: "<<nodes[currentIndex].variable<<" ";
            
            if(i!=inputCount-1)
            {
                nodes[currentIndex].elseEdge=edgeCount;
                cout<<"elseEdge: "<<nodes[currentIndex].elseEdge<<" ";
                nodes[currentIndex].thenEdge=edgeCount+1;
                cout<<"thenEdge: "<<nodes[currentIndex].thenEdge<<"\n";
                edgeCount+=2;
            }
            else
            {
                nodes[currentIndex].elseEdge=truthTable[truthCount]?nodeCount-1:0;
                cout<<"elseEdge: "<<nodes[currentIndex].elseEdge<<" ";
                nodes[currentIndex].thenEdge=truthTable[truthCount+1]?nodeCount-1:0;
                cout<<"thenEdge: "<<nodes[currentIndex].thenEdge<<"\n";
                truthCount+=2;
            }
            currentIndex++;
        }
    }
}

void replaceId(node *nodes, int nodeCount, int id, int newId){
    for(int i=0;i<nodeCount;i++){
        if(nodes[i].elseEdge==id)
            nodes[i].elseEdge=newId;
        if(nodes[i].thenEdge==id)
            nodes[i].thenEdge=newId;
    }
}

bool checkSame(char checkVariable, node *nodes, int nodeCount){
    bool same=false;
    for(int i=0;i<nodeCount;i++){
        if(nodes[i].variable!=checkVariable || nodes[i].comment=="redundant")
            continue;
            
        node node1=nodes[i];
        for(int j=i+1;j<nodeCount;j++){
            if(nodes[j].variable!=checkVariable || nodes[i].comment=="redundant")
                continue;
            node node2=nodes[j];
            if(node1.elseEdge==node2.elseEdge && node1.thenEdge==node2.thenEdge){
                nodes[j].comment="redundant";
                replaceId(nodes, nodeCount, nodes[j].id, nodes[i].id);
                same=true;
            }
        }
    }
    return same;
}

bool checkUnneed(char checkVariable, node *nodes, int nodeCount){
    bool unneed=false;
    for(int i=0;i<nodeCount;i++){
        if(nodes[i].variable!=checkVariable || nodes[i].comment=="redundant")
            continue;
        node node1=nodes[i];
        
        if(node1.elseEdge==node1.thenEdge){
            nodes[i].comment="redundant";
            replaceId(nodes, nodeCount, nodes[i].id, nodes[i].elseEdge);
            unneed=true;
        }
    }
    return unneed;
}

void printNodes(node *nodes, int nodeCount){
    cout<<"id\tvariable\telseEdge\tthenEdge\tcomment\n";
    for(int i=0;i<nodeCount;i++){
        cout<<nodes[i].id<<"\t"<<nodes[i].variable<<"\t\t"<<nodes[i].elseEdge<<"\t\t"<<nodes[i].thenEdge<<"\t\t"<<nodes[i].comment<<"\n";
    }
    cout<<"\n";
}

// dot file example:
/*
    digraph ROBDD {
        {rank=same 1}
        {rank=same 3}
        {rank=same 4}
        0 [label=0, shape=box]
        1 [label="a"]
        3 [label="b"]
        4 [label="c"]
        8 [label=1, shape=box]
        1 -> 4 [label="0", style=dotted]
        1 -> 3 [label="1", style=solid]
        3 -> 4 [label="0", style=dotted]
        3 -> 8 [label="1", style=solid]
        4 -> 0 [label="0", style=dotted]
        4 -> 8 [label="1", style=solid]
    }

*/

void generateGraphviz(node *nodes, int nodeCount, char *inputNames, int inputCount, fstream &outputFile){
    outputFile<<"digraph BDD {\n";
    
    // ranks
    for(int i=0;i<inputCount;i++)
    {
        char *indexs=nullptr;
        int indexCount=0;
        char variable=inputNames[i];
        for(int j=0;j<nodeCount;j++)
        {
            if(nodes[j].comment=="redundant")
                continue;
            if(nodes[j].variable==variable)
            {
                char *temp=new char[indexCount+1];
                for(int k=0;k<indexCount;k++)
                    temp[k]=indexs[k];
                temp[indexCount]=nodes[j].id;
                indexCount++;
                delete[] indexs;
                indexs=temp;
            }
        }

        if(indexCount>=1)
        {
            outputFile<<"\t{rank=same ";
            for(int j=0;j<indexCount;j++)
                outputFile << ""<< to_string(indexs[j]) <<" ";
            outputFile<<"}\n";
        }
    }
    outputFile<<"\n";
    
    //labels
    outputFile<<"\t0 [label=\"0\", shape=box]\n";
    for(int i=1;i<nodeCount-1;i++)
    {
        if(nodes[i].comment=="redundant")
            continue;
        outputFile<<"\t"<<nodes[i].id<<" [label=\""<<nodes[i].variable<<"\"]\n";
    }
    outputFile<<"\t"<<nodeCount-1<<" [label=\"1\", shape=box]\n";
    outputFile<<"\n";
    
    //edges
    for(int i=1;i<nodeCount-1;i++)
    {
        if(nodes[i].comment=="redundant")
            continue;
        outputFile<<"\t"<<nodes[i].id<<" -> "<<nodes[nodes[i].elseEdge].id<<" [label=\"0\", style=dotted]\n";
        outputFile<<"\t"<<nodes[i].id<<" -> "<<nodes[nodes[i].thenEdge].id<<" [label=\"1\", style=solid]\n";
    }
    
    outputFile<<"}";
    
    outputFile.close();
}

void generatePNG(string dotFileName, string pngFileName){
    string command="dot -T png "+dotFileName+" > "+pngFileName;
    system(command.c_str());
}