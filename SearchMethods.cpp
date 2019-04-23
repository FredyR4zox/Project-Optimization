/*Search Functions for the 15-Puzzle*/

#include <iostream>
#include <array>
#include <queue>
#include <stack>
#include <unordered_set>

#include "SearchMethods.h"

using namespace std;


//General Search Algorithm to search for a solution
string GENERAL_SEARCH_DFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth){
    Node *initialNode = new Node(initialConfig);
    generatedNodes++;

    stack<Node*> q;
    q.push(initialNode);

    unordered_set<Config> hashSet;

    while(!q.empty()){
        Node *removed = q.top();
        q.pop();

        visitedNodes++;

        if(removed->getConfig() == finalConfig){
            cout << "Solution found on depth: " << removed->getDepth() << endl;
            string str = removed->makePath();
            delete initialNode;
            return str;
        }

        if(removed->getDepth() < maxDepth){
            array<Node*, 4> descendantList = removed->makeDescendants(&hashSet);
            for(unsigned int i=0; i<4 && descendantList[i]!=NULL; i++){
                generatedNodes++;
                q.push(descendantList[i]);
            }
        }
    }

    delete initialNode;

    return "Solution not found";
}

//General Search Algorithm to search for a solution
string GENERAL_SEARCH_LDFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth){
    Node *initialNode = new Node(initialConfig);
    generatedNodes++;

    stack<Node*> q;
    q.push(initialNode);

    while(!q.empty()){
        Node *removed = q.top();
        q.pop();

        visitedNodes++;

        if(removed->getConfig() == finalConfig){
            cout << "Solution found on depth: " << removed->getDepth() << endl;
            string str = removed->makePath();
            delete initialNode;
            return str;
        }

        if(removed->getDepth() < maxDepth){
            array<Node*, 4> descendantList = removed->makeDescendants(NULL);
            for(unsigned int i=0; i<4 && descendantList[i]!=NULL; i++){
                generatedNodes++;
                q.push(descendantList[i]);
            }
        }
    }

    delete initialNode;

    return "Solution not found";
}

//General Search Algorithm to search for a solution
string GENERAL_SEARCH_BFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth){
    Node *initialNode = new Node(initialConfig);
    generatedNodes++;

    queue<Node*> q;
    q.push(initialNode);

    unordered_set<Config> hashSet;

    unsigned int depth = 0;

    while(!q.empty()){
        Node *removed = q.front();
        q.pop();

        if(removed->getDepth() > depth){
            cout << "Depth: " << removed->getDepth() << "\tTotal of generated nodes: " << generatedNodes << " \tTotal of visited nodes: " << visitedNodes << endl;
            depth = removed->getDepth();
        }

        visitedNodes++;

        if(removed->getConfig() == finalConfig){
            cout << endl << "Solution found on depth: " << removed->getDepth() << endl;
            string str = removed->makePath();
            delete initialNode;
            return str;
        }

        if(removed->getDepth() < maxDepth){
            array<Node*, 4> descendantList = removed->makeDescendants(&hashSet);
            for(unsigned int i=0; i<4 && descendantList[i]!=NULL; i++){
                generatedNodes++;
                q.push(descendantList[i]);
            }
        }
    }

    delete initialNode;

    return "Solution not found";
}

//General Search Algorithm to search for a solution
string GENERAL_SEARCH_ASTAR(Config& initialConfig, Config& finalConfig, unsigned int maxDepth, unsigned int heuristicsFlag){
    Node *initialNode = new Node(initialConfig);
    generatedNodes++;

    priority_queue<Node*, vector<Node*>, compareNodes> q;
    q.push(initialNode);

    while(!q.empty()){
        Node *removed = q.top();
        q.pop();

        visitedNodes++;

        if(removed->getConfig() == finalConfig){
            cout << "Solution found on depth: " << removed->getDepth() << endl;
            string str = removed->makePath();
            delete initialNode;
            return str;
        }

        if(removed->getDepth() < maxDepth){
            array<Node*, 4> descendantList = removed->makeDescendants(NULL);
            calcPathCostASTAR(descendantList, finalConfig, heuristicsFlag); //calculates the pathCost for each of the children Nodes
            for(unsigned int i=0; i<4 && descendantList[i]!=NULL; i++){
                generatedNodes++;
                q.push(descendantList[i]);
            }
        }
    }

    delete initialNode;

    return "Solution not found";
}


//General Search Algorithm to search for a solution
string GENERAL_SEARCH_GREEDY(Config& initialConfig, Config& finalConfig, unsigned int maxDepth, unsigned int heuristicsFlag){
    Node *initialNode = new Node(initialConfig);
    generatedNodes++;

    priority_queue<Node*, vector<Node*>, compareNodes> q;
    q.push(initialNode);

    unordered_set<Config> hashSet;

    while(!q.empty()){
        Node *removed = q.top();
        q.pop();

        visitedNodes++;

        if(removed->getConfig() == finalConfig){
            cout << "Solution found on depth: " << removed->getDepth() << endl;
            string str = removed->makePath();
            delete initialNode;
            return str;
        }

        if(removed->getDepth() < maxDepth){
            array<Node*, 4> descendantList = removed->makeDescendants(&hashSet);
            calcPathCostGREEDY(descendantList, finalConfig,heuristicsFlag); //calculates the pathCost for each of the children Nodes
            for(unsigned int i=0; i<4 && descendantList[i]!=NULL; i++){
                generatedNodes++;
                q.push(descendantList[i]);
            }
        }
    }

    delete initialNode;

    return "Solution not found";
}


