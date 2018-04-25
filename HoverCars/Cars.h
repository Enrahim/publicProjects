#ifndef CARS_H
#define CARS_H

#include "common.h"

struct EngineEffect {
	D3DXVECTOR3 translational;
	D3DXVECTOR3 torque;
};

/** structure for a engine in a car template */
struct EngineRawData {
//	int power;
	/** unscaled position of the engine */
	D3DXVECTOR3 position;
	/** The direction of the engine thrust; should be normalized and multiplied by power*/
	D3DXVECTOR3 direction;
};


/** Engine that is pressent in the gameworld */
class Engine {
	int fuel;
	bool activated;
//	D3DXVECTOR3 position;
//	D3DXVECTOR3 direction;
	D3DXMATRIX worldMatrix;
	EngineEffect m_effect;

public:
	Engine(EngineRawData *base, D3DXVECTOR3 &size);
	/** Draws the engine
	*	@param basetransform the world matrix of the owning car */
	void draw(const D3DXMATRIX* basetransform);
	/** activetes the engine so that next effect it will accelerate */
	void activate() { activated=true; }
	/** Returns the engine effect and stops the motor */
	EngineEffect effect();
};

/** Structure for a car template */
struct HoverCarTemplate {
	int mass;
	D3DXVECTOR3 size;
	vector<EngineRawData> engines;
};

class HoverCar
{
	D3DXVECTOR3 angularmoment;
	D3DXVECTOR3 velocity;
	D3DXVECTOR3 size;
	D3DXVECTOR3 inversInertia;
	D3DXMATRIX scalingMatrix;
	D3DXQUATERNION rotation;
	D3DXVECTOR3 position;
	int mass;
	vector<Engine*> engines;
public:
	HoverCar(HoverCarTemplate base, D3DXVECTOR3 position);
	~HoverCar();
	void move();
	void activateEngine(int engineNr);
	void draw();
};


#endif