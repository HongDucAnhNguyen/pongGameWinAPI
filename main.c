#include <stdio.h>
#include <windows.h>
#include "./main.h"

/*global variables*/
HWND g_Window;
BOOL g_gameIsRunning;

/*WPARAM = WORD PARAMETER = a 32-bit value that carries additional
information related to the message being processed
  LPARAM = LONG PARAMETER = 32-bit value that carries additional information
*/
LRESULT CALLBACK WindowProc(HWND WindowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam);
//function to render game
void RenderGraphics(void);
void ProcessPlayerInput(void);

//entry func
int WINAPI WinMain(HINSTANCE Instance, HINSTANCE prevInstance,
	PSTR cmdLine, int cmdShow)

{
	//create window class
	WNDCLASS WindowClass = { 0 };
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = Instance;
	WindowClass.style = 0;
	WindowClass.lpszClassName = L"Window Class";

	//register window class, return 0 when failed
	//fail catch
	if (RegisterClass(&WindowClass) == 0) {
		//display error message (no owner window for message box)
		MessageBox(NULL, L"Error registering window class", L"Registration Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	else {
		//registration is successful, create client window
		g_Window = CreateWindowEx(
			WS_EX_CLIENTEDGE,                              // Optional window styles.
			WindowClass.lpszClassName,                     // Window class
			L"Pong with Win32API",    // Window text
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

			NULL,       // Parent window    
			NULL,       // Menu
			Instance,  // Instance handle
			NULL        // Additional application data
		);

		//if window cannot be created
		if (g_Window == NULL) {
			MessageBox(NULL, L"Error creating window", L"Creation Error", MB_OK | MB_ICONERROR);
			return 0;
		}
		ShowWindow(g_Window, SW_SHOWNORMAL);
		g_gameIsRunning = TRUE;


	}



	/*Message loop to keep window running*/

	MSG Message;
	while (g_gameIsRunning && PeekMessage(&Message, g_Window, 0, 0, PM_REMOVE) > 0) {
		DispatchMessageA(&Message);
		ProcessPlayerInput();


	}


	return EXIT_SUCCESS;
}







//procedure identifier
LRESULT CALLBACK WindowProc(HWND WindowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT Result = 0;

	switch (uMsg) {

	case WM_SIZE: {

		break;
	}
	case WM_DESTROY: {
		g_gameIsRunning = FALSE;
		OutputDebugStringA("window message destroyed\n");
		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE: {
		g_gameIsRunning = FALSE;
		OutputDebugStringA("window message closed\n");
		DestroyWindow(WindowHandle);
		PostQuitMessage(0);
		break;
	}
	case WM_ACTIVATE: {

		break;
	}
	case WM_ACTIVATEAPP: {
		OutputDebugStringA("window message activated app\n");
		break;
	}
	case WM_PAINT: {


		break;
	}
	default: {
		Result = DefWindowProc(WindowHandle, uMsg, wParam, lParam);
		break;
	}
	}
	return Result;
}

void RenderGraphics(void) {

	//drawing backbuffer to window

	//get a handle to a device context that is compatible with the game window
	HDC DeviceContext = GetDC(g_Window);


}


void ProcessPlayerInput(void) {

}