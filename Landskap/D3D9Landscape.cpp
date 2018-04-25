#include "D3D9Landscape.h"

using namespace LandscapeHelpers;

D3DLandscape::D3DLandscape() : 
	mpLandscapeBuffer(NULL), mpLandscapeIndexes(NULL),
	buffersize(0), indexsize(0),
	DISTANCEFACTOR(1/128.0), HEIGHTFACTOR(1/16.0)
{
}

D3DLandscape::~D3DLandscape() {
	if(mpLandscapeBuffer) mpLandscapeBuffer->Release();
	if(mpLandscapeIndexes) mpLandscapeIndexes->Release();
}

void D3DLandscape::draw() {

	vector<int> xcoords;
	vector<int> ycoords;
	vector<int> heights;

	getHeightMap(LANDSCAPECENTER, xcoords, ycoords, heights);
	if(heights.size()>buffersize) {
		if(mpLandscapeBuffer) mpLandscapeBuffer->Release();
		if (FAILED(mpD3DDevice->CreateVertexBuffer(heights.size()*sizeof(LandscapeVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_LANDSCAPEVERTEX, D3DPOOL_DEFAULT, &mpLandscapeBuffer, NULL))){
			buffersize=0;
			return;
		}
		buffersize=heights.size();
	}

	LandscapeVertex* v;
	HRESULT hr;
	if(FAILED(hr=mpLandscapeBuffer->Lock(0, heights.size()*sizeof(LandscapeVertex), (void**)&v, D3DLOCK_DISCARD))){
		return;
	}
	
	int k=0;
	for(int i=0; i<ycoords.size(); i++){
		for(int j=0; j<xcoords.size(); j++) {
			v[k].x=DISTANCEFACTOR*xcoords[j];
			v[k].y=DISTANCEFACTOR*ycoords[i];
			v[k].z=HEIGHTFACTOR*heights[k];
			v[k].color=D3DCOLOR_RGBA(0,heights[k]%255,0, 127);
			k++;
		}
	}

	if(FAILED(mpLandscapeBuffer->Unlock())) {

		return;
	}

	if(xcoords.size()>indexsize) {
		if(mpLandscapeIndexes) mpLandscapeIndexes->Release();
		if(FAILED(mpD3DDevice->CreateIndexBuffer(xcoords.size()*2*sizeof(UINT16), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mpLandscapeIndexes, NULL))){
			indexsize=0;
			return;
		}
		indexsize=xcoords.size();
	}

	UINT16* id;

	mpLandscapeIndexes->Lock(0, xcoords.size()*2*sizeof(UINT16), (void**)&id, D3DLOCK_DISCARD);
	for(int i=0; i<xcoords.size(); i++)
	{
		id[2*i+1]=i;
		id[2*i]=i+xcoords.size();
	}
	mpLandscapeIndexes->Unlock();
	
	//TODO fail-testing
	mpD3DDevice->SetStreamSource(0, mpLandscapeBuffer, 0, sizeof(LandscapeVertex));
	mpD3DDevice->SetIndices(mpLandscapeIndexes);

	for(int i=0; i<ycoords.size()-1; i++) {
		
		mpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, i*xcoords.size(), 0, 2*xcoords.size(), 0, xcoords.size()*2-2);
	}


}

