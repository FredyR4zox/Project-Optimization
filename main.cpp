#include <cstdio>
#include <vector>
#include <queue>
#include "Task.h"

using namespace std;

int main(){
    int id, nTasks, duration, numberPrecedings, preceding, nWorkers;
    vector<Task> tasks;

    scanf("%d", &nTasks);
    tasks.resize(nTasks);

    for(int i=0; i<nTasks; i++){
        scanf("%d %d", &id, &numberPrecedings);
        vector<int> precedings;
        for(int j=0; j<numberPrecedings; j++){
            scanf("%d", &preceding);
            precedings.push_back(preceding);
        }
        scanf("%d %d", &duration, &nWorkers);

        tasks[id] = Task(id, numberPrecedings, precedings, duration, nWorkers);
    }

    return 0;
}

int CPM(int nTasks, vector<Task> &tasks){
    int ES[nTasks], prec[nTasks], entStep[nTasks], durMin = -1, vf = -1;
    queue<int> q;

    for(int i=0; i<nTasks, i++){
        ES[i] = 0;
        prec[i] = -1;
        entStep[i] = 0;
    }

    for(Task& t : tasks){
        entStep[t.id] = t.numberPrecedings;
        if(t.numberPrecedings == 0)
            q.push(t.id);
    }

    while(!q.empty()){
        int v = q.front();
        q.pop();

        if(durMin < ES[v])
            durMin=ES[v], vf = v;
    }



}
