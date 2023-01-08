#pragma once
#include "globals.h"
class Weather {
public:
	// rain
	void initRain()
	{
		int X = heightMap.cols;
		int Z = heightMap.rows;
		for (int i = 0; i < MAX_DROPS; i++) {
			RainDrop drop;
			drop.x = float(rand() % X);
			drop.y = float(rand() % 150);
			drop.z = float(rand() % Z);
			drop.gravity = -0.1f;
			drop.velocity = 0.0f;
			drop.initalHeight = float(rand() % 150);
			drop.slowRate = float((rand() % 150)) / 10.0;
			drops.push_back(drop);
		}
	}

	void renderRain() {
		if (raining) {
			for (int i = 0; i < drops.size(); i++) {
				RainDrop d = drops[i];
				glColor4f(0.5, 0.5, 1.0, d.y / 61.0);
				glBegin(GL_LINES);
				glVertex3f(d.x, d.y, d.z);
				glVertex3f(d.x, d.y - 0.5, d.z);
				glEnd();
				drops[i].y += drops[i].velocity / (drops[i].slowRate * 50);
				drops[i].velocity += drops[i].gravity;
				if (drops[i].y <= 0) {
					drops[i].y = drops[i].initalHeight;
					drops[i].velocity = 0.0f;
				}
			}
		}
	}

	void rain() {
		raining = !raining;
	}



	typedef struct {
		float initalHeight;
		float slowRate;
		float x, y, z;
		float gravity;
		float velocity;
	} RainDrop;
	int MAX_DROPS = 5000;
	vector<RainDrop> drops;
	bool raining = FALSE;

	// fog

	float fogIntensity = 0.0;

	void initFog() {
		GLfloat fogColor[4] = { 0.5, 0.5, 0.5, 0.2 };
		//glClearColor(0.5, 0.5, 0.5, 0.2);
		glFogi(GL_FOG_MODE, GL_EXP2);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, fogIntensity);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glFogf(GL_FOG_START, 1.0f);
		glFogf(GL_FOG_END, 25.0f);
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, GL_EXP2);
	}

	void addFog() {
		fogIntensity += 0.01;
	}




};