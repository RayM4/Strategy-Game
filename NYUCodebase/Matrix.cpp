#pragma once
#include "Matrix.h"

Matrix::Matrix() {
	identity();
}

void Matrix::identity() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i][j] = 0;
		}
	}

	matrix[0][0] = 1;
	matrix[1][1] = 1;
	matrix[2][2] = 1;
	matrix[3][3] = 1;
}

Matrix Matrix::operator* (const Matrix& m) {
	Matrix a;
	//a.matrix[0][0] = matrix[0][0] * m.matrix[0][0] + matrix[0][1] * m.matrix[1][0] 
	//	+ matrix[0][2] * m.matrix[2][0] + matrix[0][3] * m.matrix[3][0];

	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			a.matrix[row][col] = matrix[row][0] * m.matrix[0][col] + matrix[row][1] * m.matrix[1][col]
				+ matrix[row][2] * m.matrix[2][col] + matrix[row][3] * m.matrix[3][col];
		}
	}

	return a;
}

Vector Matrix::operator* (const Vector& v) {
	Vector a;

	//a.x = matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z;
	//a.y = matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z;
	//a.z = matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z;

	a.x = matrix[0][0] * v.x + matrix[1][0] * v.y + matrix[2][0] * v.z + matrix[3][0];
	a.y = matrix[0][1] * v.x + matrix[1][1] * v.y + matrix[2][1] * v.z + matrix[3][1];
	a.z = matrix[0][2] * v.x + matrix[1][2] * v.y + matrix[2][2] * v.z + matrix[3][2];

	return a;
}

Matrix Matrix::inverse() {
	float m00 = matrix[0][0], m01 = matrix[0][1], m02 = matrix[0][2], m03 = matrix[0][3];
	float m10 = matrix[1][0], m11 = matrix[1][1], m12 = matrix[1][2], m13 = matrix[1][3];
	float m20 = matrix[2][0], m21 = matrix[2][1], m22 = matrix[2][2], m23 = matrix[2][3];
	float m30 = matrix[3][0], m31 = matrix[3][1], m32 = matrix[3][2], m33 = matrix[3][3];

	float v0 = m20 * m31 - m21 * m30;
	float v1 = m20 * m32 - m22 * m30;
	float v2 = m20 * m33 - m23 * m30;
	float v3 = m21 * m32 - m22 * m31;
	float v4 = m21 * m33 - m23 * m31;
	float v5 = m22 * m33 - m23 * m32;

	float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
	float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
	float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
	float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

	float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
	float d00 = t00 * invDet;
	float d10 = t10 * invDet;
	float d20 = t20 * invDet;
	float d30 = t30 * invDet;
	float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;

	float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;

	float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	Matrix m2;
	m2.matrix[0][0] = d00;
	m2.matrix[0][1] = d01;
	m2.matrix[0][2] = d02;
	m2.matrix[0][3] = d03;
	m2.matrix[1][0] = d10;
	m2.matrix[1][1] = d11;
	m2.matrix[1][2] = d12;
	m2.matrix[1][3] = d13;
	m2.matrix[2][0] = d20;
	m2.matrix[2][1] = d21;
	m2.matrix[2][2] = d22;
	m2.matrix[2][3] = d23;
	m2.matrix[3][0] = d30;
	m2.matrix[3][1] = d31;
	m2.matrix[3][2] = d32;
	m2.matrix[3][3] = d33;

	return m2;
}

//int Matrix::getValue(int x, int y) {
//	return matrix[x][y];
//}
//
//void Matrix::setValue(int x, int y, int value) {
//	matrix[x][y] = value;
//}