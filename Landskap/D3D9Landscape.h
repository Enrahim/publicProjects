#ifndef D3D9LANDSCAPE_H
#define D3D9LANDSCAPE_H

#include <d3d9.h>
#include "landscape.h"

extern IDirect3DDevice9* mpD3DDevice;

class D3DLandscape : public Landscape {
	IDirect3DVertexBuffer9 * mpLandscapeBuffer;
	IDirect3DIndexBuffer9 * mpLandscapeIndexes;

	int buffersize;
	int indexsize;

	const float DISTANCEFACTOR;
	const float HEIGHTFACTOR;

	struct LandscapeVertex
	{
		FLOAT x, y, z;
		D3DCOLOR color;
	};
	
	static const DWORD D3DFVF_LANDSCAPEVERTEX=(D3DFVF_XYZ|D3DFVF_DIFFUSE);
	
//	LandscapeVertex* vertexes;

public:
	/** draw call to be done between begin and end scene */
	void draw();
	
	//void generateVertex(LandscapeVertex *vertexes, int x, int y, AxisInfo *xMap, AxisInfo* yMap, int nx, int seed);

	//void generateLandscape(IDirect3DVertexBuffer9 *pVertexBuffer, int seed, float xpos, float ypos, int* numX, int* numY);

	D3DLandscape();
	~D3DLandscape();
};

#endif