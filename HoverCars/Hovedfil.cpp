#include "common.h"
#include "Drawer.h"
#include "GameState.h"

CarGameDrawer gDrawer;
GameState gGameState;

enum TimerEvents {TICK_EVENT_ID};

/** Standard windows message handler.
*	Requires TICK_EVENT_ID and gameTick() defined */
LRESULT CALLBACK gEventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message) {
		case WM_TIMER:
			if (wParam==TICK_EVENT_ID) gGameState.tick();
			break;
		case WM_DESTROY:
            PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	};
	return 0;
}

/** Helper function to make a standard windows window.
*	Requiuires the global APLICATIONNAME to be defined 
*	as a string with the name of the program 
*	and gEcentHandler as the event handler for the program.
*	@param hInstance The Instance handle of the program
*	@return A handle to the newly createt window */
HWND makewindow(HINSTANCE hInstance) 
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

	return CreateWindow(APLICATIONNAME, APLICATIONNAME, WS_OVERLAPPEDWINDOW, 
		100, 100, 500, 500, GetDesktopWindow(), NULL, hInstance, NULL);	
	
}

/** MainFunction. Requires the folowing defines: TICKTIME, TICK_EVENT_ID,
*	and APLICATIONNAME. trenger en gobal Drawer gDrawer og en global
*	event handler gEventHandler */
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, 
				   LPSTR ComandLine, int nShowFlags)
{
	HWND hWindow=makewindow(hInstance);
	if(!hWindow) return 100;
	gDrawer.initiate(hWindow);
	gGameState.initiate();

	ShowWindow(hWindow, nShowFlags);
	
	SetTimer(hWindow, TICK_EVENT_ID, TICKTIME, NULL);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	gDrawer.shutdown();
	return 0;
}
	

