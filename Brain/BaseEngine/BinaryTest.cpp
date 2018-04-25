#include "NeuralBrain.h"
#include <iostream>

int main(int argc, char* argv[]){
	NeuralBrain brain(1000, 6);
	const int timePercept=brain.addPercept(5, 20000);
	const int leftPercept=brain.addPercept(5, 50000);
	const int rightPercept=brain.addPercept(5, 50000);
	const int yesDo=brain.addActuator();
	const int noDo=brain.addActuator();
	int wait=0;
	int yes=0;
	int no=0;
	int noRight=0;
	int lerror=0, rerror=0, lrerror=0, error=0; 
	int timeout=0;
	bool left=(rand()&1);
	bool right=(rand()&1);
	for(int i=0; i<2000000; i++) {
		brain.percept(timePercept);
		if(left) brain.percept(leftPercept);
		if(right) brain.percept(rightPercept);
		vector<int> res=brain.run();
		if(!res.empty()) {
			if(res.back()==yesDo) {
				if(left && right) {
					brain.feedback(600);
					yes++;
				}
				else {
					brain.feedback(-200);
					no++;
					if(left) lerror++;
					else if(right) rerror++;
					else error++;
				}
			} else {
				if(left && right) {
					brain.feedback(-600);
					no++;
					lrerror++;
				}
				else {
					brain.feedback(200);
					yes++;
					noRight++;
				}
			}
			brain.flush();
			left=(rand()&1);
			right=(rand()&1);
			wait=0;
		}
		wait++;
		if(wait>10000) {
			brain.feedback(-500);
			wait=0;
			timeout++;
		}
	}
	cout <<	"yes: " << yes << "  no: " << no << endl;
	cout <<"of the right these were \"no\" answers: "<<noRight;
	cout << endl<< "Error left and right: " << lrerror;
	cout << " Error left: " <<lerror<<" Error right: "<<rerror; 
	cout << endl << " Timeouts: " << timeout;
	cin >> wait;
	cout << "ok";
}