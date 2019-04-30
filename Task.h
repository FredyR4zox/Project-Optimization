#include <vector>

using namespace std;

class Task {
public:
    int id;
    int numberPrecedings;
    vector<int> precedings;
    int duration;
    int nWorkers;

// public:
    Task();
    Task(int id, int numberPrecedings, vector<int> precedings, int duration, int nWorkers);
};
