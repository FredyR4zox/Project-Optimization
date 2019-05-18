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
