#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

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
int STEP_SIZE = 15;
Mat heightMap;

// key control
bool keys[256];      // Array Used For The Keyboard Routine
#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

#define VK_Q 0x51
#define VK_E 0x45

#define VK_R 0x52
#define VK_F 0x46
#define VK_G 0x47


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

int fps = 120;
// currently unused
bool debug = FALSE;
bool pickingMode = FALSE;



#endif