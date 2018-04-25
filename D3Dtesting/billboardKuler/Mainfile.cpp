#include "DrawSystem.h"
#include <Windows.h>




int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	DrawSystem drawer;
	int res = drawer.initialize(hInstance);
	if(res>0) {
		drawer.cleanup();
		return res;
	}

	SetTimer(NULL, 1, 40, NULL);
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		drawer.processMessage(msg);
	}

	drawer.cleanup();
	return res;
}