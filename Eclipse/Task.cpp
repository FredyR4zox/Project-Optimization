#include <vector>
#include "Task.h"

using namespace std;

Task::Task(){
    this->id = 0;
    this->numberOfPrecedings = 0;
    this->children = vector<int>();
    this->duration = 0;
    this->nWorkers = 0;
}

Task::Task(int id, int numberOfPrecedings, vector<int>& children, int duration, int nWorkers){
    this->id = id;
    this->numberOfPrecedings = numberOfPrecedings;
    this->children = children;
    this->duration = duration;
    this->nWorkers = nWorkers;
}


void Task::setID(int id){
	this->id = id;
}

void Task::setNumberOfPrecedings(int numberOfPrecedings){
	this->numberOfPrecedings = numberOfPrecedings;
}

void Task::addChildren(int id){
	this->children.push_back(id);
}

void Task::setDuration(int duration){
	this->duration = duration;
}

void Task::setNWorkers(int nWorkers){
	this->nWorkers = nWorkers;
}


int Task::getID(){
	return this->id;
}

int Task::getNumberOfPrecedings(){
	return this->numberOfPrecedings;
}

vector<int> Task::getChildren(){
	return this->children;
}

int Task::getDuration(){
	return this->duration;
}

int Task::getNWorkers(){
	return this->nWorkers;
}
