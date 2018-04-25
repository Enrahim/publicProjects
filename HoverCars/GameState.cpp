#include "GameState.h"

GameState::~GameState() {
	vector<HoverCar*>::iterator it=cars.begin();
	while(it!=cars.end()){
		delete *it;
		it++;
	}
}

void GameState::activateEngine(int carID, int engineID) {
	if(carID >= cars.size() ||cars[carID]==NULL) return;
	cars[carID]->activateEngine(engineID);
}

void GameState::tick() {
	keyReader.handleKeys();
	vector<HoverCar*>::iterator it=cars.begin();
	while(it!=cars.end()){
		(*it)->move();
		it++;
	}
	gDrawer.drawScene();
}

void GameState::drawCars() {
	vector<HoverCar*>::iterator it=cars.begin();
	while(it!=cars.end()){
		(*it)->draw();
		it++;
	}
}

void GameState::initiate() {
	EngineRawData engine1;
	EngineRawData engine2;
	engine1.position=D3DXVECTOR3(0,1,0);
	engine1.direction=D3DXVECTOR3(1,0,0);
	engine2.position=D3DXVECTOR3(0,-1, 0);
	engine2.direction=D3DXVECTOR3(1,0,0);

	HoverCarTemplate car;
	car.mass=100000;
	car.size=D3DXVECTOR3(1,4,5);
	car.engines.push_back(engine1);
	car.engines.push_back(engine2);
	cars.push_back(new HoverCar(car, D3DXVECTOR3(0,0,0)));

	KeyEntry temp;
	temp.keyCode=0x41; //A
	temp.carID=0;
	temp.engineIDs.push_back(0);
	keyReader.addEntry(temp);

	temp.keyCode=0x51; //Q
	temp.carID=0;
	temp.engineIDs.pop_back();
	temp.engineIDs.push_back(1);
	keyReader.addEntry(temp);
}