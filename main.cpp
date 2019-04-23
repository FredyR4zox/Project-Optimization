#include <cstdio>

class Task {
private:
    int id;
    vector<int> precedings;
    int duration;
    int nWorkers;

public:
    Task(int id, int numberPrecedings, vector<int> precedings, int duration, int nWorkers);
};

vector<Task> tasks;

int main(){
    int id, nTasks, minutes, numberPrecedings, preceding;

    while(scanf("%d", &id) != EOF){
        scanf("%d", &numberPrecedings);
        for(int i=0; i<numberPrecedings; i++){
            scanf("%d", &preceding);

        }

        tasks.push_back(task);
    }

    return 0;
}
