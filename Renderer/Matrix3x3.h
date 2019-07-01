#pragma once
#ifndef MATRIX3X3_H
#define MATRIX3X3_H

#include "Matrix4x4.h"

struct Matrix3x3 {
	union {
		struct { float _00, _01, _02, _10, _11, _12, _20, _21, _22; };
		struct { float matrix[3][3]; };
	};

	static const inline void set(Matrix3x3 &m, float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22);
	static const inline void copy(Matrix3x3 &m1, Matrix4x4 &m2);
};

#endif
