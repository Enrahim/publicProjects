#ifndef NEURALBRAIN	
#define NEURALBRAIN

#include <vector>
#include <deque>
#include <set>

using namespace std;



class NeuralBrain {
	static const int TREASHOLD=100000;
	class Neuron {
		struct Synapse {
			Neuron* target;
			int weight;
		};
		vector<Synapse> synapses;
		int exitement;
	public:
		void generate(vector<Neuron>& n);
		bool exite(int e);
		bool feedback(int strenght);
		void flush();
		vector<Neuron*> activate();
		Neuron() : exitement(0) {}
	};

	struct Percept {
		vector<Neuron*> targets;
		int weight;
	};

	vector<Neuron> neurons;
	set<Neuron*>* activate;
	deque<set<Neuron*>*> activated;
	vector<Percept> percepts;

	int lastPercept;
	int firstActuator;
	int runcount;
	const int memsize;

public:
	int addPercept(int width, int strength);
	int addActuator();
	vector<int> run();
	void feedback(int strength);
	void percept(int p);
	void flush();
	NeuralBrain(int numNeurons, int memSize);
};



#endif