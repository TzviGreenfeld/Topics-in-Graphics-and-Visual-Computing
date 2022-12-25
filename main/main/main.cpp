#include "main.h"
#include "Triangle.h"

bool pickingMode = FALSE;
vector<Triangle*> triangles;

void initTriangles()
{
	int cols = heightMap.cols;
	int rows = heightMap.rows;
	int id = 0;
	for (int x = 0; x < (rows - STEP_SIZE); x += STEP_SIZE) {
		for (int y = 0; y < (cols - STEP_SIZE); y += STEP_SIZE) {
			Triangle* t1 = new Triangle(x, y);
			t1->setID(id);
			Triangle* t2 = t1->getAdjecentTriangle();
			t2->setID(id + 1);

			triangles.push_back(t1);
			triangles.push_back(t2);

			id += 2;
		}
	}
}


void render()
{
	if (leftmouse)
	{
		// Switch to picking mode  

		pickingMode = TRUE;

		GLint buffer;
		glGetIntegerv(GL_DRAW_BUFFER, &buffer);
		if (buffer == GL_FRONT) {
			// Currently rendering to the front buffer
			//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			SwapBuffers(hDC);
		}
		glDisable(GL_DEPTH_TEST);

		// draw unique colors
		for (unsigned int i = 0; i < triangles.size(); i++) {
			triangles[i]->drawIdColor();
		}

		// adjust y value so the origin is bottom left instead of top left
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		int y = viewport[3] - mouseY;

		// read the pixel color under the mouse cursor
		unsigned char pixel[3];
		glReadPixels(mouseX, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
			
		printf("Pixel values: %d %d %d\n", pixel[0], pixel[1], pixel[2]);

		// look up the triangle id corresponding to the pixel color
		int triangleID = Triangle::getTriangleID(pixel[0], pixel[1], pixel[2]);

		if (triangleID >= 0 && triangleID < triangles.size()) {
			// do something with picked trinagle here
			triangles[triangleID]->pick();
			printf("Triangle (%d)->hit=%d\n", triangleID, triangles[triangleID]->hit);
		}

		// return to front buffer
		//SwapBuffers(hDC);   
		
	}
	else
	{
		// Switch to normal rendering mode
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
	}

	for (unsigned int i = 0; i < triangles.size(); i++) {
		triangles[i]->draw();
	}

	if (pickingMode) {
		pickingMode = FALSE;
	}
	SwapBuffers(hDC);

	/*
	// delete triangles
	for (unsigned int i = 0; i < triangles.size(); i++) {
		delete triangles[i];
	}
	*/
}



int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(212, 60, 195,  // eye position
			185, 55, 170,  // reference point position
			0.0, 1.0, 0.0); // up vector direction
	glScalef(scaleValue, scaleValue*HEIGHT_RATIO, scaleValue);

	glTranslatef(256, 0.0, 256);
	drawAxis();
	glRotatef(leftRightRotate * 40, 0, 10, 0);
	glRotatef(upDownRotate * 10, -10, 0, 10);
	glTranslatef(-256, 0.0, -256);

	render();

	return TRUE;
}


int WINAPI WinMain(HINSTANCE   hInstance,      // Instance
	HINSTANCE   hPrevInstance,      // Previous Instance
	LPSTR       lpCmdLine,      // Command Line Parameters
	int     nCmdShow)       // Window Show State
{
	MSG     msg;                    // Windows Message Structure
	BOOL    done = FALSE;                 // Bool Variable To Exit Loop

	// init
	heightMap = imread("G:\\My Drive\\uni\\Topics-in-Graphics-and-Visual-Computing\\main\\Islands_of_the_Sentinel.png", IMREAD_COLOR);
	initTriangles();
	
	fullscreen = FALSE;               // Windowed Mode

	bool printing = TRUE;
	if (printing){
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}


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
			
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])  // Active?  Was There A Quit Received?
			{
				done = TRUE;          // ESC or DrawGLScene Signalled A Quit
			}
			else if (active)            // Not Time To Quit, Update Screen
			{
				//SwapBuffers(hDC);       // Swap Buffers (Double Buffering)
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
			/*
			if (leftmouse) {

				cout << mouseX;
				cout << " ";
				cout << mouseY << endl;

			}
			*/
		}
	}

	// Shutdown
	KillGLWindow();
	return (msg.wParam);
}