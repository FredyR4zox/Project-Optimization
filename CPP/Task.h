#include <vector>
#include <array>

using namespace std;

class Task {
private:
    int id;
    int numberOfPrecedings;
    vector<int> children;
    int duration;
    int nWorkers;

public:
    Task();
    Task(int id, int numberOfPrecedings, vector<int>& children, int duration, int nWorkers);

    void setID(int id);
    void setNumberOfPrecedings(int numberOfPrecedings);
    void addChildren(int id);
    void setDuration(int duration);
	void setNWorkers(int nWorkers);

    int getID();
    int getNumberOfPrecedings();
    vector<int> getChildren();
    int getDuration();
    int getNWorkers();
};
