/*Search Functions for the 15-Puzzle*/

#include <iostream>
#include <array>
#include <queue>
#include <stack>
#include <unordered_set>

#include "Heuristics.h"

using namespace std;

//Change value to change the maximum depth of DFS, BFS, ASTAR and GREEDY
#define MAX_DEPTH 80

//Global function declarations
unsigned int generatedNodes;
unsigned int visitedNodes;


string GENERAL_SEARCH_DFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth);
string GENERAL_SEARCH_LDFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth);
string GENERAL_SEARCH_BFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth);
string GENERAL_SEARCH_GREEDY(Config& initialConfig, Config& finalConfig, unsigned int maxDepth, unsigned int heuristicsFlag);
string GENERAL_SEARCH_ASTAR(Config& initialConfig, Config& finalConfig, unsigned int maxDepth, unsigned int heuristicsFlag);

void DFS(Config& initialConfig, Config& finalConfig);
void LDFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth);
void IDFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth);
void BFS(Config& initialConfig, Config& finalConfig);
void ASTAR(Config& initialConfig, Config& finalConfig, unsigned int heuristicsFlag);
void GREEDY(Config& initialConfig, Config& finalConfig, unsigned int heuristicsFlag);
