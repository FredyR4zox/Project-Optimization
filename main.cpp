#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include "Task.h"

using namespace std;

vector<int> CPM(vector<Task*>& tasks, int nTasks){
    vector<int> ES = vector<int>(nTasks+2, 0);
    int prec[nTasks+2], entStep[nTasks+2], minDur = -1, vf = 0;
    queue<int> q;

    for(int i=0; i<=nTasks+1; i++){
        //ES[i] = 0;
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

        // if(minDur < ES[v])
        //     minDur=ES[v], vf = v;

        vector<int> children = tasks[v]->getChildren();
        for(int w : children){
            if(ES[w] < ES[v] + tasks[v]->getDuration())
                ES[w] = ES[v] + tasks[v]->getDuration(), prec[w] = v;
            entStep[w]--;
            if(entStep[w] == 0)
                q.push(w);
        }
    }

    return ES;
}

int minWorkers(vector<Task*>& tasks, int nTasks, vector<int>& ES){
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



int main(){
    int id, nTasks, duration, numberOfPrecedings, preceding, nWorkers;
    vector<Task*> tasks;

    scanf("%d", &nTasks);
    tasks.resize(nTasks+2, NULL);

    for(int i=1; i<=nTasks; i++)
        tasks[i] = new Task();

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


    Task* start = new Task();
    start->setID(0);

    Task* end = new Task();
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

    vector<int> ES = CPM(tasks, nTasks);
//    vector<int> EF = ES;
//   for(int i=0; i<=nTasks+1; i++)
//        EF[i] = ES[i] + tasks[i]->getDuration();
//        printf("ES[%d] = %d\n", i, ES[i]);

    printf("minDur %d\n", ES[nTasks+1]);
    printf("minWorkers %d\n", minWorkers(tasks, nTasks, ES));

    for(Task* t : tasks)
        delete t;

    return 0;
}

