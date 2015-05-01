#pragma once

#define FIXED_TIMESTEP 0.0166666f
#define SCALE 0.65f
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "SpriteSheet.h"
#include <vector>

#include "Matrix.h"


class Entity {
public:
	Entity(float x, float y, float u, float v, float w, float h, float scale, SpriteSheet* sprites);

	float getX();
	float getY();
	float getU();
	float getV();
	float getSpeed();
	float getH();
	float getW();

	float getScale();


	SpriteSheet * getSheet();

	void setX(float x);
	void setY(float y);
	void setU(float x);
	void setV(float y);
	void setH(float h);
	void setW(float w);

	void setSpeed(float spd);

	void setTexture(SpriteSheet* sprites);


	void update(float elapsed);

	void fixedUpdate();

	void draw();
	void draw(float x, float y);
	void draw(bool flip);

	void buildMatrix();

	Matrix matrix;
	//add getters and setters if you want
	bool moved;
	int hp;
	int movement;
	int unitIdx;
	int atk;

	//for battle
	bool collidedBottom = false;
	bool collidedTop = false;
	bool collidedLeft = false;
	bool collidedRight = false;

	float accelerationX;
	float accelerationY;
	float velocityX;
	float velocityY;
	float frictionX;
	float frictionY;

	//other
	bool isDead();
	bool flip;
private:
	float lerp(float v0, float v1, float t);

	float width;
	float height;
	float u;
	float v;

	float posX;
	float posY;

	float speed;
	float scale; 

	float rotation;

	

	SpriteSheet* image;
};