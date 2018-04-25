#include "D3D9IOSystem.h"


HRESULT IOsystem::Picture::init(LPCTSTR filename, float x, float y, int width, int height)
{
	mX=x; mY=y; mW=width; mH=height;
	HRESULT hr;
	if(FAILED(hr=D3DXCreateTextureFromFileEx(mpD3DDevice, filename, width, height, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
		D3DX_DEFAULT, D3DX_DEFAULT, COLORKEY, NULL, NULL, &mpTexture)))
	{ 
		MessageBox(mhWindow, "Unable to open picture file!", "Error!", MB_OK);
		return hr;
		
		//TODO error handeling
	}

	return hr;
}

HRESULT IOsystem::Picture::init(LPCTSTR filename, float x, float y)
{
	mX=x; mY=y;
	HRESULT hr;
	if(FAILED(hr=D3DXCreateTextureFromFileEx(mpD3DDevice, filename, 0, 0, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, 
		D3DX_DEFAULT, D3DX_DEFAULT, COLORKEY, NULL, NULL, &mpTexture)))
	{ 
		return hr;
		//TODO error handeling
	}
	D3DSURFACE_DESC desc;
	mpTexture->GetLevelDesc(0, &desc);
	mW=desc.Width;
	mH=desc.Height;
	return hr;
}

/** Puts vertex data into a locked vertex-buffer
	@param vertexBuffer a offset memory pointer to memorypointer got from
	locing a static vertex-buffer **/
void IOsystem::Picture::putInScene(PictureVertex * vertexBuffer, int index)
{
	mIndex=index;
	PictureVertex vertexes[]=
		{{mX ,mY ,1 ,1 ,0 ,0},
		{mX+mW, mY, 1, 1, 1, 0},
		{mX, mY+mH, 1, 1, 0, 1},
		{mX+mW, mY+mH, 1, 1, 1, 1}};
	memcpy(vertexBuffer+index, vertexes, 4*sizeof(PictureVertex));
}

/** Moves the picture
	PictureVertex* vertexBuffer, the same locked vertexBuffer as used in putInScene **/
void IOsystem::Picture::move(PictureVertex* vertexBuffer, float x, float y)
{
	mX=x; mY=y;
	if (mIndex=-1) return; //error! Picture isn't in any scene.
	PictureVertex vertexes[]={{x ,y ,1 ,1 ,0 ,0},
		{x+mW, y, 1, 1, 1, 0},
		{x, y+mH, 1, 1, 0, 1},
		{x+mW, y+mH, 1, 1, 1, 1}};
	memcpy(vertexBuffer+mIndex, vertexes, 4*sizeof(PictureVertex));
}

/** Draws texture. Asumes state is set to stream from the corect vertex buffer **/
void IOsystem::Picture::draw()
{
	if (mIndex==-1) return; //Picture isn't in any Scene; don't draw!
	mpD3DDevice->SetTexture(0, mpTexture);
	mpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, mIndex, 4);
}

void IOsystem::render()
{
	mpD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,255,255), 1.0f, 0 );
	camera.getViewMatrix(viewMatrix);
	int dx=camera.getX() - lastX;
	int dy=camera.getY() - lastY;
	//if (dx * dx + dy * dy > REGENERATEDISTANCE) {
	//	//Landscape landscape;
	//	int olx=landscapeX;
	//	pLandscape->generateLandscape(pLandscapeBuffer, RANDOMSEED, dx + lastX, dy + lastY, &landscapeX, &landscapeY);
	//	lastX += dx;
	//	lastY += dy;
	//	if (olx != landscapeX) generateLandscapeIndexes();

	//}
	TEST(mpD3DDevice->BeginScene())
	mpD3DDevice->SetTransform(D3DTS_VIEW, &viewMatrix);
	mpD3DDevice->SetTransform(D3DTS_PROJECTION, &projectionMatrix);
	mpD3DDevice->SetTransform(D3DTS_WORLD, &worldMatrix);
	mpD3DDevice->SetFVF(D3DFVF_LANDSCAPEVERTEX);

	mpLandscape->draw();
	//TEST(mpD3DDevice->SetStreamSource(0, pLandscapeBuffer, 0, sizeof(LandscapeVertex)))
	//TEST(mpD3DDevice->SetIndices(pLandscapeIndexes))
	//HRESULT hr;
	//for(int i=0; i<landscapeY-1; i++)
	//{
	//	
	//	if(FAILED(hr=mpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, i*landscapeX, 0, landscapeX*2, 0, (landscapeX-1)*2)))
	//	{
	//		
	//		PostMessage(mhWindow, WM_DESTROY, 0, 0);
	//	}
	//}
