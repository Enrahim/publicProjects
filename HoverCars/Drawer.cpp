#include "Drawer.h"
#include "GameState.h"

void Drawer::initiate(HWND windowHandle) {
	mpD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if(!mpD3D) return;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.hDeviceWindow=windowHandle;
//	d3dpp.EnableAutoDepthStencil = TRUE;
//	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	HRESULT result;
	if (FAILED(result=mpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		windowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &mpD3DDevice )))
	{
		if (FAILED(result=mpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			windowHandle, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &mpD3DDevice )))
		{
			mpD3D->Release();
			mpD3D=NULL;
		}
	}
//	mpD3DDevice->SetRenderState(D3DRS_COLORVERTEX, true);
//	mpD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	mpD3DDevice->SetRenderState(D3DRS_LIGHTING,false);
//	mpD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	mpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mpD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	D3DXMatrixPerspectiveLH(&mProjectionMatrix, 2, 1, 1, 4000);
	D3DXMatrixLookAtLH(&mViewMatrix, &D3DXVECTOR3(10,10,-10), &D3DXVECTOR3(0,0,0),&D3DXVECTOR3(0,1,0));

	mInitiated=true;
}

void Drawer::shutdown() {
	mpD3DDevice->Release();
	mpD3DDevice=NULL;
	mpD3D->Release();
	mpD3D=NULL;
	mInitiated=false;
}

void CarGameDrawer::initiate(HWND window) {
	Drawer::initiate(window);
	if(!initiated()) return;
	if(FAILED(D3DXCreateBox(mpD3DDevice, 2, 2, 2, &mpCarMesh, NULL))){
		Drawer::shutdown();
	}

	if(FAILED(D3DXCreateBox(mpD3DDevice, 1, 1, 1, &mpEngineMesh, NULL))){
		Drawer::shutdown();
	}
}

void CarGameDrawer::drawCar(D3DXMATRIX *worldMatrix) {
	mpD3DDevice->SetTransform(D3DTS_WORLD, worldMatrix);
	mpCarMesh->DrawSubset(0);
}

void CarGameDrawer::drawEngine(D3DXMATRIX *worldMatrix)  {
	mpD3DDevice->SetTransform(D3DTS_WORLD, worldMatrix);
	mpEngineMesh->DrawSubset(0);
}

void CarGameDrawer::drawScene() {
	mpD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	if(!mpD3DDevice->BeginScene()) return;
	mpD3DDevice->SetTransform(D3DTS_VIEW, &mViewMatrix);
	mpD3DDevice->SetTransform(D3DTS_PROJECTION, &mProjectionMatrix);

	gGameState.drawCars();
	
	mpD3DDevice->EndScene();
	mpD3DDevice->Present( NULL, NULL, NULL, NULL );
}