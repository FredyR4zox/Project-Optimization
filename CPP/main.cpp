#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include "Task.h"

using namespace std;

int CPM(Task* tasks[], int nTasks, int ES[], int prec[]){
    int entStep[nTasks+2], minDur = -1, vf = 0;
    queue<int> q;

    for(int i=0; i<=nTasks+1; i++){
        ES[i] = 0;
        prec[i] = 0;
        entStep[i] = 0;
    }
    prec[nTasks+1] = -1;

    for(int i=0; i<=nTasks+1; i++)
        entStep[i] = tasks[i]->getNumberOfPrecedings();

    for(int i=0; i<=nTasks+1; i++)
        if(entStep[i] == 0)
            q.push(i);

    while(!q.empty()){
        int v = q.front();
        q.pop();

        if(minDur < ES[v])
            minDur = ES[v], vf = v;

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

int minWorkers(Task* tasks[], int nTasks, int* ES){
    int nWorkers = 0, maxWorkers = 0;

    auto comp = [](const pair<int, int>& a, const pair<int, int>& b) -> bool {
        // If they have the same time, put first the negatives (end of a task)
        if(a.first == b.first)
            return a.second < b.second;
        return a.first < b.first;
    };
    /* This multiset's pair first value will be the start time or end time of a task and
    the second value will be the number of workers needed or the number of workers
    not needed anymore (positive/negative integer) */
    multiset<pair<int, int>, decltype(comp)> time(comp);
    for(int i=0; i<=nTasks+1; i++){
        time.insert(make_pair(ES[i], tasks[i]->getNWorkers()));
        //printf("boas %d\n", tasks[i]->getNWorkers());
        time.insert(make_pair(ES[i]+tasks[i]->getDuration(), -tasks[i]->getNWorkers()));
    }

    for(pair<int, int> p : time){
        //printf("removed %d %d\n", p.first, p.second);
        nWorkers += p.second;
        //printf("nWorkers %d\n", nWorkers);
        if(nWorkers > maxWorkers)
            maxWorkers = nWorkers;
    }

    //printf("aaa %d\n", nWorkers);
    return maxWorkers;
}


void calculateLS(Task* tasks[], int nTasks, int* ES, int* LS, int* prec){
    for(int i=0; i<=nTasks+1; i++){
        LS[i] = ES[i];
        for(int j=0; j<=nTasks+1; j++)
            if(prec[j] == i)
                LS[i] = ES[j];
    }
}

void DFS(Task* tasks[], int nTasks, int task, int* ES, int* LS, int* prec, vector<int> entStep, int* start, queue<int> q, int* bestStart, int* bestNWorkers){
    for(int i=0; i<=nTasks+1; i++){
        vector<int> c = tasks[i]->getChildren();
        if(find(c.begin(), c.end(), task) != c.end()){
            //printf("found %d in childrens %d\n", task, i);
            entStep[i]--;
            //printf("entStep %d\n", entStep[i]);
            if(entStep[i] == 0){
                //printf("push %d\n", i);
                q.push(i);
            }
        }
    }

    if(q.empty()){
        for(int i=ES[task]; i<=LS[task]; i++){
            start[task] = i;
            printf("[");
            for(int j=0; j<nTasks+1; j++)
                printf("%d, ", start[j]);
            printf("%d]\n", start[nTasks+1]);
            int minW = minWorkers(tasks, nTasks, start);
            if(minW < *bestNWorkers){
                *bestNWorkers = minW;
                for(int j=0; j<=nTasks+1; j++)
                    bestStart[j] = start[j];
            }
        }

        return;
    }

    int v = q.front(), min = ES[nTasks+1];
    q.pop();

    vector<int> children = tasks[task]->getChildren();
    for(int id : children)
        if(min > start[id])
            min = start[id];

    //printf("analizing %d\n", task);
    //printf("v = %d\n", v);
    for(int i=ES[task]; i<=min; i++){
        start[task] = i;
        DFS(tasks, nTasks, v, ES, LS, prec, entStep, start, q, bestStart, bestNWorkers);
    }
}


int main(){
    int nTasks, id, duration, numberOfPrecedings, preceding, nWorkers;

    scanf("%d", &nTasks);
    Task* tasks[nTasks+2];
    //tasks.resize(nTasks+2, NULL);

    for(int i=0; i<=nTasks+1; i++)
        tasks[i] = new Task();

    tasks[0]->setID(0);
    tasks[nTasks+1]->setID(nTasks+1);

    for(int i=0; i<nTasks; i++){
        scanf("%d %d", &id, &numberOfPrecedings);
        tasks[id]->setID(id);

        while(numberOfPrecedings--){
            scanf("%d", &preceding);
            tasks[id]->addChildren(preceding);
            tasks[preceding]->setNumberOfPrecedings(tasks[preceding]->getNumberOfPrecedings()+1);
        }

        scanf("%d %d\n", &duration, &nWorkers);
        tasks[id]->setDuration(duration);
        tasks[id]->setNWorkers(nWorkers);
    }

    for(int i=1; i<=nTasks; i++){
        if(tasks[i]->getNumberOfPrecedings() == 0){
            tasks[0]->addChildren(tasks[i]->getID());
            tasks[i]->setNumberOfPrecedings(1);
        }

        if(tasks[i]->getChildren().size() == 0){
            tasks[i]->addChildren(tasks[nTasks+1]->getID());
            tasks[nTasks+1]->setNumberOfPrecedings(tasks[nTasks+1]->getNumberOfPrecedings()+1);
        }
    }

    /*
    for(int i=0; i<=nTasks+1; i++){
        printf("children of %d\n", i);
        vector<int> children = tasks[i]->getChildren();
        for(int j=0; j<children.size(); j++)
            printf("\t %d", children[j]);
        printf("\n");
    }
    */

    int ES[nTasks+2], LS[nTasks+2], prec[nTasks+2], start[nTasks+2];

    int minDur = CPM(tasks, nTasks, ES, prec);
//    vector<int> EF = ES;
//   for(int i=0; i<=nTasks+1; i++)
//        EF[i] = ES[i] + tasks[i]->getDuration();
//        printf("ES[%d] = %d\n", i, ES[i]);

    printf("minDur %d %d\n", ES[nTasks+1], minDur);
    printf("minWorkers %d\n", minWorkers(tasks, nTasks, ES));

    calculateLS(tasks, nTasks, ES, LS, prec);


    int bestStart[nTasks+2], bestNWorkers;
    vector<int> entStep;
    //entStep.reserve(nTasks+2);

    queue<int> q;
    for(int i=0; i<=nTasks+1; i++){
        entStep.push_back(tasks[i]->getChildren().size());
        if(entStep[i] == 0)
            q.push(i);
    }

    int task = q.front();
    q.pop();

    DFS(tasks, nTasks, task, ES, LS, prec, entStep, start, q, bestStart, &bestNWorkers);

    printf("%d\n", bestNWorkers);

    for(Task* t : tasks)
        delete t;

    return 0;
}