#ifdef PICTURETEST
	mpD3DDevice->SetFVF(D3DFVF_PICTUREVERTEX);
	mpD3DDevice->SetStreamSource(0, pPictureBuffer, 0, 0);
	mTestPicture.draw();
#endif

	mpD3DDevice->EndScene();
	mpD3DDevice->Present( NULL, NULL, NULL, NULL );
}



void IOsystem::generateLandscapeIndexes()
{
//	short* indexes=(short*)calloc(landscapeX*2, sizeof(short));
//	for(int i=0; i<landscapeX; i++)
//	{
//		indexes[2*i+1]=i;
//		indexes[2*i]=i+landscapeX;
//	}
//
//	short* output;
//	TEST(pLandscapeIndexes->Lock(0, landscapeX, (void**)&output, 0))
//	memcpy(output, indexes, 2*landscapeX*sizeof(short));
//	pLandscapeIndexes->Unlock();
}

void IOsystem::handleKeyDown(int keyKode){
	switch (keyKode) {
		case VK_NUMPAD1:
			camera.roll(0, -0.1);
			break;
		case VK_NUMPAD2:
			camera.move(2, -10);
			break;
		case VK_NUMPAD3:
			camera.move(1, -10);
			break;
		case VK_NUMPAD4:
			camera.roll(1, -0.1);
			break;
		case VK_NUMPAD5:
			camera.axisReset();
			break;
		case VK_NUMPAD6:
			camera.roll(1, 0.1);
			break;
		case VK_NUMPAD7:
			camera.roll(0, 0.1);
			break;
		case VK_NUMPAD8:
			camera.move(2, 10);
			break;
		case VK_NUMPAD9:
			camera.move(1, 10);
			break;
	}
}


LRESULT IOsystem::eventHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
{
	switch( msg )
	{
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

	    case WM_PAINT:
	        render();
	        ValidateRect( hWnd, NULL );
	        return 0;

		case WM_KEYDOWN:
			handleKeyDown(wParam);
			render();
			break;
			
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

void IOsystem::initDirect3D()
{
	mpD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if(!mpD3D) return;
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.hDeviceWindow=mhWindow;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	HRESULT result;
	if (FAILED(result=mpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		mhWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &mpD3DDevice )))
	{
		if (FAILED(result=mpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			mhWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &mpD3DDevice )))
		{
			mpD3D->Release();
			mpD3D=NULL;
			MessageBox(mhWindow, "Unable to create device!", "Error", MB_OK);
			return;
		}
	}
	mpD3DDevice->SetRenderState(D3DRS_COLORVERTEX, true);
	mpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	mpD3DDevice->SetRenderState(D3DRS_LIGHTING,false);
#ifndef PICTURETEST
	mpD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
#endif

//	TEST(mpD3DDevice->CreateVertexBuffer(MAXNUMCOORD*MAXNUMCOORD*sizeof(LandscapeVertex), D3DUSAGE_WRITEONLY, D3DFVF_LANDSCAPEVERTEX, D3DPOOL_DEFAULT, &pLandscapeBuffer, NULL))
//	Landscape landscape;
//	pLandscape->generateLandscape(pLandscapeBuffer, RANDOMSEED, 0, 0, &landscapeX, &landscapeY);
	camera.move( 1, 200 );
//	camera.move( 2, 0 );
	camera.getViewMatrix(viewMatrix);
	D3DXMatrixPerspectiveLH(&projectionMatrix, 2, 1, 1, 4000);
	D3DXMatrixIdentity(&worldMatrix);
	//mpD3DDevice->CreateIndexBuffer(landscapeX*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pLandscapeIndexes, NULL);
	//generateLandscapeIndexes();

#ifdef PICTURETEST
	TEST(mpD3DDevice->CreateVertexBuffer(4*sizeof(PictureVertex), D3DUSAGE_WRITEONLY, D3DFVF_PICTUREVERTEX, D3DPOOL_DEFAULT, &pPictureBuffer, NULL))
	mTestPicture.init("1211471074079.png", 0, 0, 100, 100);
	PictureVertex* pB;
	pPictureBuffer->Lock(0, 4*sizeof(PictureVertex),  (void**)&pB, 0);
	mTestPicture.putInScene(pB, 0);
	pPictureBuffer->Unlock();
#endif
}

