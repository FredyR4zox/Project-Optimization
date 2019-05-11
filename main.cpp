#include <cstdio>
#include <vector>
#include <queue>
#include "TaskGraph.h"

using namespace std;

int CPM(int nTasks, vector<TaskGraph::Task*>& tasks){
    int ES[nTasks+2], prec[nTasks+2], entStep[nTasks+2], minDur = -1, vf = 0;
    queue<int> q;

    for(int i=0; i<=nTasks+1; i++){
        ES[i] = 0;
        prec[i] = -1;
        entStep[i] = 0;
    }

    for(int i=0; i<=nTasks+1; i++)
        entStep[i] = tasks[i]->getNumberOfPrecedings();

    for(int i=0; i<=nTasks+1; i++)
        if(entStep[i] == 0)
            q.push(i);

    while(!q.empty()){
        int v = q.front();
        q.pop();

        if(minDur < ES[v])
            minDur=ES[v], vf = v;

        vector<int> children = tasks[v]->getChildren();
        for(int w : children){
            if(ES[w] < ES[v] + tasks[v]->getDuration())
                ES[w] = ES[v] + tasks[v]->getDuration(), prec[w] = v;
            entStep[w]--;
            if(entStep[w] == 0)
                q.push(w);
        }
    }

    return minDur;
}


int main(){
    int id, nTasks, duration, numberOfPrecedings, preceding, nWorkers;
    vector<TaskGraph::Task*> tasks;

    scanf("%d", &nTasks);
    tasks.resize(nTasks+2, NULL);

    for(int i=1; i<=nTasks; i++)
        tasks[i] = new TaskGraph::Task();

    for(int i=0; i<nTasks; i++){
        scanf("%d %d", &id, &numberOfPrecedings);
        tasks[id]->setID(id);
        tasks[id]->setNumberOfPrecedings(numberOfPrecedings);

        while(numberOfPrecedings--){
            scanf("%d", &preceding);
            tasks[preceding]->addChildren(id);
        }

        scanf("%d %d\n", &duration, &nWorkers);
        tasks[id]->setDuration(duration);
        tasks[id]->setNWorkers(nWorkers);
    }


    TaskGraph::Task* start = new TaskGraph::Task();
    start->setID(0);

    TaskGraph::Task* end = new TaskGraph::Task();
    end->setID(nTasks+1);

    for(int i=1; i<=nTasks; i++){
        if(tasks[i]->getNumberOfPrecedings() == 0){
            start->addChildren(tasks[i]->getID());
            tasks[i]->setNumberOfPrecedings(1);
        }

        if(tasks[i]->getChildren().size() == 0){
            tasks[i]->addChildren(end->getID());
            end->setNumberOfPrecedings(end->getNumberOfPrecedings()+1);
        }
    }
    tasks[0] = start;
    tasks[nTasks+1] = end;

    printf("minDur %d\n", CPM(nTasks, tasks));

    for(TaskGraph::Task* t : tasks)
        delete t;

    return 0;
}

