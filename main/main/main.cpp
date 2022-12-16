#include "main.h"
#include "Triangle.h"


// mouse handling
struct {
	bool IsPressed;
	int x;
	int y;
} m_leftMouseButton;

void MouseCB(int Button, int State, int x, int y)
{
	if (Button == GLUT_LEFT_BUTTON) {
		m_leftMouseButton.IsPressed = (State == GLUT_DOWN);
		m_leftMouseButton.x = x;
		m_leftMouseButton.y = y;
	}
}

void rotate() 
{

	if (keys[VK_A])
		leftRightRotate += 0.01f;

	if (keys[VK_D])
		leftRightRotate -= 0.01f;

	if (keys[VK_W])
		upDownRotate += 0.01f;

	if (keys[VK_S])
		upDownRotate -= 0.01f;

}


void picking()
{
	return;
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(212, 60, 194,
		186, 55, 171,
		0, 1, 0);
	glScalef(scaleValue, scaleValue*HEIGHT_RATIO, scaleValue);

	glTranslatef(256, 0.0, 256);
	drawAxis();
	glRotatef(leftRightRotate * 40, 0, 10, 0);
	glRotatef(upDownRotate * 10, -10, 0, 10);
	glTranslatef(-256, 0.0, -256);
	
	// picking
	//glEnable(GL_DEPTH_TEST);

	vector<Triangle*> triangles;

	int cols = heightMap.cols;
	int rows = heightMap.rows;
	for (int x = 0; x < (rows - STEP_SIZE); x += STEP_SIZE) {
		for (int y = 0; y < (cols - STEP_SIZE); y += STEP_SIZE) {
			Triangle* t1 = new Triangle(x, y);
			Triangle* t2 = t1->getAdjecentTriangle();

			triangles.push_back(t1);
			triangles.push_back(t2);

			
			if (m_leftMouseButton.IsPressed) {
				t1->drawIdColor(2);
				t2->drawIdColor(2);
			}
			else
			{
				t1->draw();
				t2->draw();
			}
		}
	}

	// delete triangles
	for (unsigned int i = 0; i < triangles.size(); i++) {
		delete triangles[i];
	}

	return TRUE;
}




int WINAPI WinMain(HINSTANCE   hInstance,      // Instance
	HINSTANCE   hPrevInstance,      // Previous Instance
	LPSTR       lpCmdLine,      // Command Line Parameters
	int     nCmdShow)       // Window Show State
{
	MSG     msg;                    // Windows Message Structure
	BOOL    done = FALSE;                 // Bool Variable To Exit Loop


										  // Ask The User Which Screen Mode They Prefer
	//if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	//{
	fullscreen = FALSE;               // Windowed Mode
	//}

	// Create Our OpenGL Window
	if (!CreateGLWindow((char*)"mini_project", WIDTH, HEIGHT, 16, fullscreen))
	{
		return 0;                   // Quit If Window Was Not Created
	}

	

	while (!done)                        // Loop That Runs While done=FALSE
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))   // Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)       // Have We Received A Quit Message?
			{
				done = TRUE;          // If So done=TRUE
			}
			else                    // If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);     // Translate The Message
				DispatchMessage(&msg);      // Dispatch The Message
			}
		}
		else                        // If There Are No Messages
		{
			if (m_leftMouseButton.IsPressed) {
				picking();
				glutSwapBuffers();
			}

			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])  // Active?  Was There A Quit Received?
			{
				done = TRUE;          // ESC or DrawGLScene Signalled A Quit
			}
			else if (active)            // Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);       // Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])            // Is F1 Being Pressed?
			{
				keys[VK_F1] = FALSE;      // If So Make Key FALSE
				KillGLWindow();         // Kill Our Current Window
				fullscreen = !fullscreen;     // Toggle Fullscreen / Windowed Mode
											  // Recreate Our OpenGL Window
				if (!CreateGLWindow((char*)"NeHe & Ben Humphrey's Height Map Tutorial", 640, 480, 16, fullscreen))
				{
					return 0;       // Quit If Window Was Not Created
				}
			}

			// keys / mouse events
			if (keys[VK_UP])
				scaleValue += 0.001f;

			if (keys[VK_DOWN])
				scaleValue -= 0.001f;

			if (keys[VK_W] || keys[VK_A] || keys[VK_S] || keys[VK_D]) {
				rotate();
			}
			
		}
	}

	// Shutdown
	KillGLWindow();
	return (msg.wParam);
}