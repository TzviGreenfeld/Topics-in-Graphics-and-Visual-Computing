#include "main.h"
#include "Triangle.h"
bool pickingMode = FALSE;

vector<Triangle *> triangles;
vector<Triangle *> lowRestriangles;
vector<int> pickedTriangles;



void initTriangles(int stepSize)
{

	// delete old triangles
	/*
	for (unsigned int i = 0; i < triangles.size(); i++) {
		delete triangles[i];
	}
	*/
	int cols = heightMap.cols;
	int rows = heightMap.rows;
	int id = 0;
	for (int x = 0; x < (rows - stepSize); x += stepSize)
	{
		for (int y = 0; y < (cols - stepSize); y += stepSize)
		{
			Triangle *t1 = new Triangle(x, y);
			t1->setID(id);
			Triangle *t2 = t1->getAdjecentTriangle();
			t2->setID(id + 1);

			triangles.push_back(t1);
			triangles.push_back(t2);

			id += 2;

			t1->debug = TRUE;
			t2->debug = TRUE;
		}
	}
}

void picking(int x, int y) {
	/*x, y: xmouse position when clicek */

	// Switch to picking mode
	pickingMode = TRUE;
	GLint buffer;
	glGetIntegerv(GL_DRAW_BUFFER, &buffer);
	if (buffer == GL_FRONT)
	{
		// Currently rendering to the front buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// draw unique colors
	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		triangles[i]->drawIdColor();
	}

	// adjust y value so the origin is bottom left instead of top left
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int	modifiedY = viewport[3] - mouseY;

	// read the pixel color under the mouse cursor
	unsigned char pixel[3];
	glReadPixels(x, modifiedY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

	//printf("Pixel values: %d %d %d\n", pixel[0], pixel[1], pixel[2]);

	// look up the triangle id corresponding to the pixel color
	int triangleID = Triangle::getTriangleID(pixel[0], pixel[1], pixel[2]);

	if (triangleID >= 0 && triangleID < triangles.size())
	{
		// do something with picked trinagle here
		triangles[triangleID]->pick();
		pickedTriangles.push_back(triangleID);

		if (pickedTriangles.size() == 2) {
			vector<int> path = graph->BFS(pickedTriangles[0], pickedTriangles[1]);
			for (int node : path) {
				triangles[node]->paint();
			}
			pickedTriangles.clear();
		}

		printf("picked id = (%d)\n", triangleID);
		// paint neighbors as well
		for (int neighbor : graph->getNeighbors(triangleID)) {

			//triangles[neighbor]->pick();
		}
	}

}

void render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if (pause) {
		int x;
		printf("Enter Triangle id: ");
		scanf("%d", &x);
		triangles[x]->paint();
	}

	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		if (triangles[i])
			triangles[i]->draw();
	}

	if (pickingMode)
	{
		pickingMode = FALSE;
	}
	SwapBuffers(hDC);
}

int DrawGLScene(GLvoid) // Here's Where We Do All The Drawing
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(212, 60, 195,	  // eye position
		185, 55, 170,	  // reference point position
		0.0, 1.0, 0.0); // up vector direction
	glScalef(scaleValue, scaleValue * HEIGHT_RATIO, scaleValue);
	glTranslatef(MAP_WIDTH / 2, 0.0, MAP_HEIGHT / 2);
	drawAxis();
	//TODO: make the transformation by map wisth and height
	glTranslatef(0, 10.0 * upDownTransorm, 0); // up/down
	glRotatef(leftRightRotate * 40, 0, 10, 0); // rotate with keyboard
	glRotatef(diagonalRotate * 40, -1, 0, 1); // rotate with keyboard
	glTranslatef(-MAP_WIDTH / 2, 0.0, -MAP_HEIGHT / 2);

	render();


	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance,		// Instance
	HINSTANCE hPrevInstance, // Previous Instance
	LPSTR lpCmdLine,			// Command Line Parameters
	int nCmdShow)			// Window Show State
{
	MSG msg;		   // Windows Message Structure
	BOOL done = FALSE; // Bool Variable To Exit Loop

	// init
	heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\Islands_of_the_Sentinel.png", IMREAD_COLOR);
	//heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\Heightmap.png", IMREAD_COLOR);
	//heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\4kHmap.png", IMREAD_COLOR);
	MAP_WIDTH = heightMap.cols;
	MAP_HEIGHT = heightMap.rows;


	fullscreen = FALSE; // Windowed Mode

	bool printing = TRUE;
	if (printing)
	{
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}

	initTriangles(STEP_SIZE);
	graph = new Graph(heightMap.rows, heightMap.cols, STEP_SIZE);
	//printf("map dimensions: %d, %d", heightMap.rows, heightMap.cols);
	printf("triangles.size=%d\n", triangles.size());



	if (!CreateGLWindow((char *)"mini_project", WIDTH, HEIGHT, 16, fullscreen))
	{
		return 0; // Quit If Window Was Not Created
	}

	while (!done) // Loop That Runs While done=FALSE
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT) // Have We Received A Quit Message?
			{
				done = TRUE; // If So done=TRUE
			}
			else // If Not, Deal With Window Messages
			{
				TranslateMessage(&msg); // Translate The Message
				DispatchMessage(&msg);	// Dispatch The Message
			}
		}
		else // If There Are No Messages
		{

			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE]) // Active?  Was There A Quit Received?
			{
				done = TRUE; // ESC or DrawGLScene Signalled A Quit
			}
			else if (active) // Not Time To Quit, Update Screen
			{
				// SwapBuffers(hDC);       // Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1]) // Is F1 Being Pressed?
			{
				keys[VK_F1] = FALSE;	  // If So Make Key FALSE
				KillGLWindow();			  // Kill Our Current Window
				fullscreen = !fullscreen; // Toggle Fullscreen / Windowed Mode
										  // Recreate Our OpenGL Window
				if (!CreateGLWindow((char *)"NeHe & Ben Humphrey's Height Map Tutorial", 640, 480, 16, fullscreen))
				{
					return 0; // Quit If Window Was Not Created
				}
			}

			// keys / mouse events
			bool anyKeyPressed = any_of(keys, keys + 256, [](bool b) { return b; });
			if (anyKeyPressed) {
				handleInput();
			}

		}
	}

	// Shutdown
	KillGLWindow();
	return (msg.wParam);
}