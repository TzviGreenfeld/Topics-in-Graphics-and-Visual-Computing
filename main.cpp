#include "main.h"
#include "Triangle.h"
#include "Graph.h"
#include "Weather.h"
#include "ResMapper.h"
#include <thread>
#include <chrono>

vector<int> pickedTriangles;
vector<Triangle *> triangles;
vector<Triangle *> lowResTriangles;
ResMapper *resMapper;
Graph *lowResGraph;
Graph *highResGraph;
Weather *weather;



void delay(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void handleInput()
{
	// wasd for rotation
	if (keys[VK_A])
		leftRightRotate += 0.01f;

	if (keys[VK_D])
		leftRightRotate -= 0.01f;

	if (keys[VK_W])
		upDownTransorm += 0.5f;

	if (keys[VK_S])
		upDownTransorm -= 0.5f;

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

	// Mesh Refinement with spacebar
	if (keys[VK_R]) {
		delay(100);
		weather->rain();
	}

	if (keys[VK_F]) {
		delay(100);
		weather->addFog();
	}
}

void initTriangles(int stepSize, vector<Triangle *> &currTriangles)
{

	int cols = heightMap.cols;
	int rows = heightMap.rows;
	int id = 0;
	for (int x = 0; x < (rows - stepSize); x += stepSize)
	{
		for (int y = 0; y < (cols - stepSize); y += stepSize)
		{
			Triangle *t1 = new Triangle(x, y, stepSize);
			t1->setID(id);
			Triangle *t2 = t1->getAdjecentTriangle();
			t2->setID(id + 1);

			currTriangles.push_back(t1);
			currTriangles.push_back(t2);

			id += 2;
		}
	}
}



void picking(int x, int y) {
	/*x, y: xmouse position when click */

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

	if (triangleID >= 0 && triangleID < triangles.size()) {
		triangles[triangleID]->pick();
		int largeTriangleId = resMapper->smallToLarge(triangleID);
		printf("small id %d\nlarge id %d\n", triangleID, largeTriangleId);
		if (largeTriangleId >= 0 && largeTriangleId < lowResTriangles.size()) {
			lowResTriangles[largeTriangleId]->pick();
		}

	}

	if (triangleID >= 0 && triangleID < triangles.size())
	{
		// do something with picked trinagle here
		pickedTriangles.push_back(triangleID);

		if (pickedTriangles.size() == 2) {
			// clear previous path
			for (auto& t : triangles) {
				t->unpaint();
			}

			vector<int> path = lowResGraph->dijkstra(resMapper->smallToLarge(pickedTriangles[0]), resMapper->smallToLarge(pickedTriangles[1]));
			//vector<int> path = highResGraph->dijkstra(pickedTriangles[0], pickedTriangles[1]);
			/*vector<int> path;
			for (int j = 0; j < originalPath.size(); j += 2) {
				path.push_back(originalPath[j]);
			} */

			for (int i = 1; i < path.size() + 1; i++) {
				vector<int> highResPath;
				if (i == 0) {
					highResPath = highResGraph->BFS(pickedTriangles[0], resMapper->largeToSmall(path[i]));
				}
				else if (i == path.size()) {
					highResPath = highResGraph->BFS(resMapper->largeToSmall(path[i - 1]), pickedTriangles[1]);
				}
				else {
					highResPath = highResGraph->BFS(resMapper->largeToSmall(path[i - 1]), resMapper->largeToSmall(path[i]));
				}
				for (int bfsNode : highResPath) {
					//printf("%d\n", bfsNode);
					triangles[bfsNode]->paint();
				}
				highResPath.clear();
			}
			pickedTriangles.clear();
			path.clear();

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

	for (unsigned int i = 0; i < triangles.size(); i++)D
	{
		if (triangles[i])
			triangles[i]->draw();
	}
		if (keys[VK_RIGHT]) {
			//if (TRUE) {
			for (unsigned int i = 0; i < lowResTriangles.size(); i++)
			{
				lowResTriangles[i]->drawRedThickOutline();
			}
		}

	if (pickingMode)
	{
		pickingMode = FALSE;
	}

	SwapBuffers(hDC);
	weather->renderRain();
	weather->initFog();

}


void initScene() {

	heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\Islands_of_the_Sentinel.png", IMREAD_COLOR);
	//heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\Heightmap.png", IMREAD_COLOR);
	//heightMap = imread("C:\\Users\\tzvig\\source\\repos\\main\\4kHmap.png", IMREAD_COLOR);

	MAP_WIDTH = heightMap.cols;
	MAP_HEIGHT = heightMap.rows;
	initTriangles(STEP_SIZE, triangles);
	initTriangles(30, lowResTriangles);
	lowResGraph = new Graph(heightMap.rows, heightMap.cols, 30, lowResTriangles);
	highResGraph = new Graph(heightMap.rows, heightMap.cols, STEP_SIZE, triangles);

	resMapper = new ResMapper(triangles, lowResTriangles);

	weather = new Weather;
	weather->initRain();

	//printf("map dimensions: %d, %d", heightMap.rows, heightMap.cols);
	allowPrinting();
	printf("triangles.size=%d\n", triangles.size());
	printf("lowResTriangles.size=%d\n", lowResTriangles.size());


}

int DrawGLScene(GLvoid) // Here's Where We Do All The Drawing
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(212, 60, 195,	  // eye position
		185, 55, 170,	  // reference point position
		0.0, 1.0, 0.0); // up vector direction
	glScalef(scaleValue, scaleValue * HEIGHT_RATIO, scaleValue);

	/*
	float val = 1.0 / (2.0 * STEP_SIZE);
	gluLookAt(MAP_WIDTH * val, 80.0, MAP_HEIGHT * val,
		MAP_WIDTH / 2.0, -10.0, MAP_HEIGHT / 2.0,
		0.0, 1.0, 0.0);
	*/


	glTranslatef(MAP_WIDTH / 2, 0.0, MAP_HEIGHT / 2);
	//drawAxis();
	//TODO: make the transformation by map wisth and height
	glTranslatef(0, 10.0 * upDownTransorm, 0); // up/down
	glRotatef(leftRightRotate * 40, 0, 10, 0); // rotate with keyboard
	glRotatef(diagonalRotate * 40, -1, 0, 1); // rotate with keyboard
	glTranslatef(-MAP_WIDTH / 2, 0.0, -MAP_HEIGHT / 2);

	return TRUE;
}

void mainLoop() {
	DrawGLScene();
	render();
	handleInput();
}

