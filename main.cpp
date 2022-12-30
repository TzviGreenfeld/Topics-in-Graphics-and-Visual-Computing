#include "main.h"
#include "Triangle.h"
#include <chrono>

vector<Triangle *> triangles;
vector<Triangle *> lowRestriangles;
vector<int> pickedTriangles;


void initTriangles(int stepSize)
{

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

	return TRUE;
}


void initScene() {

	heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\Islands_of_the_Sentinel.png", IMREAD_COLOR);
	//heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\Heightmap.png", IMREAD_COLOR);
	//heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\4kHmap.png", IMREAD_COLOR);

	MAP_WIDTH = heightMap.cols;
	MAP_HEIGHT = heightMap.rows;
	initTriangles(STEP_SIZE);
	graph = new Graph(heightMap.rows, heightMap.cols, STEP_SIZE);

	//printf("map dimensions: %d, %d", heightMap.rows, heightMap.cols);
	printf("triangles.size=%d\n", triangles.size());


}


void mainLoop() {
	DrawGLScene();
	render();
	handleInput();
}

