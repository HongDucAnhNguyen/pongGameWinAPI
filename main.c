#include <stdio.h>
#include <windows.h>
#include "./main.h"

/*global variables*/
HWND g_Window;
BOOL g_gameIsRunning;
GAMEBITMAP g_backBuffer;
RECT clientRect;
BOOL g_windowInFocus;
PLAYER g_player;
PLAYER g_opponent;
PLAYER g_pongBall;
int clientWindowWidth;
int clientWindowHeight;
int playerSpeed;
int32_t playerScreenX;
int32_t playerScreenY;
int32_t opponentScreenY;
int32_t opponentScreenX;
int32_t pongBallScreenX;
int32_t pongBallScreenY;
BOOL g_windowHasFocus;
BOOL oppMovesUp = TRUE;
BOOL oppMovesDown = FALSE;
BOOL pongBallMovesLeft = TRUE;
BOOL pongBallMovesRight = FALSE;
BOOL pongBallMovesUp = TRUE;
BOOL pongBallMovesDown = FALSE;
FILETIME  timeStarted;
LONGLONG timeStartedMS;


int playerScore;
int oppScore;

/*WPARAM = WORD PARAMETER = a 32-bit value that carries additional
information related to the message being processed
  LPARAM = LONG PARAMETER = 32-bit value that carries additional information
*/
LRESULT CALLBACK WindowProc(HWND WindowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam);
//function to render game
void RenderGraphics(void);
void ProcessPlayerInput(void);
void setClientRect(void);
void SetDIBSection(void);
void DrawPlayer(int32_t StartingPixel, int32_t characterWidth, int32_t characterHeight);
BOOL GameIsAlreadyRunning(void);
void OpponentMoves(void);
void pongBallMoves(void);
void checkPlayerCollision(void);
void checkEdgeCollision(void);


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
	if (GameIsAlreadyRunning() == TRUE) {

		MessageBox(NULL, L"Another instance of Game is already running", L"Game is already running", MB_OK);
		return 0;
	}
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
			CW_USEDEFAULT, CW_USEDEFAULT, 600, 500,

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
		setClientRect();
		SetDIBSection();



	}
	g_player.WorldPositionX = 25;
	g_player.WorldPositionY = 25;
	g_player.characterHeight = 16;
	g_player.characterWidth = 5;



	g_opponent.WorldPositionX = BITMAP_WIDTH - 25;
	g_opponent.WorldPositionY = 25;
	g_opponent.characterHeight = 16;
	g_opponent.characterWidth = 5;

	g_pongBall.WorldPositionX = BITMAP_WIDTH / 2;
	g_pongBall.WorldPositionY = 35;
	g_pongBall.characterHeight = 5;
	g_pongBall.characterWidth = 5;


	//check if allocation failed
	if (g_backBuffer.Memory == NULL) {
		MessageBox(NULL, L"Error allocating memory", L"Memory Allocation Error", MB_OK | MB_ICONERROR);

		return 0;
	}


	double lastFrameTime = 0.0;


	/*Message loop to keep window running*/

	MSG Message;
	while (g_gameIsRunning && PeekMessage(&Message, g_Window, 0, 0, PM_REMOVE) > 0) {
		DispatchMessageA(&Message);
		ProcessPlayerInput();
		RenderGraphics();
		Sleep(1);
	}


	return EXIT_SUCCESS;
}




BOOL GameIsAlreadyRunning(void) {
	/*A mutex (short for "mutual exclusion") is a synchronization
	primitive used in computer programming to prevent multiple threads
	or processes from accessing shared resources simultaneously.
	The primary purpose of a mutex is to ensure that only one thread or
	process can access a critical section of code or a shared resource
	at any given time. This prevents race conditions and maintains data integrity.*/

	//A handle datatype is a vague value that serves as a reference to any obj
	//that is managed by the Windows operating system
	HANDLE Mutex = NULL;
	Mutex = CreateMutex(NULL, NULL, L"GameMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return TRUE;
	}
	else return FALSE;
}



