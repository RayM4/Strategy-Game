#pragma once
#include "Vector.h"

class Particle {
public:
	Vector position;
	Vector velocity;
	float lifetime;
	float sizeDeviation;
};