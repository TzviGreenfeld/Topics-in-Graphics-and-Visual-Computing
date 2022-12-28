#include <windows.h>
#include <gl\glu.h> 
#include <GL\glut.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#include "Graph.h"




#pragma comment(lib, "opengl32.lib") 
#pragma comment(lib, "glu32.lib")  
#pragma warning(disable : 4996)

using namespace std;
using namespace cv;

#define _CRT_SECURE_NO_DEPRECATE

HDC hDC = NULL;      // Private GDI Device Context
HGLRC hRC = NULL;    // Permanent Rendering Context
HWND hWnd = NULL;    // Holds Our Window Handle
HINSTANCE hInstance; // Holds The Instance Of The Application

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Declaration For WndProc

bool active = TRUE; // Window Active Flag Set To TRUE By Default
bool fullscreen = TRUE; // Fullscreen Flag Set To TRUE By Default
bool bRender = FALSE;   // Polygon Flag Set To TRUE By Default ( NEW )

int WIDTH = 640, HEIGHT = 480; // window w, h

// map
int MAP_WIDTH = 0, MAP_HEIGHT = 0; // Size Of Our .RAW Height Map ( NEW )
#define HEIGHT_RATIO 1.5f // Ratio That The Y Is Scaled According To The X And Z ( NEW )
int STEP_SIZE = 5;
Mat heightMap;

// key control
bool keys[256];      // Array Used For The Keyboard Routine
#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

#define VK_Q 0x51
#define VK_E 0x45

// left mouse botton
bool leftmouse = FALSE;
bool rightMouseDown = FALSE;
int mouseX = 0, mouseY = 0;

// right mouse botton
POINT lastMousePos;
int rightMouse_dx, rightMouse_dy;
bool pause = FALSE;

// rotation and transformation
float scaleValue = 0.25f;
float leftRightRotate = 0.50f;
float diagonalRotate = 0.005f;
float upDownTransorm = 0.25f;

// forward declaration
void picking(int x, int y);
//void rotateScene(int dx, int dy);

// 20%
Graph *graph;

bool debug = FALSE;

GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // Resize And Initialize The GL Window
{
	if (height == 0) // Prevent A Divide By Zero By
	{
		height = 1; // Making Height Equal One
	}

	glViewport(0, 0, width, height); // Reset The Current Viewport

	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	glLoadIdentity();			 // Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 500.0f);

	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	glLoadIdentity();			// Reset The Modelview Matrix
}

int InitGL(GLvoid)
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return TRUE;

	// init text library

}

