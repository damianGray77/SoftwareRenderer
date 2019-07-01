#pragma once
#ifndef VERTEX3_H
#define VERTEX3_H

#include "Vertex2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

struct Vertex3 : public Vertex2 {
	float z;

	Vertex3 operator +(const Vertex3 &) const;
	Vertex3 operator -(const Vertex3 &) const;
	Vertex3 operator *(const Vertex3 &) const;
	Vertex3 operator /(const Vertex3 &) const;
	Vertex3 operator *(const float) const;
	Vertex3 operator /(const float) const;
	Vertex3 operator *(const Matrix4x4 &) const;
	Vertex3 operator *(const Matrix3x3 &) const;
	Vertex3 &operator +=(const Vertex3 &);
	Vertex3 &operator -=(const Vertex3 &);
	Vertex3 &operator *=(const Vertex3 &);
	Vertex3 &operator /=(const Vertex3 &);
	Vertex3 &operator *=(const float);
	Vertex3 &operator /=(const float);
	Vertex3 &operator *=(const Matrix4x4 &);
	Vertex3 &operator *=(const Matrix3x3 &);
	bool operator ==(const Vertex3 &);
	bool operator !=(const Vertex3 &);
	static float magnitude(const Vertex3 &);
	static float dot(const Vertex3 &, const Vertex3 &);
	static Vertex3 cross(const Vertex3 &, const Vertex3 &);
	static Vertex3 normal(Vertex3 &);
};

#endif
