#pragma once
#include "globals.h"
#include "Triangle.h"


class ResMapper {
public:
	ResMapper(vector<Triangle *> _triangles, vector<Triangle *> _lowResTriangles)
		:triangles(_triangles), lowResTriangles(_lowResTriangles) {};

	bool isPointInTriangle_LtoS(int largeTriangleId, int smallTriangleId) {
		//  is smallTriangle.x and smallTrinagle.y in the largeTriangle?
		Point p;
		int largeStepSize = lowResTriangles[largeTriangleId]->stepSize;
		int largeEven = largeTriangleId % 2 == 0 ? 1 : -1;
		p.x = lowResTriangles[largeTriangleId]->x + int(largeEven * largeStepSize / 2);
		p.y = lowResTriangles[largeTriangleId]->y + int(largeEven * largeStepSize / 2);

		Point ta, tb, tc;
		ta.x = triangles[smallTriangleId]->x;
		ta.y = triangles[smallTriangleId]->y;

		int smallEven = smallTriangleId % 2 == 0 ? 1 : -1;
		int smallStepSize = triangles[smallTriangleId]->stepSize;
		tb.x = ta.x + smallEven * smallStepSize;
		tb.y = ta.y;

		tc.x = ta.x;
		tc.y = ta.y + smallEven * smallStepSize;

		double area_abp = Triangle::triangleArea(ta, tb, p);
		double area_acp = Triangle::triangleArea(ta, tc, p);
		double area_bcp = Triangle::triangleArea(tb, tc, p);

		double area_abc = Triangle::triangleArea(ta, tb, tc);

		return abs(area_abc - (area_abp + area_acp + area_bcp)) < 1e-9;
	}

	bool isPointInTriangle(int largeTriangleId, int smallTriangleId) {
		//  is smallTriangle.x and smallTrinagle.y in the largeTriangle?
		Point p;
		int smallEven = smallTriangleId % 2 == 0 ? 1 : -1;
		int smallStepSize = triangles[smallTriangleId]->stepSize;
		p.x = triangles[smallTriangleId]->x + int(smallEven * smallStepSize / 2);
		p.y = triangles[smallTriangleId]->y + int(smallEven * smallStepSize / 2);

		Point ta, tb, tc;
		ta.x = lowResTriangles[largeTriangleId]->x;
		ta.y = lowResTriangles[largeTriangleId]->y;

		int largeStepSize = lowResTriangles[largeTriangleId]->stepSize;
		int largeEven = largeTriangleId % 2 == 0 ? 1 : -1;
		tb.x = ta.x + largeEven * largeStepSize;
		tb.y = ta.y;

		tc.x = ta.x;
		tc.y = ta.y + largeEven * largeStepSize;

		double area_abp = Triangle::triangleArea(ta, tb, p);
		double area_acp = Triangle::triangleArea(ta, tc, p);
		double area_bcp = Triangle::triangleArea(tb, tc, p);

		double area_abc = Triangle::triangleArea(ta, tb, tc);

		return abs(area_abc - (area_abp + area_acp + area_bcp)) < 1e-9;
	}

	int largeToSmall(int id) {
		int lowResStepSize = lowResTriangles[0]->stepSize;
		int highResStepSize = triangles[0]->stepSize;

		Triangle* largeTriangle = lowResTriangles[id];
		int sx = largeTriangle->x;
		int sy = largeTriangle->y;

		for (auto& smallTriangle : triangles) {
			if (isPointInTriangle_LtoS(id, smallTriangle->id)) {
				return smallTriangle->id;
			}
		}
		return -1;
	}

	int smallToLarge(int id) {
		int lowResStepSize = lowResTriangles[0]->stepSize;
		int highResStepSize = triangles[0]->stepSize;

		Triangle* smallTriangle = triangles[id];
		int sx = smallTriangle->x;
		int sy = smallTriangle->y;

		for (auto& largeTriangle : lowResTriangles) {
			if (isPointInTriangle(largeTriangle->id, id)) {
				return largeTriangle->id;
			}
		}
		return -1;
	}


	vector<Triangle *> triangles;
	vector<Triangle *> lowResTriangles;
};