void IOsystem::cleanupDirect3D()
{
#ifdef PICTURETEST
	pPictureBuffer->Release();
#endif
	//pLandscapeIndexes->Release();
	//pLandscapeBuffer->Release();
	delete mpLandscape;
	mpD3DDevice->Release();
	mpD3D->Release();
}


void IOsystem::myRegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
	wcex.lpfnWndProc	= gEventHandler;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= APLICATIONNAME;
	wcex.hIconSm		= NULL;

	RegisterClassEx(&wcex);
}

int IOsystem::runIOsystem(HINSTANCE hInstance, int nShowFlags)
{
	myRegisterWindowClass(hInstance);
	mhWindow=CreateWindow(APLICATIONNAME, APLICATIONNAME, WS_OVERLAPPEDWINDOW, 
		100, 100, 500, 500, GetDesktopWindow(), NULL, hInstance, NULL);
	if(!mhWindow) return 100;

	initDirect3D();
	if(!mpD3D) return 200;

	mpLandscape=new D3DLandscape();
	mpLandscape->fullGrid(LANDSCAPECENTER, 7);

	ShowWindow(mhWindow, nShowFlags);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	cleanupDirect3D();
	return 0;
}



IOsystem::Camera::Camera() 
{
	position=D3DXVECTOR3(0, 0, 0);
	axis[0]=D3DXVECTOR3(0, 1, 0);
	axis[1]=D3DXVECTOR3(0, 0, 1);
	axis[2]=D3DXVECTOR3(1, 0, 0);
}
		/** Rolls the selected angle around the selected axis
		*	@param axis the axis number 0-2
		*	@param angle the angle of rotation */
void IOsystem::Camera::roll(int axisNr, float angle) 
{
	D3DXMATRIX rollMatrix;
	D3DXMatrixRotationAxis(&rollMatrix, &axis[axisNr%3], angle);
	D3DXVec3TransformCoord(&axis[(axisNr+1)%3], &axis[(axisNr+1)%3], &rollMatrix);
	D3DXVec3TransformCoord(&axis[(axisNr+2)%3], &axis[(axisNr+2)%3], &rollMatrix);
}

		/** Moves the camera along the selected axis.
		*	@param axis the axis to move along
		*	@param step how far to move */
void IOsystem::Camera::move(int axisNr, float step)
{
	D3DXVECTOR3 moveVect;
	D3DXVec3Scale(&moveVect, &axis[axisNr%3], step);
	D3DXVec3Add(&position, &position, &moveVect);
}

void IOsystem::Camera::getViewMatrix(D3DXMATRIX& viewMat)
{
	viewMat._11 = axis[0].x; viewMat._12 = axis[1].x; viewMat._13 = axis[2].x;
	viewMat._21 = axis[0].y; viewMat._22 = axis[1].y; viewMat._23 = axis[2].y;
	viewMat._31 = axis[0].z; viewMat._32 = axis[1].z; viewMat._33 = axis[2].z;
	viewMat._41 = - D3DXVec3Dot( &position, &axis[0] );
	viewMat._42 = - D3DXVec3Dot( &position, &axis[1] );
	viewMat._43 = - D3DXVec3Dot( &position, &axis[2] );
	viewMat._44 = 1.0;
}
void IOsystem::Camera::axisReset()
{
	axis[0]=D3DXVECTOR3(0, 1, 0);
	axis[1]=D3DXVECTOR3(0, 0, 1);
	axis[2]=D3DXVECTOR3(1, 0, 0);
}

LRESULT WINAPI gEventHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return ioSystem.eventHandler(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR ComandLine, int nShowFlags)
{	
	return ioSystem.runIOsystem(hInstance, nShowFlags);
}