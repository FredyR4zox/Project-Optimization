/*Search Functions for the 15-Puzzle*/

#include <iostream>
#include <array>
#include <queue>
#include <stack>
#include <unordered_set>

using namespace std;

//Change value to change the maximum depth of DFS, BFS, ASTAR and GREEDY
#define MAX_DEPTH 80

//Global function declarations
unsigned int generatedNodes;
unsigned int visitedNodes;


string GENERAL_SEARCH_DFS(Config& initialConfig, Config& finalConfig, unsigned int maxDepth);