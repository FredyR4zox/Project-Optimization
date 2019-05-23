#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include "Task.h"
#include <cstring>

using namespace std;

int CPM_ES(Task* tasks[], int nTasks, int ES[], int prec[]){
    int entStep[nTasks+2], minDur = -1;
    queue<int> q;

    for(int i=0; i<=nTasks+1; i++){
        ES[i] = 0;
        prec[i] = 0;
        entStep[i] = tasks[i]->getNumberOfPrecedings();
        if(entStep[i] == 0)
            q.push(i);
    }
    prec[nTasks+1] = -1;

    while(!q.empty()){
        int v = q.front();
        q.pop();

        if(minDur < ES[v])
            minDur = ES[v];

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

void CPM_LF(Task* tasks[], int nTasks, int LF[], int minDur){
    int entStep[nTasks+2];
    queue<int> q;

    for(int i=0; i<=nTasks+1; i++){
        LF[i] = minDur;
        entStep[i] = tasks[i]->getChildren().size();
        if(entStep[i] == 0)
            q.push(i);
    }

    while(!q.empty()){
        int v = q.front();
        q.pop();

        for(int w=0; w<=nTasks+1; w++){
            vector<int> children = tasks[w]->getChildren();
            if(find(children.begin(), children.end(), v) != children.end()){
                if(LF[w] > LF[v] - tasks[v]->getDuration())
                    LF[w] = LF[v] - tasks[v]->getDuration();
                entStep[w]--;
                if(entStep[w] == 0)
                    q.push(w);
            }
        }
    }
}

int calculateWorkers(Task* tasks[], int nTasks, int* ES, int minDur){
    int nWorkers = 0, maxWorkers = 0, points[minDur+1] = { 0 };

    for(int i=0; i<=nTasks+1; i++){
        if(ES[i] == -1)
            continue;

        points[ES[i]] += tasks[i]->getNWorkers();
        points[ES[i] + tasks[i]->getDuration()] -= tasks[i]->getNWorkers();
    }

    for(int i=0; i<=minDur; i++){
        nWorkers += points[i];
        if(nWorkers > maxWorkers)
            maxWorkers = nWorkers;
    }

    return maxWorkers;
}

int calculateCriticalWorkers(Task* tasks[], int nTasks, int* ES, int* LS, int minDur){
    int nWorkers = 0, maxWorkers = 0, points[minDur+1] = { 0 };

    for(int i=0; i<=nTasks+1; i++){
        if(ES[i] == -1 || ES[i] != LS[i])
            continue;

        points[ES[i]] += tasks[i]->getNWorkers();
        points[ES[i] + tasks[i]->getDuration()] -= tasks[i]->getNWorkers();
    }

    for(int i=0; i<=minDur; i++){
        nWorkers += points[i];
        if(nWorkers > maxWorkers)
            maxWorkers = nWorkers;
    }

    return maxWorkers;
}

auto pqComp = [](const pair<int, int>& a, const pair<int, int>& b) -> bool {
    // If they have the same time, put first the negatives (end of a task)
    if(a.second == b.second)
        return a.first > b.first;

    return a.second > b.second;
};

bool DFS(int maxWorkers, Task* tasks[], int nTasks, int* ES, int* LS, int minDur, priority_queue<pair<int, int>, vector<pair<int, int> >, decltype(pqComp)>& slack, int* points, int* start, int curMaxWorkers, int* bestStart){
    if(slack.empty()){
        copy(start, start+nTasks+2, bestStart);

        return true;
    }

    int task = slack.top().first;
    slack.pop();

    vector<int> children = tasks[task]->getChildren();
    vector<int> children2;
    set<int> notAllowed;
    int duration = tasks[task]->getDuration(), taskWorkers = tasks[task]->getNWorkers(), test;

    for(int id : children){
        if(start[id] != -1){
            test = start[id] - duration;
            for(int i=ES[task]; i<=LS[task]; i++)
                if(i > test)
                    notAllowed.insert(i);
        }
    }
    for(int i=0; i<=nTasks+1; i++){
        children2 = tasks[i]->getChildren();
        if(find(children2.begin(), children2.end(), task) != children2.end()){
            if(start[i] != -1){
                test = start[i] + tasks[i]->getDuration();
                for(int j=ES[task]; j<=LS[task]; j++)
                    if(j < test)
                        notAllowed.insert(j);
            }
        }
    }

    int oldCurMaxWorkers = curMaxWorkers;
    for(int i=ES[task]; i<=LS[task]; i++){
        if(notAllowed.find(i) != notAllowed.end())
            continue;

        start[task] = i;

        int ES2[nTasks+2], LS2[nTasks+2];
        copy(ES, ES+nTasks+2, ES2);
        copy(LS, LS+nTasks+2, LS2);

        for(int id : children)
            if(start[id] == -1)
                ES2[id] = max(ES2[id], i+duration);
        for(int j=0; j<=nTasks+1; j++){
            children2 = tasks[j]->getChildren();
            if(find(children2.begin(), children2.end(), task) != children2.end())
                if(start[j] == -1)
                    LS2[j] = min(LS2[j], i - tasks[j]->getDuration());
        }

        test = i + duration;
        curMaxWorkers = oldCurMaxWorkers;
        for(int j=i; j<test; j++){
            points[j] += taskWorkers;
            if(points[j] > curMaxWorkers)
                curMaxWorkers = points[j];
        }

        if(curMaxWorkers > maxWorkers){
            for(int j=i; j<test; j++)
                points[j] -= taskWorkers;

            continue;
        }

        bool ret = DFS(maxWorkers, tasks, nTasks, ES2, LS2, minDur, slack, points, start, curMaxWorkers, bestStart);

        for(int j=i; j<test; j++)
            points[j] -= taskWorkers;

        if(ret){
            start[task] = -1;
            slack.push(make_pair(task, LS[task] - ES[task] - tasks[task]->getDuration()));
            return true;
        }
    }


    start[task] = -1;
    slack.push(make_pair(task, LS[task] - ES[task] - tasks[task]->getDuration()));
    return false;
}

int main(){
    int nTasks, id, duration, numberOfPrecedings, preceding, nWorkers;

    scanf("%d", &nTasks);
    Task* tasks[nTasks+2];

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

    int ES[nTasks+2], LF[nTasks+2], LS[nTasks+2], prec[nTasks+2];

    int minDur = CPM_ES(tasks, nTasks, ES, prec);
    CPM_LF(tasks, nTasks, LF, minDur);

    for(int i=0; i<=nTasks+1; i++)
        LS[i] = LF[i]-tasks[i]->getDuration();

    int esWorkers = calculateWorkers(tasks, nTasks, ES, minDur);
    int criticalWorkers = calculateCriticalWorkers(tasks, nTasks, ES, LS, minDur);

    for(int i=0; i<=nTasks+1; i++)
        printf("%d: ES = %d, LS = %d, dur = %d\n", i, ES[i], LS[i], tasks[i]->getDuration());

    printf("minDur %d %d\n", ES[nTasks+1], minDur);
    printf("minWorkers %d\n", esWorkers);
    printf("minCriticalWorkers %d\n", criticalWorkers);


    int start[nTasks+2], bestStart[nTasks+2], points[minDur+1] = { 0 };
    fill(start, start+nTasks+2, -1);
    copy(ES, ES+nTasks+2, bestStart);

    priority_queue<pair<int, int>, vector<pair<int, int> >, decltype(pqComp)> slack(pqComp);
    for(int i=0; i<=nTasks+1; i++){
        if(ES[i] == LS[i]){
            start[i] = ES[i];

            for(int j=ES[i]; j<ES[i]+tasks[i]->getDuration(); j++)
                points[j] += tasks[i]->getNWorkers();
        }
        else
            slack.push(make_pair(i, LS[i] - ES[i] - tasks[i]->getDuration()));
    }

    for(int i=criticalWorkers; i<=esWorkers; i++){
        if(DFS(i, tasks, nTasks, ES, LS, minDur, slack, points, start, criticalWorkers, bestStart)){
            esWorkers = i;
            break;
        }
    }


    for(int i=1; i<=nTasks; i++)
        printf(" %d", bestStart[i]);

    printf("\n%d\n", esWorkers);

    for(Task* t : tasks)
        delete t;

    return 0;
}
