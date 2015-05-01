#pragma once
#include "Vector.h"
#include <vector>
#include "Particle.h"
#include <math.h>
#include <time.h>
#include "Color.h"
#include "SpriteSheet.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class ParticleEmitter {
public:
	ParticleEmitter(unsigned int particleCount, float x, float y, float gx, float gy, float vx, float vy, float dx, float dy);
	ParticleEmitter();
	~ParticleEmitter();
	void Update(float elapsed);
	void Render();

	void Render2(const char *image_path);

	Vector getPos();
	Vector getGrav();
	Vector getVelocity();
	float getLife();
	float getTimer();

	void setPos(Vector a);
	void setGrav(Vector a);
	void setVelocity(Vector a);
	void setLife(float a);

	void setSColor(float r, float g, float b, float a);
	void setEColor(float r, float g, float b, float a);

	std::vector<Particle> particles;
	
	float startSize;
	float endSize;
	float sizeDeviation;
	

private:
	float lerp(float from, float to, float time);
	GLuint LoadTexture(const char *image_path);

	Color startColor;
	Color endColor;
	Vector position;
	Vector gravity;
	Vector velocity;
	Vector velocityDeviation;
	float maxLifetime;
	float timer;
};