//procedure identifier
LRESULT CALLBACK WindowProc(HWND WindowHandle, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT Result = 0;

	switch (uMsg) {

	case WM_SIZE: {
		setClientRect();
		SetDIBSection();
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

void setClientRect(void) {
	//retrieve initial window dimensions
	GetClientRect(g_Window, &clientRect);
	clientWindowWidth = clientRect.right - clientRect.left;
	clientWindowHeight = clientRect.bottom - clientRect.top;
}

void SetDIBSection(void) {
	if (g_backBuffer.Memory) {
		VirtualFree(g_backBuffer.Memory, 0, MEM_RELEASE);
	}
	//biSize specifies the size in bytes needed for the BMIHEADERINFO struct
	g_backBuffer.BitmapInfo.bmiHeader.biSize = sizeof(g_backBuffer.BitmapInfo.bmiHeader);
	g_backBuffer.BitmapInfo.bmiHeader.biWidth = BITMAP_WIDTH;
	g_backBuffer.BitmapInfo.bmiHeader.biHeight = BITMAP_HEIGHT;
	g_backBuffer.BitmapInfo.bmiHeader.biBitCount = GAME_BITSPERPIXEL;

	g_backBuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;
	g_backBuffer.BitmapInfo.bmiHeader.biPlanes = 1;

	//allocate memory and store the pointer of the bitmap or canvas
	g_backBuffer.Memory =
		VirtualAlloc(NULL, GAME_DRAWING_AREA_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}



void RenderGraphics(void) {

	//drawing backbuffer to window

	//get a handle to a device context that is compatible with the game window
	HDC DeviceContext = GetDC(g_Window);

	//create and configure window pixel
	PIXEL32 WindowPixel = { 0 };
	WindowPixel.Blue = 0;
	WindowPixel.Green = 0;
	WindowPixel.Red = 0;
	WindowPixel.Alpha = 0;

	//fill up bitmap region with configuration of Pixel structure, bit by bit
	for (int i = 0; i < BITMAP_WIDTH * BITMAP_HEIGHT; i++) {
		/*memcpy or memcpy_s are both functions to copy bytes from buffer to buffer
		however, memcpy_s is more secure because it needs the programmer to specify the size of the
		destination buffer explicitly

		errno_t memcpy_s(void *dest, rsize_t destsz, const void *src, rsize_t count);

		dest: A pointer to the destination memory location where the data will be copied.

		destsz: The size of the destination buffer, in bytes.

		src: A pointer to the source memory location from which the data will be copied.

		count: The number of bytes to copy from the source to the destination.


		*/
		memcpy_s((PIXEL32*)g_backBuffer.Memory + i, sizeof(PIXEL32), &WindowPixel, sizeof(PIXEL32));

	}



	//set screen position for player
	playerScreenX = g_player.WorldPositionX;
	playerScreenY = g_player.WorldPositionY;

	opponentScreenY = g_opponent.WorldPositionY;
	opponentScreenX = g_opponent.WorldPositionX;

	pongBallScreenX = g_pongBall.WorldPositionX;
	pongBallScreenY = g_pongBall.WorldPositionY;
	/*Explanation for the expression to determine the starting pixel:

	in winapi, if bitmap_height is a positive value, the bitmap is drawn bottom left to right all the way up
	bitmapheight x bitmapwidth = area of bitmap rectangle

	- bitmapwidth => jump one row from top of window

	- bitmapwidth x screenY => remove more rows, according to the starting Y position, in this case, jump 25 more rows from top

	+ screenX => move player horizontally to position X on the window

	==> coordinate is now 25,25 if we count it as starting from the top of the window
	*/

	int32_t playerStartingScreenPixel = ((BITMAP_HEIGHT * BITMAP_WIDTH) - BITMAP_WIDTH - BITMAP_WIDTH *
		playerScreenY) + playerScreenX;

	int32_t oppStartingScreenPixel = ((BITMAP_HEIGHT * BITMAP_WIDTH) - BITMAP_WIDTH - BITMAP_WIDTH *
		opponentScreenY + opponentScreenX);

	int32_t pongBallStartingScreenPixel = ((BITMAP_HEIGHT * BITMAP_WIDTH) - BITMAP_WIDTH - BITMAP_WIDTH * pongBallScreenY) + pongBallScreenX;


	//draw player
	DrawPlayer(playerStartingScreenPixel, g_player.characterWidth, g_player.characterHeight);

	//draw opponent
	DrawPlayer(oppStartingScreenPixel, g_opponent.characterWidth, g_opponent.characterHeight);
	DrawPlayer(pongBallStartingScreenPixel, g_pongBall.characterWidth, g_pongBall.characterHeight);



	StretchDIBits(DeviceContext, 0, 0, clientWindowWidth, clientWindowHeight, 0, 0, BITMAP_WIDTH, BITMAP_HEIGHT,
		g_backBuffer.Memory, &g_backBuffer.BitmapInfo, DIB_RGB_COLORS, SRCCOPY
	);

	char ScoreTextBuffer[64] = { 0 };
	sprintf_s(ScoreTextBuffer, _countof(ScoreTextBuffer), "Score: %d - %d", playerScore, oppScore);
	TextOutA(DeviceContext, BITMAP_WIDTH / 2.0, 25, ScoreTextBuffer, (int)strlen(ScoreTextBuffer));
	ReleaseDC(g_Window, DeviceContext);

}







void DrawPlayer(int32_t StartingScreenPixel, int32_t characterWidth, int32_t characterHeight) {
	for (int32_t y = 0; y < characterHeight; y++) {
		for (int32_t x = 0; x < characterWidth; x++) {
			memset((PIXEL32*)g_backBuffer.Memory + StartingScreenPixel + x - (BITMAP_WIDTH * y), 0xFF, sizeof(PIXEL32));


		}
	}
}

void OpponentMoves(void) {

	if (g_pongBall.WorldPositionY < g_opponent.WorldPositionY) {
		oppMovesUp = TRUE;
		oppMovesDown = FALSE;
	}
	if (g_pongBall.WorldPositionY > g_opponent.WorldPositionY) {
		oppMovesDown = TRUE;
		oppMovesUp = FALSE;

	}

	if (oppMovesUp) {
		if (g_opponent.WorldPositionY > 0) {

			g_opponent.WorldPositionY--;



		}
		

	}

	if (oppMovesDown) {
		if (g_opponent.WorldPositionY < BITMAP_HEIGHT - g_opponent.characterHeight) {

			g_opponent.WorldPositionY++;



		}
		
	}





}

void pongBallMoves(void) {


	checkEdgeCollision();
	checkPlayerCollision();

}


void checkEdgeCollision(void) {


	if (pongBallMovesLeft && pongBallMovesUp) {

		if (g_pongBall.WorldPositionY == 0) {
			pongBallMovesDown = TRUE;
			pongBallMovesUp = FALSE;
		}

		if (g_pongBall.WorldPositionX == 0) {
			oppScore++;
			g_pongBall.WorldPositionX = BITMAP_WIDTH / 2;
			g_pongBall.WorldPositionY = 35;
			pongBallMovesRight = TRUE;
			pongBallMovesLeft = FALSE;
		}

		g_pongBall.WorldPositionX--;
		g_pongBall.WorldPositionY--;


	}
	if (pongBallMovesLeft && pongBallMovesDown) {

		if (g_pongBall.WorldPositionX > 0 && g_pongBall.WorldPositionY == BITMAP_HEIGHT - (g_pongBall.characterHeight + 2)) {
			pongBallMovesUp = TRUE;
			pongBallMovesDown = FALSE;
		}

		if (g_pongBall.WorldPositionX == 0) {
			oppScore++;
			g_pongBall.WorldPositionX = BITMAP_WIDTH / 2;
			g_pongBall.WorldPositionY = 35;
			pongBallMovesUp = TRUE;
			pongBallMovesRight = TRUE;
			pongBallMovesLeft = FALSE;
			pongBallMovesDown = FALSE;
		}

		g_pongBall.WorldPositionX--;
		g_pongBall.WorldPositionY++;


	}

	if (pongBallMovesRight && pongBallMovesUp) {

		if (g_pongBall.WorldPositionY == 0) {
			pongBallMovesDown = TRUE;
			pongBallMovesUp = FALSE;
		}
		if (g_pongBall.WorldPositionX == BITMAP_WIDTH - g_pongBall.characterWidth) {
			playerScore++;
			g_pongBall.WorldPositionX = BITMAP_WIDTH / 2;
			g_pongBall.WorldPositionY = 35;

			pongBallMovesLeft = TRUE;
			pongBallMovesRight = FALSE;
		}

		g_pongBall.WorldPositionX++;
		g_pongBall.WorldPositionY--;

	}
	if (pongBallMovesRight && pongBallMovesDown) {

		if (g_pongBall.WorldPositionX == BITMAP_WIDTH - g_pongBall.characterWidth) {

			playerScore++;
			g_pongBall.WorldPositionX = BITMAP_WIDTH / 2;
			g_pongBall.WorldPositionY = 35;
			pongBallMovesLeft = TRUE;
			pongBallMovesUp = TRUE;
			pongBallMovesDown = FALSE;
			pongBallMovesRight = FALSE;
		}
		if (g_pongBall.WorldPositionX < BITMAP_WIDTH - g_pongBall.characterWidth && g_pongBall.WorldPositionY
			== BITMAP_HEIGHT - (g_pongBall.characterHeight + 2)) {
			pongBallMovesUp = TRUE;
			pongBallMovesDown = FALSE;
		}


		g_pongBall.WorldPositionX++;
		g_pongBall.WorldPositionY++;




	}











}


void checkPlayerCollision(void) {
	/*check collision

	NOTE: turn into independent func
	*/
	if (pongBallMovesRight) {
		//pong Ball in range
		if (g_pongBall.WorldPositionY >= g_opponent.WorldPositionY - 5 && g_pongBall.WorldPositionY <=
			g_opponent.WorldPositionY + 16
			)
		{
			//front collision

			if (g_pongBall.WorldPositionX + 5 == g_opponent.WorldPositionX) {
				pongBallMovesLeft = TRUE;
				pongBallMovesDown = !pongBallMovesDown;
				pongBallMovesUp = !pongBallMovesUp;

				pongBallMovesRight = FALSE;

			}


		}
	}

	if (pongBallMovesLeft) {

		if (
			(g_pongBall.WorldPositionY >= g_player.WorldPositionY - 5 && g_pongBall.WorldPositionY <=
				g_player.WorldPositionY + 16)) {

			//front collision
			if (g_pongBall.WorldPositionX == g_player.WorldPositionX + 5) {

				pongBallMovesRight = TRUE;
				pongBallMovesDown = TRUE;
				pongBallMovesUp = FALSE;
				pongBallMovesLeft = FALSE;
			}


		}

	}


}


void ProcessPlayerInput(void) {

	short EscapeKeyIsDown = GetAsyncKeyState(VK_ESCAPE);
	short UpKeyIsDown = GetAsyncKeyState(VK_UP) | GetAsyncKeyState('W');
	short DownKeyIsDown = GetAsyncKeyState(VK_DOWN) | GetAsyncKeyState('S');

	pongBallMoves();
	OpponentMoves();
	if (EscapeKeyIsDown) {
		SendMessage(g_Window, WM_CLOSE, 0, 0);
	}
	if (UpKeyIsDown) {
		if (g_player.WorldPositionY > 0) {
			g_player.WorldPositionY--;
		}

	}
	if (DownKeyIsDown) {
		if (g_player.WorldPositionY < BITMAP_HEIGHT - 16) {
			g_player.WorldPositionY++;
		}


	}

}