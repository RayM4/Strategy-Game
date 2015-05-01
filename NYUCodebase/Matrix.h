#pragma once
#include "Vector.h"

class Matrix {
public:
	Matrix();

	union {
		float matrix[4][4];
		float m1[16];
	};

	void identity();

	//int getValue(int x, int y);
	//void setValue(int x, int y, int value);

	Matrix inverse();
	Matrix operator* (const Matrix& m);
	Vector operator* (const Vector& v);
private:
};