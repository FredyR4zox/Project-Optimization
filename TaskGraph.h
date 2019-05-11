#include <vector>
#include <array>

using namespace std;

namespace TaskGraph {
	class Task;
	class Arc;
	class Node;
	class TaskGraph;
};

namespace TaskGraph {
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

	class Arc {
	private:
	    int id;
	    int numberOfPrecedings;
	    vector<int> children;
	    int duration;
	    int nWorkers;
	    array<Node*, 2> nodes;

	public:
	    Arc();
	    Arc(int id, int numberOfPrecedings, vector<int>& children, int duration, int nWorkers);

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

	class Node {
	private:
	    int id;
	    int numberOfPrecedings;
	    vector<Arc*> children;

	public:
	    Node();
	    Node(int id, int numberOfPrecedings, vector<Arc*>& children);

	    void setID(int id);
	    void setNumberOfPrecedings(int numberOfPrecedings);
	    void addChildren(Arc* arc);

	    int getID();
	    int getNumberOfPrecedings();
	    vector<Arc*> getChildren();
	};

	class TaskGraph {
	private:
		vector<Node*> nodes;
		vector<Arc*> arcs;

	public:
	    TaskGraph();
	    TaskGraph(vector<Node*>& nodes, vector<Arc*>& arcs);

	    void setNodes(vector<Node*>& nodes);
	    void setArcs(vector<Arc*>& arcs);

	    vector<Node*> getNodes();
	    vector<Arc*> getArcs();
	};
};
