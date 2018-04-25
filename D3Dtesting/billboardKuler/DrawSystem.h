#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <D3DCompiler.h>

// Wierdly seem required in vs 2013 :S
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

struct Camera {
	ID3D11Buffer* eulerView;
	ID3D11Buffer* perspective;
	struct ViewStruct {
		float viewPos[3];
		float waste1;
		float viewDir[2];
		float waste2[2];
	} cpuView;

	//struct PerspectiveStruct {
	//	float xynf[4];
	//};


	Camera() : eulerView(NULL), perspective(NULL) { memset(&cpuView, 0, sizeof(cpuView)); }
	bool initialize(ID3D11Device* dev, float near, float far, float xScale, float yScale);
	void cleanup();
	~Camera() { cleanup(); }
};


class KuleAssets {
	const int numKule;
	ID3D11Buffer *cpuPos;
	ID3D11Buffer *cpuVel;
	ID3D11Buffer *visualPos;
	ID3D11Buffer *visualPosTarget;
	ID3D11Buffer *visualVelocity;
	ID3D11Buffer *visualVelocityTarget;
	ID3D11Buffer *difuseColor;
	ID3D11Buffer *radius;
	ID3D11Buffer *shaderConstants;
	ID3D11VertexShader *kuleVertexMove;
	ID3D11VertexShader *kuleVertexStatic;
	ID3D11VertexShader *kuleVertexCopy; //copy from cpu to visual first run
	ID3D11GeometryShader* kuleGeometryMove;
	ID3D11GeometryShader* kuleGeometryCopy;
	ID3D11HullShader *kuleHull;
	ID3D11DomainShader *kuleDomain;
	ID3D11PixelShader *kulePixel;
	ID3D11InputLayout *kuleCopyLayout;
	ID3D11InputLayout *kuleMoveLayout;
	ID3D11InputLayout *kuleStaticLayout;
	ID3D11DepthStencilState *pureZtestState;

	bool initialized;
	bool cleanHardware; //Indicates if the hardware buffers are clean and should be filled

public:
	void cleanup();
	bool intializeAssets(ID3D11Device *dev);
	void sampleGenerate( int seed );
	bool draw(ID3D11DeviceContext *context, ID3D11RenderTargetView *target, ID3D11DepthStencilView* zBuffer, Camera& camera);
	KuleAssets(int n) : numKule(n), cpuPos(NULL), cpuVel(NULL), visualPos(NULL), visualVelocity(NULL), 
		difuseColor(NULL), shaderConstants(NULL), kuleVertexMove(NULL), kuleVertexStatic(NULL), kuleVertexCopy(NULL),
		kuleGeometryMove(NULL), kuleGeometryCopy(NULL), kuleHull(NULL), kuleDomain(NULL), kulePixel(NULL),
		kuleCopyLayout(NULL), kuleMoveLayout(NULL), kuleStaticLayout(NULL),
		pureZtestState(NULL), initialized(false), cleanHardware(true) {;}
	struct Data {
		INT32* positions; // numKule 3-vector intigerpositions (R32G32B32_SINT)
		INT32* velocities; // numKule 3-vector intigervelocities (R32G32B32_SINT)
		unsigned char* colors; //numKule 4-vector char farger (R8G8B8A8_UNORM)
		float* radii; //numKule float vector (R32_FLOAT)
		Data(): positions(NULL), velocities(NULL), colors(NULL), radiuses(NULL) {;}
	} data; // should be filled by aplication before initialization and at updates.
};



class DrawSystem {
	HWND mainWindow;
	ID3D11Device *device;
	IDXGISwapChain *swapChain;
	ID3D11DeviceContext *imidiateContext;
	ID3D11RenderTargetView *renderTarget;
	ID3D11Texture2D *zBuffer;
	ID3D11DepthStencilView *zBufferView;
	KuleAssets* kuler;
	Camera camera;

	bool initialized;

	/** Registerer windows klasse, og oppretter hovedvinduet 
	*	@returns true ved suksess */
	bool myCreateWindow(HINSTANCE hInst);
	bool myCreateD3D11();
	bool myAttachRenderTarget();
	bool myPrepareStensilBuffer();

public:
	/** initializes the draw system. Should only be called once!
	*	@returns 
	*	0 on success
	*	1 on error when registering window class or creating window
	*	2 on unable to create device
	*	3 on unable to create renderTarget
	*	4 on unable to initialize kule assets
	*	5 on unable to initialize camera
	*	-1 on too low feature level (not currently tested)
	*	-2 on already initialized */
	int initialize(HINSTANCE hInst);
	void cleanup();

	void processMessage(MSG &msg);

	/** Draws the frame. Throws a hres on error */
	void draw();

	DrawSystem() : mainWindow(0), device(NULL), swapChain(NULL), imidiateContext(NULL),
		renderTarget(NULL), zBuffer(NULL), zBufferView(NULL), initialized(false), kuler(NULL) { ; }
	~DrawSystem() { cleanup(); }

};

template<typename T>
void safeRelease(T* &i) {
	if (i) {
		(i)->Release();
		(i) = NULL;
	}
}