void DFS(Config& initialConfig, Config& finalConfig){   /*Depth first search function*/
    if(!solutionExists(initialConfig, finalConfig)){
        cout << "It's not possible to reach to the final configuration starting from the initial configuration" << endl << endl;    //There is no solution
        return;
    }

    generatedNodes = visitedNodes = 0;
    string str = GENERAL_SEARCH_DFS(initialConfig, finalConfig, MAX_DEPTH);

    if(str != "Solution not found")
        cout << "Movements: " << str << endl << endl;
    else
        cout << "Solution not found" << endl << endl;

    cout << "Generated Nodes: " << generatedNodes << endl;
    cout << "Visited Nodes: " << visitedNodes << endl;
    cout << "Maximum of memory spent on nodes: " << (generatedNodes*sizeof(Node))/1024 << " KB" << endl << endl;
}

void LDFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth){ /*Limited Breadth first search function*/
    if(!solutionExists(initialConfig, finalConfig)){
        cout << "It's not possible to reach to the final configuration starting from the initial configuration" << endl << endl;    //There is no solution
        return;
    }

    generatedNodes = visitedNodes = 0;
    string str = GENERAL_SEARCH_LDFS(initialConfig, finalConfig, maxDepth);

    if(str != "Solution not found")
        cout << "Movements: " << str << endl << endl;
    else
        cout << "Solution not found" << endl << endl;

    cout << "Generated Nodes: " << generatedNodes << endl;
    cout << "Visited Nodes: " << visitedNodes << endl;
    cout << "Maximum of memory spent on nodes: " << (generatedNodes*sizeof(Node))/1024 << " KB" << endl << endl;
}

void IDFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth){  /*Iterative Depth first search function*/
    if(!solutionExists(initialConfig, finalConfig)){
        cout << "It's not possible to reach to the final configuration starting from the initial configuration" << endl << endl;    //There is no solution
        return;
    }

    for(unsigned int i=1; i<=maxDepth; i++){
        generatedNodes = visitedNodes = 0;
        string str = GENERAL_SEARCH_LDFS(initialConfig, finalConfig, i);

        cout << "Depth: " << i << "\tGenerated nodes: " << generatedNodes << "  \tVisited nodes: " << visitedNodes << endl;
        cout << "Maximum of memory spent on nodes: " << (generatedNodes*sizeof(Node))/1024 << " KB" << endl << endl;

        if(str != "Solution not found"){
            cout << "Movements: " << str << endl <<  endl;
            return;
        }
    }

    cout << "Solution not found" << endl << endl;
}

void BFS(Config& initialConfig, Config& finalConfig){ /*Breadth first search function*/
    if(!solutionExists(initialConfig, finalConfig)){
        cout << "It's not possible to reach to the final configuration starting from the initial configuration" << endl << endl;    //There is no solution
        return;
    }

    generatedNodes = visitedNodes = 0;
    string str = GENERAL_SEARCH_BFS(initialConfig, finalConfig, MAX_DEPTH);

    if(str != "Solution not found")
        cout << "Movements: " << str << endl << endl;
    else
        cout << "Solution not found" << endl << endl;

    cout << "Generated Nodes: " << generatedNodes << endl;
    cout << "Visited Nodes: " << visitedNodes << endl;
    cout << "Maximum of memory spent on nodes: " << (generatedNodes*sizeof(Node))/1024 << " KB" << endl << endl;
}

void ASTAR(Config& initialConfig, Config& finalConfig, unsigned int heuristicsFlag){   /*A* search function calls overloaded 2nd version of BFS*/
    if(!solutionExists(initialConfig, finalConfig)){
        cout << "It's not possible to reach to the final configuration starting from the initial configuration" << endl << endl;    //There is no solution
        return;
    }

    generatedNodes = visitedNodes = 0;
    string str = GENERAL_SEARCH_ASTAR(initialConfig, finalConfig, MAX_DEPTH, heuristicsFlag);

    if(str != "Solution not found")
        cout << "Movements: " << str << endl << endl;
    else
        cout << "Solution not found" << endl << endl;

    cout << "Generated Nodes: " << generatedNodes << endl;
    cout << "Visited Nodes: " << visitedNodes << endl;
    cout << "Maximum of memory spent on nodes: " << (generatedNodes*sizeof(Node))/1024 << " KB" << endl << endl;
}

void GREEDY(Config& initialConfig, Config& finalConfig, unsigned int heuristicsFlag){    /*Greedy with Heuristics search function*/
    if(!solutionExists(initialConfig, finalConfig)){
        cout << "It's not possible to reach to the final configuration starting from the initial configuration" << endl << endl;    //There is no solution
        return;
    }

    generatedNodes = visitedNodes = 0;
    string str = GENERAL_SEARCH_GREEDY(initialConfig, finalConfig, MAX_DEPTH, heuristicsFlag);

    if(str != "Solution not found")
        cout << "Movements: " << str << endl << endl;
    else
        cout << "Solution not found" << endl << endl;

    cout << "Generated Nodes: " << generatedNodes << endl;
    cout << "Visited Nodes: " << visitedNodes << endl;
    cout << "Maximum of memory spent on nodes: " << (generatedNodes*sizeof(Node))/1024 << " KB" << endl << endl;
}
