#pragma once
#ifndef MATRIX4X4_H
#define MATRIX4X4_H

struct Matrix4x4 {
	union {
		struct { float _00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33; };
		struct { float matrix[4][4]; };
	};

	Matrix4x4 operator +(const Matrix4x4 &m) const;
	Matrix4x4 operator -(const Matrix4x4 &m) const;
	Matrix4x4 operator *(const Matrix4x4 &m) const;
	Matrix4x4 &operator +=(const Matrix4x4 &m);
	Matrix4x4 &operator -=(const Matrix4x4 &m);
	Matrix4x4 &operator *=(const Matrix4x4 &m);

	static void set(Matrix4x4 &m, float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23, float _30, float _31, float _32, float _33);
	static void copy(Matrix4x4 &m1, Matrix4x4 &m2);
	static void identity(Matrix4x4 &m);
	static void zero(Matrix4x4 &m);
	static void invert(Matrix4x4 &m1, Matrix4x4 &m2);
	static void scale(Matrix4x4 &m, float x, float y, float z);
	static void translate(Matrix4x4 &m, float x, float y, float z);
	static void rotate(Matrix4x4 &m, float x, float y, float z);
	static void rotate_x(Matrix4x4 &m, float num);
	static void rotate_y(Matrix4x4 &m, float num);
	static void rotate_z(Matrix4x4 &m, float num);
};

#endif
