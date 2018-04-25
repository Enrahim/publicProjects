#include "DrawSystem.h"


LRESULT CALLBACK mainWindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

bool DrawSystem::myCreateWindow(HINSTANCE hInst) {
	const char className[] = "D3D Kule Hoved Vindu";
	WNDCLASS wndCls;
	memset(&wndCls, 0, sizeof(WNDCLASS));
	wndCls.lpfnWndProc = mainWindowProc;
	wndCls.hInstance = hInst;
	wndCls.lpszClassName = className;

	ATOM classHandle = RegisterClass(&wndCls);
	if(classHandle == 0) {
		DWORD res = GetLastError();
		return false;
	}

	mainWindow = CreateWindowEx(0, className, className, WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInst, 0);
	if(mainWindow) {
		return true;	
	}
	DWORD res = GetLastError();
	return false;
}

bool DrawSystem::myCreateD3D11() {
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.RefreshRate.Numerator=60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator=1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.OutputWindow = mainWindow;
	swapChainDesc.Windowed = TRUE;

	const D3D_FEATURE_LEVEL acceptable[]={ D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL tmp;
	UINT flags = 0;
#if defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, acceptable, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &tmp, &imidiateContext);
	if(res!=S_OK) {
		cleanup();
		return false;
	}

	return true;
}

bool DrawSystem::myAttachRenderTarget() {
	ID3D11Texture2D *tempBuffer = NULL;
	HRESULT res=swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*) &tempBuffer);
	if(FAILED(res)) {
		cleanup();
		return false;
	}

	res = device->CreateRenderTargetView(tempBuffer, NULL, &renderTarget);
	if(FAILED(res)) {
		tempBuffer->Release();
		cleanup();
		return false;
	}
	tempBuffer->Release();

	return true;
}

bool DrawSystem::myPrepareStensilBuffer() {
	D3D11_RENDER_TARGET_VIEW_DESC  renderDesc;
	renderTarget->GetDesc(&renderDesc);
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	device->CreateTexture2D()
}

int DrawSystem::initialize(HINSTANCE hInst) {
	if (initialized) return -2;
	if(!myCreateWindow(hInst)) return 1;
	if(!myCreateD3D11()) return 2;
	if(!myAttachRenderTarget()) return 3;
	kuler = new KuleAssets(100);
	kuler->sampleGenerate(41000);
	if (!kuler->intializeAssets(device)) return 4;
	if (!camera.initialize(device, 100.f, 10000.f, 0.0016f, 0.0009f)) return 5;

	initialized=true;
	return 0;
}


void DrawSystem::cleanup() {
	if (kuler) {
		delete kuler;
		kuler = NULL;
	}
	safeRelease(imidiateContext);
	safeRelease(renderTarget);
	safeRelease(swapChain);
	safeRelease(device);
	DestroyWindow(mainWindow);
}

void DrawSystem::draw() {
	static const float backgroundColor[] = {0.f, 0.f, 1.f, 1.f};
	HRESULT res;
	imidiateContext->ClearRenderTargetView(renderTarget, backgroundColor );
	res = (swapChain->Present(1, 0));
	if( FAILED(res) ) {
		throw res;
	}
}

void DrawSystem::processMessage(MSG& msg) {
	switch(msg.message) {
	case WM_TIMER:
			draw();
			SetTimer(NULL, 1, 40, NULL);
			break;
	}
	if(msg.hwnd != mainWindow) return;
	switch(msg.message) {
	case WM_SIZE:
		imidiateContext->ClearState();
		safeRelease(renderTarget);
		swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		myAttachRenderTarget();
	case WM_PAINT:
		draw();
		break;
	}
}

void Camera::cleanup() {
	safeRelease(eulerView);
	safeRelease(perspective);
}

bool Camera::initialize(ID3D11Device* dev, float nearP, float farP, float xScale, float yScale) {
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(ViewStruct);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA init = { &cpuView, 0, 0 };

	HRESULT res = dev->CreateBuffer(&bufferDesc, &init, &eulerView);
	if (FAILED(res)) {
		cleanup();
		return false;
	}
	
	float xyzn[4];
	xyzn[0] = xScale;
	xyzn[1] = yScale;
	xyzn[2] = farP / (nearP - farP);
	xyzn[3] = nearP;

	bufferDesc.ByteWidth = sizeof(xyzn);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA init2 = { xyzn, 0, 0 };
	res = dev->CreateBuffer(&bufferDesc, &init2, &perspective);
	if (FAILED(res)) {
		cleanup();
		return false;
	}
	return true;
}