#include "Cars.h"
#include "Drawer.h"

Engine::Engine(EngineRawData *base, D3DXVECTOR3& size)
{
	activated=false;
	D3DXVECTOR3 position;
	position.x=base->position.x*size.x;
	position.y=base->position.y*size.y;
	position.z=base->position.z*size.z;
	D3DXMATRIX transMatrix;
	D3DXMATRIX rotMatrix;
	D3DXVECTOR3 nullVector(0,0,0);
	D3DXMatrixLookAtRH(&rotMatrix, &nullVector, &(base->direction), &position);
	D3DXMatrixTranslation(&transMatrix, position.x, position.y, position.z);
	worldMatrix=rotMatrix*transMatrix;

	D3DXVec3Normalize(&position, &position);
	m_effect.translational=base->direction;
	D3DXVec3Cross(&m_effect.torque, &position, &base->direction); 
}

EngineEffect Engine::effect() 
{
	if(activated) {
		activated=false;
		return m_effect;
	}
	D3DXVECTOR3 nullVector(0,0,0);
	EngineEffect temp={nullVector, nullVector};
	return temp;
}

void Engine::draw(const D3DXMATRIX* basetransform) {
	D3DXMATRIX temp;
	D3DXMatrixMultiply(&temp, &worldMatrix, basetransform);
	gDrawer.drawEngine(&temp);
}

HoverCar::HoverCar(HoverCarTemplate base, D3DXVECTOR3 pos) :
	angularmoment(0,0,0),
	velocity(0,0,0),
	size(base.size),
	rotation(1,0,0,0),
	position(pos),
	mass(base.mass)	{
	double xsquare=size.x*size.x;
	double ysquare=size.y*size.y;
	double zsquare=size.z*size.z;
	inversInertia.x=12/(mass*(ysquare+zsquare));
	inversInertia.y=12/(mass*(xsquare+zsquare));
	inversInertia.z=12/(mass*(xsquare+ysquare));
	D3DXMatrixScaling(&scalingMatrix, size.x, size.y, size.z);
	vector<EngineRawData>::iterator it=base.engines.begin();
	while(it!=base.engines.end()) {
		engines.push_back(new Engine(&(*it), size));
		it++;
	}
}

HoverCar::~HoverCar(){
	vector<Engine*>::iterator it=engines.begin();
	while(it!=engines.end()) {
		delete *it;
		it++;
	}
}

void HoverCar::move()
{
	D3DXVECTOR3 force(0,0,0);
	D3DXVECTOR3 torque(0,0,0);
	EngineEffect tempeffect;
	vector<Engine*>::iterator it=engines.begin();
	while(it != engines.end()) {
		tempeffect=(*it)->effect();
		force+=tempeffect.translational;
		torque+=tempeffect.torque;
		it++;
	}
	
	D3DXMATRIX tempmat;
	D3DXQUATERNION tempquat;
	D3DXVec3TransformCoord(&force, &force, 
			D3DXMatrixRotationQuaternion(
				&tempmat, D3DXQuaternionConjugate
					(&tempquat, &rotation)));
	D3DXVec3TransformCoord(&torque, &torque, &tempmat);

	velocity += force/mass*TICKTIME;
//	velocity += gravity()*TICKTIME;
	angularmoment += torque*TICKTIME;	
	position += velocity*TICKTIME;

	
	D3DXVECTOR3 angularVelocity;
	D3DXVec3TransformCoord(&angularVelocity, &angularmoment, &tempmat);
	angularVelocity.x *= inversInertia.x;
	angularVelocity.y *= inversInertia.y;
	angularVelocity.z *= inversInertia.z;
//	D3DXMatrixRotationQuaternion(&tempmat, &rotation);
//	D3DXVec3TransformCoord(&angularVelocity, &angularVelocity, &tempmat);
	float angle=D3DXVec3Length(&angularVelocity)*TICKTIME;
	D3DXQuaternionRotationAxis(&tempquat, &angularVelocity, angle);
	rotation= tempquat * rotation;
	D3DXQuaternionNormalize(&rotation, &rotation);
}

void HoverCar::activateEngine(int engineNr) {
	if(engineNr >= engines.size() ||engines[engineNr]==NULL) return;
	engines[engineNr]->activate();
}

void HoverCar::draw()
{
	D3DXMATRIX worldMatrix;
	D3DXMATRIX tempmat;
	D3DXMatrixRotationQuaternion(&worldMatrix, &rotation);
	D3DXMatrixTranslation(&tempmat, position.x, position.y, position.z);
	D3DXMatrixMultiply(&tempmat, &worldMatrix, &tempmat);
	D3DXMatrixMultiply(&worldMatrix, &scalingMatrix, &tempmat);	
	vector<Engine*>::iterator it=engines.begin();
	while(it != engines.end()) {
		(*it)->draw(&tempmat);
		it++;
	}
	gDrawer.drawCar(&worldMatrix);
}