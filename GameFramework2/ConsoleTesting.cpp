#include <Windows.h>
#include "ThreadSystemWin.h"
#include "ActionProtocol.h"
#include "SafeStatisticsLogger.h"

HWND editInput;

LRESULT CALLBACK mainWindowProc(HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DESTROY) PostQuitMessage(0);
	if (uMsg == WM_COMMAND) {
		if (lParam > 0 && HIWORD(wParam) == BN_CLICKED) {
			TCHAR dummy[100];
			GetWindowText(editInput, dummy, 100);
			SetWindowText(editInput, TEXT(""));
			wcout << dummy << endl;
		}
	}
	return DefWindowProc(window, uMsg, wParam, lParam);
}

//#define NO_CONSOLLE_HERE

#ifdef NO_CONSOLLE_HERE
//#pragma comment( linker, "/subsystem:windows" )
int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
#else
//#pragma comment( linker, "/subsystem:console" )
int main() {
	HINSTANCE hInst = GetModuleHandle(0);
#endif

	WNDCLASS mainWindClass;
	memset(&mainWindClass, 0, sizeof(WNDCLASS));
	mainWindClass.lpfnWndProc = mainWindowProc;
	mainWindClass.hInstance = hInst;
	mainWindClass.lpszClassName = TEXT("Game Testing System");
	RegisterClass(&mainWindClass);

	HWND mainWindow = CreateWindow(TEXT("Game Testing System"), TEXT("Controll Forward Test"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInst, NULL);
	HWND submitButton = CreateWindow(TEXT("BUTTON"), TEXT("submit"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 250, 200, 100, 20, mainWindow,(HMENU) 1, hInst, NULL);
	editInput = CreateWindow(TEXT("EDIT"), TEXT("edit"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 150, 300, 30, mainWindow, (HMENU) 2, hInst, NULL);

	IThreadSystem* threadSys = new ThreadSystemWin();
	threadSys->mainThread("mainThread");
	int loggerID = threadSys->startThread("loggerThread", new LoggerThread());
	if (loggerID >= 0) {
		LOG_INFO("Enabeling thread system for logging");
		StatisticsLogger::defLogger.enableThreading(threadSys, loggerID);
		LOG_INFO("Thread system running");
	} else {
		LOG_WARNING("Log thread system thread creation failed. Runing on unsafe fallback system.");
	}

	MSG msg;
	int bret;
	while ((bret = GetMessage(&msg, NULL, 0, 0) != 0)) {
		if (bret == -1) {
			threadSys->broadcast(new QuitMessage());
			return 100;
		}
		if (!IsDialogMessage(mainWindow, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	StatisticsLogger::defLogger.printout();
//	cout << endl << "Press key pluss enter to end: ";
//	char dummy;
//	cin >> dummy;

	threadSys->broadcast(new QuitMessage());

	system("pause");

	return msg.wParam;
}