#pragma once
#include <cmath>
#include "globals.h"
using namespace std;


class Triangle
{
public:
	Triangle(const GLfloat v1[3], const GLfloat v2[3], const GLfloat v3[3], int stepSize)
		: v1{ v1[0], v1[1], v1[2] },
		v2{ v2[0], v2[1], v2[2] },
		v3{ v3[0], v3[1], v3[2] },
		stepSize(stepSize){}

	Triangle(const int x, const int y, int stepSize)
		: x(x), y(y), stepSize(stepSize)
	{
		// x1, y1, z1
		v1[0] = float(x);
		v1[1] = height(x, y);
		v1[2] = float(y);

		// x2, y2, z2
		v2[0] = float(x) + stepSize;
		v2[1] = height(x + stepSize, y);
		v2[2] = float(y);

		// x3, y3, z3
		v3[0] = float(x);
		v3[1] = height(x, y + stepSize);
		v3[2] = float(y) + stepSize;
	}

	void draw()
	{
		// set vertex color set it to wwhite if this.hit
		glBegin(GL_TRIANGLES);
		SetVertexColor(v1[1]);
		glVertex3f(v1[0], v1[1], v1[2]);
		SetVertexColor(v2[1]);
		glVertex3f(v2[0], v2[1], v2[2]);
		SetVertexColor(v3[1]);
		glVertex3f(v3[0], v3[1], v3[2]);
		glEnd();

		drawOutline();
	}


	void drawOutline()
	{
		glColor4f(0.0, 0.0, 0.0, 0.2);
		glBegin(GL_LINE_LOOP);
		glVertex3f(v1[0], v1[1], v1[2]);
		glVertex3f(v2[0], v2[1], v2[2]);
		glVertex3f(v3[0], v3[1], v3[2]);
		glEnd();
	}
	void drawRedThickOutline()
	{
		this->hit ? glColor4f(0.0, 0.0, 1.0, 0.5) : glColor4f(1.0, 0.0, 0.0, 0.5);
		glLineWidth(2);

		glBegin(GL_LINE_LOOP);
		glVertex3f(v1[0], v1[1], v1[2]);
		glVertex3f(v2[0], v2[1], v2[2]);
		glVertex3f(v3[0], v3[1], v3[2]);
		glEnd();
	}

	void drawIdColor()
	{
		glDisable(GL_FOG);

		int i = id;
		// unique color by openGL doc
		int r = (i & 0x000000FF) >> 0;
		int g = (i & 0x0000FF00) >> 8;
		int b = (i & 0x00FF0000) >> 16;

		// store in this.color for easy access
		color.r = r;
		color.g = g;
		color.b = b;

		// glColor3f(r, g, b);
		glColor3ub(r, g, b);

		// draw as solid color
		glBegin(GL_TRIANGLES);
		glVertex3f(v1[0], v1[1], v1[2]);
		glVertex3f(v2[0], v2[1], v2[2]);
		glVertex3f(v3[0], v3[1], v3[2]);
		glEnd();

		glEnable(GL_FOG);
	}

	Triangle *getAdjecentTriangle()
	{

		GLfloat adj_v1[3] = { v3[0], v3[1], v3[2] };
		GLfloat adj_v2[3] = { v2[0], v2[1], v2[2] };
		GLfloat adj_v3[3] = { float(x) + stepSize, height(x + stepSize, y + stepSize), float(y) + stepSize };

		Triangle *res = new Triangle(adj_v1, adj_v2, adj_v3, stepSize);
		res->setXY(x + stepSize, y + stepSize);
		return res;
	}

	void paint() {
		this->userColor = TRUE;
	}

	float height(int X, int Y) // This Returns The Height From A Height Map Index
	{
		int x = X % heightMap.rows;
		int y = Y % heightMap.cols;

		if (!&heightMap)
			return 0;
		// map values from [0,256] to [0, 61] in order to use the map in color_scale_01.png
		return 61.0 * heightMap.at<Vec3b>(Point(y, x)).val[0] / 256.0;
	}

	array<float, 3> getCenter() {
		array<float, 3> center = { (v1[0] + v2[0] + v3[0]) / 3.0,
			(v1[1] + v2[1] + v3[1]) / 3.0,
			(v1[2] + v2[2] + v3[2]) / 3.0 };
		return center;
	}

	void pick()
	{
		hit = !hit;
		//hit = TRUE;
		draw();
		drawOutline();
	}

	void setXY(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	void setID(int id)
	{
		this->id = id;
	}

	static int getTriangleID(int r, int g, int b)
	{
		int i = (r << 0) | (g << 8) | (b << 16);
		return i;
	}

	static double triangleArea(Point p1, Point p2, Point p3) {
		// A = | (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2 |
		float res = abs(
			(p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y)) / 2.0
		);
		return res;

	}

	void unpaint() {
		this->hit = FALSE;
		this->userColor = FALSE;
	}


	void SetVertexColor(int fColor) // This Sets The Color Value For A Particular Index
	{
		if (this->userColor) {
			glColor3f(1.0, 1.0, 1.0);
			return;
		}
		// fcolor is the height of a vertex (y value) and sould be in range [0,61]
		if (this->hit)
		{
			// paint it white if hit
			glColor3f(1.0, 1.0, 1.0);
			return;
		}
		float height = fColor / 61.0;


		GLfloat alpha = 1;
		if (fColor < 15.8)
		{
			glColor3f(0.4f, 0.4f, 0.4f);
		}
		// dark gray
		else if (fColor < 21.45)
		{
			glColor4f(0, 0, 255, alpha);
		}
		else if (fColor < 27.10)
		{
			glColor4f(0, 128, 255, alpha);
		}
		else if (fColor < 32.75)
		{
			glColor4f(0, 255, 255, alpha);
		}
		else if (fColor < 28.40)
		{
			glColor4f(0, 255, 128, alpha);
		}
		else if (fColor < 44.05)
		{
			glColor4f(0, 255, 0, alpha);
		}
		else if (fColor < 49.70)
		{
			glColor4f(255, 255, 0, alpha);
		}
		else if (fColor < 55.35)
		{
			glColor4f(255, 128, 0, alpha);
		}
		else if (fColor < 61.00)
		{
			glColor4f(255, 0, 0, alpha);
		}
		else
		{
			glColor3f(255, 255, 255);
		} // white
	}

	struct Color
	{
		GLubyte r;
		GLubyte g;
		GLubyte b;
	};

	float sign(float x1, float y1, float x2, float y2, float x3, float y3) {
		return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
	}





	GLfloat v1[3], v2[3], v3[3];
	int x, y;
	int id;
	Color color;
	bool hit = FALSE;
	bool userColor = FALSE;
	bool debug = FALSE;
	int stepSize;
};