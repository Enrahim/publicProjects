#ifndef D3DIOSYSTEM_H
#define D3DIOSYSTEM_H

#include <windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include "D3D9Landscape.h"

//#define PICTURETEST


HRESULT g_hr;
#define TEST(x) if(FAILED(g_hr=(x))){MessageBox(mhWindow, "(" #x ") failed", "Error", MB_OK);} 

//define for attempt to speed things up
#define SPEED

//define for profiling, just doing some of the generation on first go trough
//#define TESTERFORST

const int MAXNUMCOORD=600;
const int RANDOMSEED=147433;
const char APLICATIONNAME[]="Landscape demo";
const int REGENERATEDISTANCE=10000;

// mp prefix of "historical" reasons
IDirect3D9* mpD3D=NULL;
IDirect3DDevice9* mpD3DDevice=NULL;
HWND mhWindow;

LRESULT WINAPI gEventHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

//class Landscape {
//	struct AxisInfo
//	{
//		/**	xmap[low] og xmap[high] er aksene den relaterer til
//		*	index er sortering basert på koordinat pos. */
//		int low, high, pos, depth, index;
//		void set(int l, int h, int p, int d, int i)
//		{
//			low=l;
//			high=h;
//			pos=p;
//			depth=d;
//			index=i;
//		}
//	};
//
//	struct GameVertex
//	{
//		FLOAT z, dx, dy;
//	};
//
//
//	GameVertex* mVertexData;
//	int* mXcoord;
//	int* mYcoord;
//	int maxCoordinate;
//	int baseSightRange;
//	int maxDepth;
//	int maxNumCoord;
//
//	void test(int low, int high, int minCoord, int maxCoord, int depth, int* coordinateNumber, 
//		float pos, AxisInfo* coordinateMap, int *axisIndex, int* coordMap);
//
//
//	static const int RANDOMROOF=150889;
//	float heightRnd(int seed, int x, int y)
//	{
//		return fabs(((seed * (x + maxCoordinate + 3) * (y + 3 * maxCoordinate + 10) )%RANDOMROOF)/(float)RANDOMROOF);
//	}
//
//
//public:
//
//	Landscape();
//	~Landscape();
//
//
//};


class IOsystem
{	

	int landscapeX, landscapeY;
	D3DLandscape* mpLandscape;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;
	int lastX, lastY;

	IDirect3DVertexBuffer9 * pPictureBuffer;

	struct PictureVertex
	{
		FLOAT x, y, z, rhw;
		FLOAT tu, tv;
	};
	static const DWORD D3DFVF_PICTUREVERTEX = (D3DFVF_XYZRHW|D3DFVF_TEX1);	
	class Picture
	{
		IDirect3DDevice9* mpD3DDevice;
		float mX, mY;
		int mW, mH;
		int mIndex;
		
		static const D3DCOLOR COLORKEY=D3DCOLOR_XRGB(255, 0, 255);
		LPDIRECT3DTEXTURE9 mpTexture;
	public:
		Picture() : mIndex(-1) {}
		
		HRESULT init(LPCTSTR filename, float x, float y, int width, int height);

		HRESULT init(LPCTSTR filename, float x, float y);

		/** Puts vertex data into a locked vertex-buffer
			@param vertexBuffer a offset memory pointer to memorypointer got from
			locing a static vertex-buffer **/
		void putInScene(PictureVertex * vertexBuffer, int index);

		/** Moves the picture
			PictureVertex* vertexBuffer, the same locked vertexBuffer as used in putInScene **/
		void move(PictureVertex* vertexBuffer, float x, float y);

		/** Draws texture. Asumes state is set to stream from the corect vertex buffer **/
		void draw();

		/** Removes picture from a scene, so that it won't be drawn. 
			Warning! Does not clean the vertex buffer. **/
		void removePicture()
		{
			mIndex=-1;
		}

		~Picture()
		{
			if(mpTexture) mpTexture->Release();
		}
	} mTestPicture;
	class Camera {
		D3DXVECTOR3 position;
		//0-right/left, 1-down/up, 2-back/forward 
		D3DXVECTOR3 axis[3];
	public:
		Camera();
		/** Rolls the selected angle around the selected axis
		*	@param axisNr the axis number 0-2: right, up, forward
		*	@param angle the angle of rotation */
		void roll(int axisNr, float angle);
		/** Moves the camera along the selected axis.
		*	@param axisNr the axis to move along 0-2: right, up, forward.
		*	@param step how far to move */
		void move(int axisNr, float step);
		void getViewMatrix(D3DXMATRIX& viewMat);
		void axisReset();
		int getX(){ return position.x; }
		int getY(){ return position.y; }
	} camera;

	void render();
	void generateLandscapeIndexes();
	void handleKeyDown(int keyKode);

public:

	IOsystem()
	{
		mpD3D=NULL;
		mpD3DDevice=NULL;
		mhWindow=NULL;
	}

	struct LandscapeVertex
	{
		FLOAT x, y, z;
		D3DCOLOR color;
	};

	static const DWORD D3DFVF_LANDSCAPEVERTEX=(D3DFVF_XYZ|D3DFVF_DIFFUSE);

	LRESULT eventHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	void initDirect3D();
	void cleanupDirect3D();
	void myRegisterWindowClass(HINSTANCE hInstance);
	int runIOsystem(HINSTANCE hInstance, int nShowFlags);
} ioSystem;	

#endif