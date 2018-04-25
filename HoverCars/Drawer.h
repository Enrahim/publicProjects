#ifndef DRAWER_H
#define DRAWER_H
#include "common.h"

class Drawer {
protected:
	bool mInitiated;
	IDirect3D9* mpD3D;
	IDirect3DDevice9* mpD3DDevice;
	D3DXMATRIX mProjectionMatrix;

	D3DXMATRIX mViewMatrix;

public:
	Drawer(){ mInitiated=false; }
	/** Prepares direct3d drawing system */
	void initiate(HWND window);
	void shutdown();

	/** Tests if the system is succefully initiated */
	bool initiated() { return mInitiated; }
};

class CarGameDrawer : public Drawer {
	LPD3DXMESH mpCarMesh;
	LPD3DXMESH mpEngineMesh;
public:
	void initiate(HWND window);
	
	void drawScene();

	void drawCar(D3DXMATRIX *worldMatrix);
	void drawEngine(D3DXMATRIX *worldMatrix);
};

extern CarGameDrawer gDrawer;

#endif