GLvoid KillGLWindow(GLvoid) // Properly Kill The Window
{
	if (fullscreen) // Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0); // If So Switch Back To The Desktop
		ShowCursor(TRUE);				// Show Mouse Pointer
	}

	if (hRC) // Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL)) // Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC)) // Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL; // Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC)) // Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL; // Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd)) // Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL; // Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance)) // Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL; // Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint PixelFormat;				  // Holds The Results After Searching For A Match
	WNDCLASS wc;					  // Windows Class Structure
	DWORD dwExStyle;				  // Window Extended Style
	DWORD dwStyle;					  // Window Style
	RECT WindowRect;				  // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;		  // Set Left Value To 0
	WindowRect.right = (long)width;	  // Set Right Value To Requested Width
	WindowRect.top = (long)0;		  // Set Top Value To 0
	WindowRect.bottom = (long)height; // Set Bottom Value To Requested Height

	fullscreen = fullscreenflag; // Set The Global Fullscreen Flag

	hInstance = GetModuleHandle(NULL);			   // Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;			   // WndProc Handles Messages
	wc.cbClsExtra = 0;							   // No Extra Window Data
	wc.cbWndExtra = 0;							   // No Extra Window Data
	wc.hInstance = hInstance;					   // Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);		   // Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	   // Load The Arrow Pointer
	wc.hbrBackground = NULL;					   // No Background Required For GL
	wc.lpszMenuName = NULL;						   // We Don't Want A Menu
	wc.lpszClassName = "OpenGL";				   // Set The Class Name
	// glutMouseFunc(MouseCB);

	if (!RegisterClass(&wc)) // Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	if (fullscreen) // Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;					// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;					// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE; // Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
				return FALSE; // Return FALSE
			}
		}
	}

	if (fullscreen) // Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW; // Window Extended Style
		dwStyle = WS_POPUP;			 // Windows Style
		ShowCursor(FALSE);			 // Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;					// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |				// Required Window Style
		WS_CLIPCHILDREN,				// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left, // Calculate Window Width
		WindowRect.bottom - WindowRect.top, // Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow(); // Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	static PIXELFORMATDESCRIPTOR pfd = // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
		1,							   // Version Number
		PFD_DRAW_TO_WINDOW |		   // Format Must Support Window
			PFD_SUPPORT_OPENGL |	   // Format Must Support OpenGL
			PFD_DOUBLEBUFFER,		   // Must Support Double Buffering
		PFD_TYPE_RGBA,				   // Request An RGBA Format
		bits,						   // Select Our Color Depth
		0, 0, 0, 0, 0, 0,			   // Color Bits Ignored
		0,							   // No Alpha Buffer
		0,							   // Shift Bit Ignored
		0,							   // No Accumulation Buffer
		0, 0, 0, 0,					   // Accumulation Bits Ignored
		16,							   // 16Bit Z-Buffer (Depth Buffer)
		0,							   // No Stencil Buffer
		0,							   // No Auxiliary Buffer
		PFD_MAIN_PLANE,				   // Main Drawing Layer
		0,							   // Reserved
		0, 0, 0						   // Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd))) // Did We Get A Device Context?
	{
		KillGLWindow(); // Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow(); // Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd)) // Are We Able To Set The Pixel Format?
	{
		KillGLWindow(); // Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC))) // Are We Able To Get A Rendering Context?
	{
		KillGLWindow(); // Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC)) // Try To Activate The Rendering Context
	{
		KillGLWindow(); // Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);	  // Show The Window
	SetForegroundWindow(hWnd);	  // Slightly Higher Priority
	SetFocus(hWnd);				  // Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height); // Set Up Our Perspective GL Screen

	if (!InitGL()) // Initialize Our Newly Created GL Window
	{
		KillGLWindow(); // Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE; // Return FALSE
	}

	return TRUE; // Success
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) // Check For Windows Messages
	{
	case WM_ACTIVATE: // Watch For Window Activate Message
	{
		if (!HIWORD(wParam)) // Check Minimization State
		{
			active = TRUE;
		}
		else
		{
			active = FALSE;
		}

		return 0;
	}

	case WM_SYSCOMMAND: // Intercept System Commands
	{
		switch (wParam)
		{
		case SC_SCREENSAVE:	  // Screensaver Trying To Start?
		case SC_MONITORPOWER: // Monitor Trying To Enter Powersave?
			return 0;		  // Prevent From Happening
		}
		break; // Exit
	}

	case WM_CLOSE: // Did We Receive A Close Message?
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_LBUTTONDOWN: // Did We Receive A Left Mouse Click?
	{

		leftmouse = TRUE;
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		ScreenToClient(hWnd, &cursorPos);
		mouseX = cursorPos.x;
		mouseY = cursorPos.y;

		picking(mouseX, mouseY);

		bRender = !bRender; // Change Rendering State Between Fill/Wire Frame
		return 0;			// Jump Back
	}
	case WM_LBUTTONUP:
	{
		leftmouse = FALSE;
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		// positive delta means the mousewheel rolled up, negative means down
		if (delta > 0)
		{
			// zoom in
			scaleValue += 0.01f;
		}
		else if (delta < 0)
		{
			// zoom out
			scaleValue -= 0.01f;
		}

		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		pause = TRUE;

	}
	case WM_RBUTTONUP:
	{
		pause = FALSE;

	}
	case WM_MOUSEMOVE:
	{
		if (rightMouseDown)
		{
			// Right mouse button is down, update the rotation based on the mouse movement
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			ScreenToClient(hWnd, &cursorPos);
			rightMouse_dx = cursorPos.x - lastMousePos.x;
			rightMouse_dy = cursorPos.y - lastMousePos.y;

			// Update the last mouse position
			lastMousePos = cursorPos;
		}
		return 0;
	}
	case WM_KEYDOWN: // Is A Key Being Held Down?
	{
		keys[wParam] = TRUE; // If So, Mark It As TRUE
		return 0;
	}

	case WM_KEYUP: // Has A Key Been Released?
	{
		keys[wParam] = FALSE;
		return 0;
	}

	case WM_SIZE: // Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam)); // LoWord=Width, HiWord=Height
		return 0;
	}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void drawAxis()
{
	// Draw the x-axis in red
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(700.0, 0.0, 0.0);
	glEnd();

	// Draw the y-axis in green
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 150.0, 0.0);
	glEnd();

	// Draw the z-axis in blue
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 700.0);
	glEnd();
}

void handleInput()
{
	// wasd for rotation
	if (keys[VK_A])
		leftRightRotate += 0.01f;

	if (keys[VK_D])
		leftRightRotate -= 0.01f;

	if (keys[VK_W])
		upDownTransorm += 0.05f;

	if (keys[VK_S])
		upDownTransorm -= 0.05f;

	// diagonal rotation
	if (keys[VK_Q])
		diagonalRotate += 0.01f;

	if (keys[VK_E])
		diagonalRotate -= 0.01f;

	// up/down arrows or mousewheel for zoom
	if (keys[VK_UP])
		scaleValue += 0.001f;

	if (keys[VK_DOWN])
		scaleValue -= 0.001f;


}




