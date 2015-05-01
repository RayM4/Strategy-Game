#include "Bullet.h"
#define PI 3.14159265359f

Bullet::Bullet(float x, float y, float u, float v, float w, float h, float speed, SpriteSheet* sprite)
	: x(x), y(y), u(u), v(v), w(w), h(h), speed(speed), image(sprite) {
	angle = 0.0f;
};

void Bullet::setX(float px) { x = px; }
void Bullet::setY(float py) { y = py; }
void Bullet::setSpeed(float spd) { speed = spd; }
void Bullet::setAngle(float ang) { angle = ang; }
void Bullet::setTime(float time) { timeAlive = time; }

float Bullet::getX() { return x; }
float Bullet::getY() { return y; }
float Bullet::getW() { return w; }
float Bullet::getAngle() { return angle; }
float Bullet::getSpeed() { return speed; }
float Bullet::getTime() { return timeAlive; }

void Bullet::draw() {
	buildMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix.m1);

	image->draw(SCALE1, x, y, u, v, w, h, flip);
	

	glPopMatrix();
}

void Bullet::update(float elapsed) {
	if (flip) {
		x += cos(angle * PI / 180.0f) * speed;
	}
	else {
		x += cos(angle * PI / 180.0f) * speed * -1.0f;
	}
	//x += cos(angle * PI / 180.0f) * speed;
	//y += sin(angle * PI / 180.0f) * speed;
	timeAlive++;
}

void Bullet::buildMatrix() {
	Matrix scale;
	scale.matrix[0][0] = SCALE1;
	scale.matrix[1][1] = SCALE1;

	Matrix rot;
	rot.matrix[0][0] = cos(angle);
	rot.matrix[1][0] = -sin(angle);
	rot.matrix[0][1] = sin(angle);
	rot.matrix[1][1] = cos(angle);

	Matrix trans;
	trans.matrix[3][0] = x;
	trans.matrix[3][1] = y;

	matrix = scale * rot * trans;
}