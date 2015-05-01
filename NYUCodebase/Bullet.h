#pragma once
#include "SpriteSheet.h"
#include <math.h>
#include "Matrix.h"
#define SCALE1 0.65f

class Bullet {
public:
	Bullet(float x, float y, float u, float v, float w, float h, float speed, SpriteSheet* sprite);

	void setX(float x);
	void setY(float y);
	void setSpeed(float spd);
	void setAngle(float angle);
	void setTime(float time);

	float getX();
	float getY();
	float getW();
	float getAngle();
	float getSpeed();
	float getTime();

	void draw();
	void update(float elapsed);
	Matrix matrix;
	void buildMatrix();
	bool flip;
private:
	float x;
	float y;
	float u;
	float v;
	float h;
	float w;
	float angle;
	float speed;
	float timeAlive;
	SpriteSheet* image;
};