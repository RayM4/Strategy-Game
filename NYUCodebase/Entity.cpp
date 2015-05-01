#include "Entity.h"

#define GRAVITY_X 0.0f;
#define GRAVITY_Y -8.0f;
#define FIXED_TIMESTEP 0.0166666f

Entity::Entity(float x, float y, float u, float v, float w, float h, float scale, SpriteSheet* sprites)
	: posX(x), posY(y), u(u), v(v), width(w), height(h), image(sprites), scale(scale) {
	rotation = 0.0f;
	moved = false;
	frictionX = 5.0f;
	frictionY = 5.0f;
	flip = true;
};

float Entity::getX() { return posX; }
float Entity::getY() { return posY; }
float Entity::getU() { return u; }
float Entity::getV() { return v; }
float Entity::getSpeed() { return speed; }


float Entity::getH() { return height; }
float Entity::getW() { return width; }


SpriteSheet * Entity::getSheet() { return image; }

float Entity::getScale() { return scale;  };

void Entity::setX(float x) { posX = x; }
void Entity::setY(float y) { posY = y; }
void Entity::setU(float x) { u = x; }
void Entity::setV(float y) { v = y; }
void Entity::setH(float h) { height = h; }
void Entity::setW(float w) { width = w; }
void Entity::setSpeed(float spd) { speed = spd; }


void Entity::setTexture(SpriteSheet* sprite) { image = sprite; }

bool Entity::isDead() {
	if (hp > 0) {
		return false;
	}
	else {
		return true;
	}
}


void Entity::draw() {
	if (!isDead()) {
		buildMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrix.m1);

		image->draw(scale, posX, posY, u, v, width, height);

		glPopMatrix();
	}
}

void Entity::draw(bool flip) {
	if (!isDead()) {
		buildMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(matrix.m1);

		image->draw(scale, posX, posY, u, v, width, height, flip);

		glPopMatrix();
	}
}

void Entity::draw(float x, float y) {
	buildMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix.m1);
	image->drawf(scale, x, y, u, v, width, height);

	glPopMatrix();
}

void Entity::update(float elapsed) {

}

void Entity::fixedUpdate() {
	collidedBottom = false;
	collidedTop = false;
	collidedLeft = false;
	collidedRight = false;

	velocityX = lerp(velocityX, 0.0f, FIXED_TIMESTEP * frictionX);
	velocityY = lerp(velocityY, 0.0f, FIXED_TIMESTEP * frictionY);
	velocityX = velocityX + accelerationX * FIXED_TIMESTEP;
	velocityY = velocityY + accelerationY * FIXED_TIMESTEP;
	velocityY = velocityY + -15.0f * FIXED_TIMESTEP;
	velocityX = velocityX + 0.0f * FIXED_TIMESTEP;

	posX = posX + velocityX * FIXED_TIMESTEP;
	posY = posY + velocityY * FIXED_TIMESTEP;
}

float Entity::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}

void Entity::buildMatrix() {
	Matrix scale;
	scale.matrix[0][0] = SCALE;
	scale.matrix[1][1] = SCALE;

	Matrix rot;
	rot.matrix[0][0] = cos(rotation);
	rot.matrix[1][0] = -sin(rotation);
	rot.matrix[0][1] = sin(rotation);
	rot.matrix[1][1] = cos(rotation);

	Matrix trans;
	trans.matrix[3][0] = posX;
	trans.matrix[3][1] = posY;

	matrix = scale * rot * trans;
}