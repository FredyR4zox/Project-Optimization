#include <vector>
#include "Task.h"

using namespace std;

Task::Task(){
    this->id = 0;
    this->numberPrecedings = 0;
    this->duration = 0;
    this->nWorkers = 0;
}

Task::Task(int id, int numberPrecedings, vector<int> precedings, int duration, int nWorkers){
    this->id = id;
    this->numberPrecedings = numberPrecedings;
    this->precedings = precedings;
    this->duration = duration;
    this->nWorkers = nWorkers;
}
