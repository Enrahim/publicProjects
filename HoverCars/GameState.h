#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "common.h"
#include "Cars.h"
#include "InputHandeling.h"
#include "Drawer.h"

class GameState{
	vector<HoverCar*> cars;
	KeyTranslator keyReader;

public:
	~GameState();
	void initiate();
	void activateEngine(int carID, int engineID);
	void tick();
	/** Should only be called by the drawer after beginScene! */
	void drawCars();
};

extern GameState gGameState;
#endif