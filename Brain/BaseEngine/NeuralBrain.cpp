#include "NeuralBrain.h"
#include <stdlib.h>

void NeuralBrain::Neuron::generate(vector<Neuron>& n) {
	synapses.resize(4);
	for(int i=0; i<4; i++) {
		synapses[i].target = &n[rand() % n.size()];
		synapses[i].weight = TREASHOLD/2-(1<<i)*TREASHOLD/10;
	}
}


bool NeuralBrain::Neuron::exite(int e) {
	exitement+=e;
	if(exitement>TREASHOLD) return true;
	if(exitement<0) exitement=0;
	return false;
}

bool NeuralBrain::Neuron::feedback(int strenght) {
	int sum=0;
	for(int i=0; i<synapses.size(); i++) {
		if(synapses[i].weight>0) {
			synapses[i].weight+=strenght;
			sum+=synapses[i].weight;
		}
		else synapses[i].weight-=strenght;
	}
	if(sum<TREASHOLD/2) return true;
	if(sum>TREASHOLD) feedback(TREASHOLD-sum);
	return false;
}

void NeuralBrain::Neuron::flush() {
	exitement=0;
}

vector<NeuralBrain::Neuron*> NeuralBrain::Neuron::activate() {
	vector<Synapse>::iterator it=synapses.begin();
	vector<Neuron*> ret;
	for(;it!=synapses.end(); it++) {
		if(it->target->exite(it->weight)) {
			ret.push_back(it->target);
		}
	}
	exitement=0;
	return ret;
}

int NeuralBrain::addPercept(int width, int strength) {
	if(lastPercept+width>=firstActuator) return -1;
	int n=percepts.size();
	percepts.resize(n+1);
	for(int i=0; i<width; i++) {
		percepts[n].targets.push_back(&neurons[lastPercept++]);
	}
	percepts[n].weight=strength;
	lastPercept+=width;
	return n;
}

int NeuralBrain::addActuator() {
	if(firstActuator<=lastPercept) return -1;
	firstActuator--;
	return (neurons.size()-firstActuator);
}

vector<int> NeuralBrain::run() {
	set<Neuron*>* nextActive=new set<Neuron*>();
	set<Neuron*>::iterator it=activate->begin();
	for(;it!=activate->end(); it++) {
		vector<Neuron*> temp=(*it)->activate();
		nextActive->insert(temp.begin(), temp.end());
	}
	if(runcount<memsize) runcount++; 
	else {
		delete activated.front();
		activated.pop_front();
	}
	activated.push_back(activate);
	activate=nextActive;
	vector<int> ret;
	int id=1;
	for(int i=neurons.size()-1; i>=firstActuator; i--) {
		if(neurons[i].exite(0)) ret.push_back(id);
		id++;
	}
	return ret;
}

void NeuralBrain::percept(int p) {
	for(int i=0; i<percepts[p].targets.size(); i++) {
		if(percepts[p].targets[i]->exite(percepts[p].weight)) {
			activate->insert(percepts[p].targets[i]);
		}
	}
}


void NeuralBrain::feedback(int strength) {
	deque<set<Neuron*>*>::iterator it=activated.begin();
	int time=1;
	for(;it!=activated.end(); it++) {
		set<Neuron*>::iterator it2=(*it)->begin();
		for(; it2!=(*it)->end(); it2++) {
			if((*it2)->feedback(strength*time)) {
				(*it2)->generate(neurons);
			}
		}
		time++;
	}
}

void NeuralBrain::flush() {
	vector<Neuron>::iterator it=neurons.begin();
	for(;it!=neurons.end(); it++) {
		it->flush();
	}
	while(!activated.empty()){
		delete activated.front();
		activated.pop_front();
	}
	delete activate;
	activate=new set<Neuron*>();
	runcount=0;
}

NeuralBrain::NeuralBrain(int numNeurons, int memSize) : 
	lastPercept(0), firstActuator(numNeurons), runcount(0), 
	memsize(memSize), activate() {
	activate=new set<Neuron*>();
	neurons.resize(numNeurons);
	for(int i=0; i<numNeurons; i++) {
		neurons[i].generate(neurons);
	}
}