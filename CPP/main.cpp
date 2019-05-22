#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include "Task.h"
#include <cstring>

using namespace std;

int CPM_ES(Task* tasks[], int nTasks, int ES[], int prec[]){
    int entStep[nTasks+2], minDur = -1, vf = 0;
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

int minWorkers(Task* tasks[], int nTasks, int* ES, int minDur){
    int nWorkers = 0, maxWorkers = 0, points[minDur+1];
    for(int i=0; i<minDur+1; i++)
        points[i] = 0;
    //memset(points, 0, minDur+1);

    for(int i=0; i<=nTasks+1; i++){
        if(ES[i] == -1)
            continue;
        points[ES[i]] += tasks[i]->getNWorkers();
        points[ES[i] + tasks[i]->getDuration()] -= tasks[i]->getNWorkers();
    }

    for(int i=0; i<=minDur; i++){
        nWorkers += points[i];
        //printf("nWorkers %d\n", nWorkers);
        if(nWorkers > maxWorkers)
            maxWorkers = nWorkers;
    }

    /*
    auto comp = [](const pair<int, int>& a, const pair<int, int>& b) -> bool {
        // If they have the same time, put first the negatives (end of a task)
        if(a.first == b.first)
            return a.second < b.second;

        return a.first < b.first;
    };
    */
    /* This multiset's pair first value will be the start time or end time of a task and
    the second value will be the number of workers needed or the number of workers
    not needed anymore (positive/negative integer) */
    /*
    multiset<pair<int, int>, decltype(comp)> time(comp);
    for(int i=0; i<=nTasks+1; i++){
        if(ES[i] == -1)
            continue;
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
    }*/

    //printf("aaa %d\n", maxWorkers);
    return maxWorkers;
}

int minCriticalWorkers(Task* tasks[], int nTasks, int* ES, int* LS){
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
        if(ES[i] == -1 || ES[i] != LS[i])
            continue;
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

auto pqComp = [](const pair<int, int>& a, const pair<int, int>& b) -> bool {
    // If they have the same time, put first the negatives (end of a task)
    if(a.second == b.second)
        return a.first > b.first;

    return a.second > b.second;
};

void DFS(Task* tasks[], int nTasks, int task, int* ES, int* LS, priority_queue<pair<int, int>, vector<pair<int, int> >, decltype(pqComp)>& slack, int* start, int* bestStart, int* bestNWorkers){
    int n = minWorkers(tasks, nTasks, start, 0);
    if(n > *bestNWorkers)
        return;

    /*
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
    */

    int min = LS[task];
    vector<int> children = tasks[task]->getChildren();
    for(int id : children)
        if(start[id] != -1 && min > start[id] - tasks[task]->getDuration())
            min = start[id] - tasks[task]->getDuration();

    if(slack.empty()){
        for(int i=ES[task]; i<=min; i++){
            start[task] = i;
            /*
            printf("[");
            for(int j=0; j<nTasks+1; j++)
                printf("%d, ", start[j]);
            printf("%d]\n", start[nTasks+1]);*/
            int minW = minWorkers(tasks, nTasks, start, 0);
            if(minW < *bestNWorkers){
                *bestNWorkers = minW;
                for(int j=0; j<=nTasks+1; j++)
                    bestStart[j] = start[j];
            }
        }

        start[task] = -1;

        return;
    }

    int v = slack.top().first;
    //printf("poped %d\n", slack.top().second);
    slack.pop();

    //printf("analizing %d\n", task);
    //printf("v = %d\n", v);
    for(int i=ES[task]; i<=min; i++){
        start[task] = i;
        DFS(tasks, nTasks, v, ES, LS, slack, start, bestStart, bestNWorkers);
    }
    start[task] = -1;
    //printf("pushed %d\n", LS[v] - ES[v]);
    slack.push(make_pair(v, LS[v] - ES[v]));
}

void branchAndBound(Task* tasks[], int nTasks, int task, int* ES, int* LS, int minDur, priority_queue<pair<int, int>, vector<pair<int, int> >, decltype(pqComp)>& slack, bool* points, int* start, int* bestStart, int* bestNWorkers){
    int n = minWorkers(tasks, nTasks, start, minDur);
    if(n > *bestNWorkers)
        return;

    /*
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
    */

    int max = LS[task];
    vector<int> children = tasks[task]->getChildren();
    set<int> notAllowed;
    for(int id : children){
        // if(start[id] != -1 && max > start[id] - tasks[task]->getDuration())
        //     max = start[id] - tasks[task]->getDuration();
        if(start[id] != -1){
            int test = start[id] - tasks[task]->getDuration();
            for(int i=ES[task]; i<=LS[task]; i++)
                if(i > test)
                    notAllowed.insert(i);
        }
    }

    if(slack.empty()){
        for(int i=ES[task]; i<=LS[task]; i++){
            if(notAllowed.find(i) != notAllowed.end())
                continue;

            start[task] = i;
            int minW = minWorkers(tasks, nTasks, start, minDur);
            if(minW < *bestNWorkers){
                *bestNWorkers = minW;
                for(int j=0; j<=nTasks+1; j++)
                    bestStart[j] = start[j];
            }
        }
        // start[task] = ES[task];
        // int minW = minWorkers(tasks, nTasks, start, minDur);
        // if(minW < *bestNWorkers){
        //     *bestNWorkers = minW;
        //     for(int j=0; j<=nTasks+1; j++)
        //         bestStart[j] = start[j];
        // }
        //
        // for(int i=ES[task]+1; i<max; i++){
        //     if(points[i]){
        //         start[task] = i;
        //         minW = minWorkers(tasks, nTasks, start, minDur);
        //         if(minW < *bestNWorkers){
        //             *bestNWorkers = minW;
        //             for(int j=0; j<=nTasks+1; j++)
        //                 bestStart[j] = start[j];
        //         }
        //     }
        // }
        //
        // start[task] = max;
        // minW = minWorkers(tasks, nTasks, start, minDur);
        // if(minW < *bestNWorkers){
        //     *bestNWorkers = minW;
        //     for(int j=0; j<=nTasks+1; j++)
        //         bestStart[j] = start[j];
        // }

        start[task] = -1;

        return;
    }

    int v = slack.top().first;
    //printf("poped %d\n", slack.top().second);
    slack.pop();


    //printf("analizing %d\n", task);
    //printf("v = %d\n", v);
    // start[task] = ES[task];
    // bool alreadyTrue = points[ES[task]], alreadyTrue2 = points[ES[task]+tasks[task]->getDuration()];
    // points[ES[task]] = true;
    // points[ES[task]+tasks[task]->getDuration()] = true;
    // branchAndBound(tasks, nTasks, v, ES, LS, minDur, slack, points, start, bestStart, bestNWorkers);
    // points[ES[task]] = alreadyTrue;
    // points[ES[task]+tasks[task]->getDuration()] = alreadyTrue2;
    //
    // for(int i=ES[task]+1; i<min; i++){
    //     if(points[i]){
    //         start[task] = ES[task];
    //         alreadyTrue = points[ES[task]], alreadyTrue2 = points[ES[task]+tasks[task]->getDuration()];
    //         points[ES[task]] = true;
    //         points[ES[task]+tasks[task]->getDuration()] = true;
    //         branchAndBound(tasks, nTasks, v, ES, LS, minDur, slack, points, start, bestStart, bestNWorkers);
    //         points[ES[task]] = alreadyTrue;
    //         points[ES[task]+tasks[task]->getDuration()] = alreadyTrue2;
    //     }
    // }
    //
    // start[task] = LS[task];
    // alreadyTrue = points[ES[task]], alreadyTrue2 = points[ES[task]+tasks[task]->getDuration()];
    // points[ES[task]] = true;
    // points[ES[task]+tasks[task]->getDuration()] = true;
    // branchAndBound(tasks, nTasks, v, ES, LS, minDur, slack, points, start, bestStart, bestNWorkers);
    // points[ES[task]] = alreadyTrue;
    // points[ES[task]+tasks[task]->getDuration()] = alreadyTrue2;

    for(int i=ES[task]; i<=LS[task]; i++){
        if(notAllowed.find(i) != notAllowed.end())
            continue;

        start[task] = i;
        branchAndBound(tasks, nTasks, v, ES, LS, minDur, slack, points, start, bestStart, bestNWorkers);
    }


    start[task] = -1;
    //printf("pushed %d\n", LS[v] - ES[v]);
    slack.push(make_pair(v, LS[v] - ES[v]));
}

void branchAndBound2(Task* tasks[], int nTasks, int task, int* ES, int* LS, int minDur, priority_queue<pair<int, int>, vector<pair<int, int> >, decltype(pqComp)>& slack, int* points, int* start, int maxNWorkers, int* bestStart, int* bestNWorkers){

    vector<int> children = tasks[task]->getChildren();
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

    if(slack.empty()){
        for(int i=ES[task]; i<=LS[task]; i++){
            if(notAllowed.find(i) != notAllowed.end())
                continue;

            start[task] = i;
            test = i+duration;
            for(int j=i; j<test; j++){
                points[j] += taskWorkers;
                if(points[j] > maxNWorkers)
                    maxNWorkers = points[j];
            }

            //printf("found with %d, best %d\n", maxNWorkers, *bestNWorkers);

            if(maxNWorkers < *bestNWorkers){
                *bestNWorkers = maxNWorkers;
                printf("mehhhh\n");
                for(int j=0; j<=nTasks+1; j++)
                    bestStart[j] = start[j];
            }

            for(int j=i; j<test; j++)
                points[j] -= taskWorkers;
        }

        start[task] = -1;

        return;
    }

    int v = slack.top().first;
    slack.pop();

    int oldMaxNWorkers = maxNWorkers;
    for(int i=ES[task]; i<=LS[task]; i++){
        maxNWorkers = oldMaxNWorkers;
        if(notAllowed.find(i) != notAllowed.end())
            continue;

        start[task] = i;
        test = i+duration;
        for(int j=i; j<test; j++){
            points[j] += taskWorkers;
            if(points[j] > maxNWorkers)
                maxNWorkers = points[j];
        }

        if(maxNWorkers > *bestNWorkers){
            //printf("cut\n");
            for(int j=i; j<test; j++)
                points[j] -= taskWorkers;

            continue;
        }

        branchAndBound2(tasks, nTasks, v, ES, LS, minDur, slack, points, start, maxNWorkers, bestStart, bestNWorkers);
        for(int j=i; j<test; j++)
            points[j] -= taskWorkers;
    }


    start[task] = -1;
    slack.push(make_pair(v, LS[v] - ES[v]));
}

void branchAndBound3(Task* tasks[], int nTasks, int task, int* ES, int* LS, int minDur, priority_queue<pair<int, int>, vector<pair<int, int> >, decltype(pqComp)>& slack, int* points, int* start, int maxNWorkers, int* bestStart, int* bestNWorkers, int* restart){

    vector<int> children = tasks[task]->getChildren();
    set<int> notAllowed;
    bool notAllowed2[minDur+1] = {false};
    int duration = tasks[task]->getDuration(), taskWorkers = tasks[task]->getNWorkers(), test;

    for(int id : children){
        if(start[id] != -1){
            test = start[id] - duration;
            for(int i=ES[task]; i<=LS[task]; i++)
                if(i > test)
                    notAllowed2[i] = true;
        }
    }

    if(slack.empty()){
        for(int i=ES[task]; i<=LS[task]; i++){
            if(notAllowed2[i])
                continue;

            start[task] = i;
            test = i+duration;
            for(int j=i; j<test; j++){
                points[j] += taskWorkers;
                if(points[j] > maxNWorkers)
                    maxNWorkers = points[j];
            }

            //printf("found with %d, best %d\n", maxNWorkers, *bestNWorkers);

            if(maxNWorkers < *bestNWorkers){
                *bestNWorkers = maxNWorkers;
                printf("mehhhh\n");
                for(int j=0; j<=nTasks+1; j++)
                    bestStart[j] = start[j];
            }

            for(int j=i; j<test; j++)
                points[j] -= taskWorkers;
        }

        start[task] = -1;

        return;
    }

    int v = slack.top().first;
    slack.pop();

    int oldMaxNWorkers = maxNWorkers;
    for(int i=ES[task]; i<=LS[task]; i++){
        maxNWorkers = oldMaxNWorkers;
        if(notAllowed.find(i) != notAllowed.end())
            continue;

        start[task] = i;
        test = i+duration;
        for(int j=i; j<test; j++){
            points[j] += taskWorkers;
            if(points[j] > maxNWorkers)
                maxNWorkers = points[j];
        }

        if(maxNWorkers > *bestNWorkers){
            //printf("cut\n");
            for(int j=i; j<test; j++)
                points[j] -= taskWorkers;

            continue;
        }

        branchAndBound3(tasks, nTasks, v, ES, LS, minDur, slack, points, start, maxNWorkers, bestStart, bestNWorkers);

        for(int j=i; j<test; j++)
            points[j] -= taskWorkers;
    }


    start[task] = -1;
    slack.push(make_pair(v, LS[v] - ES[v] - tasks[v]->getDuration()));
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

    int ES[nTasks+2], LF[nTasks+2], LS[nTasks+2], prec[nTasks+2], start[nTasks+2];

    int minDur = CPM_ES(tasks, nTasks, ES, prec);
    CPM_LF(tasks, nTasks, LF, minDur);

    for(int i=0; i<=nTasks+1; i++)
        LS[i] = LF[i]-tasks[i]->getDuration();

    int bestNWorkers = minWorkers(tasks, nTasks, ES, minDur);
    int criticalNWorkers = minCriticalWorkers(tasks, nTasks, ES, LS);

    for(int i=0; i<=nTasks+1; i++)
        printf("%d: ES = %d, LS = %d, dur = %d\n", i, ES[i], LS[i], tasks[i]->getDuration());

    printf("minDur %d %d\n", ES[nTasks+1], minDur);
    printf("minWorkers %d\n", bestNWorkers);
    printf("minCriticalWorkers %d\n", criticalNWorkers);


    int bestStart[nTasks+2];
    bool points[minDur+1];
    for(int i=0; i<minDur+1; i++)
        points[i] = false;
    /*
    vector<int> entStep;
    //entStep.reserve(nTasks+2);


    queue<int> q;
    for(int i=0; i<=nTasks+1; i++){
        start[i] = -1;
        entStep.push_back(tasks[i]->getChildren().size());
        if(entStep[i] == 0)
            q.push(i);
    }

    int task = q.front();
    q.pop();
    */
    int points2[minDur+1];
    for(int i=0; i<=minDur; i++)
        points2[i] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int> >, decltype(pqComp)> slack(pqComp);
    for(int i=0; i<=nTasks+1; i++){
        start[i] = -1;
        if(ES[i] == LS[i]){
            start[i] = ES[i];

            for(int j=ES[i]; j<ES[i]+tasks[i]->getDuration(); j++)
                points2[j] += tasks[i]->getNWorkers();
        }
        else{
            slack.push(make_pair(i, LS[i] - ES[i] - tasks[i]->getDuration()));
            //printf("insert %d\n", LS[i] - ES[i]);
        }
    }

    int task = slack.top().first;
    //printf("poped %d\n", slack.top().second);
    slack.pop();

    branchAndBound3(tasks, nTasks, task, ES, LS, minDur, slack, points2, start, criticalNWorkers, bestStart, &bestNWorkers);

    for(int i=0; i<=nTasks+1; i++)
        printf("%d = %d\n", i, bestStart[i]);

    printf("%d\n", bestNWorkers);

    for(Task* t : tasks)
        delete t;

    return 0;
}
