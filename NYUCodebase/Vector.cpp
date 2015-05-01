#include "Vector.h"

Vector::Vector() {
	x = 1.0f;
	y = 1.0f;
	z = 1.0f;
}

Vector::Vector(float x, float y, float z)
	: x(x), y(y), z(z) {};

void Vector::normalize() {
	float length = sqrt(x*x + y*y);
	x /= length;
	y /= length;
}

float Vector::length() {
	return sqrt(x*x + y*y);
}