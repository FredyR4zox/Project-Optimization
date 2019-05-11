#include <vector>
#include "TaskGraph.h"

using namespace std;

TaskGraph::Task::Task(){
    this->id = 0;
    this->numberOfPrecedings = 0;
    this->children = vector<int>();
    this->duration = 0;
    this->nWorkers = 0;
}

TaskGraph::Task::Task(int id, int numberOfPrecedings, vector<int>& children, int duration, int nWorkers){
    this->id = id;
    this->numberOfPrecedings = numberOfPrecedings;
    this->children = children;
    this->duration = duration;
    this->nWorkers = nWorkers;
}


void TaskGraph::Task::setID(int id){
	this->id = id;
}

void TaskGraph::Task::setNumberOfPrecedings(int numberOfPrecedings){
	this->numberOfPrecedings = numberOfPrecedings;
}

void TaskGraph::Task::addChildren(int id){
	this->children.push_back(id);
}

void TaskGraph::Task::setDuration(int duration){
	this->duration = duration;
}

void TaskGraph::Task::setNWorkers(int nWorkers){
	this->nWorkers = nWorkers;
}


int TaskGraph::Task::getID(){
	return this->id;
}

int TaskGraph::Task::getNumberOfPrecedings(){
	return this->numberOfPrecedings;
}

vector<int> TaskGraph::Task::getChildren(){
	return this->children;
}

int TaskGraph::Task::getDuration(){
	return this->duration;
}

int TaskGraph::Task::getNWorkers(){
	return this->nWorkers;
}




TaskGraph::TaskGraph::TaskGraph(){
	this->nodes = vector<Node*>();
	this->arcs = vector<Arc*>();
}

TaskGraph::TaskGraph::TaskGraph(vector<Node*>& nodes, vector<Arc*>& arcs){
	this->nodes = nodes;
	this->arcs = arcs;
}


void TaskGraph::TaskGraph::setNodes(vector<Node*>& nodes){
	this->nodes = nodes;
}

void TaskGraph::TaskGraph::setArcs(vector<Arc*>& arcs){
	this->arcs = arcs;
}


vector<TaskGraph::Node*> TaskGraph::TaskGraph::getNodes(){
	return this->nodes;
}

vector<TaskGraph::Arc*> TaskGraph::TaskGraph::getArcs(){
	return this->arcs;
}




TaskGraph::Node::Node(){
	this->id = 0;
	this->numberOfPrecedings = 0;
	this->children = vector<Arc*>();
}

TaskGraph::Node::Node(int id, int numberOfPrecedings, vector<Arc*>& children){
	this->id = id;
	this->numberOfPrecedings = numberOfPrecedings;
	this->children = children;
}


void TaskGraph::Node::setID(int id){
	this->id = id;
}

void TaskGraph::Node::setNumberOfPrecedings(int numberOfPrecedings){
	this->numberOfPrecedings = numberOfPrecedings;
}

void TaskGraph::Node::addChildren(Arc* arc){
	this->children.push_back(arc);
}


int TaskGraph::Node::getID(){
	return this->id;
}

int TaskGraph::Node::getNumberOfPrecedings(){
	return this->numberOfPrecedings;
}

vector<TaskGraph::Arc*> TaskGraph::Node::getChildren(){
	return this->children;
}




TaskGraph::Arc::Arc(){
	this->id = 0;
	this->numberOfPrecedings = 0;
	this->children = vector<int>();
	this->duration = 0;
	this->nWorkers = 0;
}

TaskGraph::Arc::Arc(int id, int numberOfPrecedings, vector<int>& children, int duration, int nWorkers){
	this->id = id;
	this->numberOfPrecedings = numberOfPrecedings;
	this->children = children;
	this->duration = duration;
	this->nWorkers = nWorkers;
}


void TaskGraph::Arc::setID(int id){
	this->id = id;
}

void TaskGraph::Arc::setNumberOfPrecedings(int numberOfPrecedings){
	this->numberOfPrecedings = numberOfPrecedings;
}

void TaskGraph::Arc::addChildren(int id){
	this->children.push_back(id);
}

void TaskGraph::Arc::setDuration(int duration){
	this->duration = duration;
}

void TaskGraph::Arc::setNWorkers(int nWorkers){
	this->nWorkers = nWorkers;
}


int TaskGraph::Arc::getID(){
	return this->id;
}

int TaskGraph::Arc::getNumberOfPrecedings(){
	return this->numberOfPrecedings;
}

vector<int> TaskGraph::Arc::getChildren(){
	return this->children;
}

int TaskGraph::Arc::getDuration(){
	return this->duration;
}

int TaskGraph::Arc::getNWorkers(){
	return this->nWorkers;
}