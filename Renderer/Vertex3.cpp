#include "stdafx.h"
#include "Vertex3.h"

Vertex3 Vertex3::operator +(const Vertex3 &v) const {
	Vertex3 r;
	r.x = x + v.x;
	r.y = y + v.y;
	r.z = z + v.z;

	return r;
}

Vertex3 Vertex3::operator -(const Vertex3 &v) const {
	Vertex3 r;
	r.x = x - v.x;
	r.y = y - v.y;
	r.z = z - v.z;

	return r;
}

Vertex3 Vertex3::operator *(const Vertex3 &v) const {
	Vertex3 r;
	r.x = x * v.x;
	r.y = y * v.y;
	r.z = z * v.z;

	return r;
}

Vertex3 Vertex3::operator /(const Vertex3 &v) const {
	Vertex3 r;
	r.x = x / v.x;
	r.y = x / v.y;
	r.z = x / v.z;

	return r;
}

Vertex3 Vertex3::operator *(const float num) const {
	Vertex3 r;
	r.x = x * num;
	r.y = y * num;
	r.z = z * num;

	return r;
}

Vertex3 Vertex3::operator /(const float num) const {
	const float i = 1.0f / num;

	Vertex3 r;
	r.x = x * i;
	r.y = y * i;
	r.z = z * i;

	return r;
}

Vertex3 Vertex3::operator *(const Matrix4x4 &m) const {
	Vertex3 r;
	r.x = (x * m._00) + (y * m._10) + (z * m._20) + m._30;
	r.y = (x * m._01) + (y * m._11) + (z * m._21) + m._31;
	r.z = (x * m._02) + (y * m._12) + (z * m._22) + m._32;

	return r;
}

Vertex3 Vertex3::operator *(const Matrix3x3 &m) const {
	Vertex3 r;
	r.x = (x * m._00) + (y * m._10) + (z * m._20);
	r.y = (x * m._01) + (y * m._11) + (z * m._21);
	r.z = (x * m._02) + (y * m._12) + (z * m._22);

	return r;
}

Vertex3 &Vertex3::operator +=(const Vertex3 &v) {
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vertex3 &Vertex3::operator -=(const Vertex3 &v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

Vertex3 &Vertex3::operator *=(const Vertex3 &v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

Vertex3 &Vertex3::operator /=(const Vertex3 &v) {
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}

Vertex3 &Vertex3::operator *=(const float num) {
	x *= num;
	y *= num;
	z *= num;

	return *this;
}

Vertex3 &Vertex3::operator /=(const float num) {
	const float i = 1.0f / num;
	x *= i;
	y *= i;
	z *= i;

	return *this;
}

Vertex3 &Vertex3::operator *=(const Matrix4x4 &m) {
	const float rx = (x * m._00) + (y * m._10) + (z * m._20) + m._30;
	const float ry = (x * m._01) + (y * m._11) + (z * m._21) + m._31;
	const float rz = (x * m._02) + (y * m._12) + (z * m._22) + m._32;

	x = rx;
	y = ry;
	z = rz;

	return *this;
}

Vertex3 &Vertex3::operator *=(const Matrix3x3 &m) {
	const float rx = (x * m._00) + (y * m._10) + (z * m._20);
	const float ry = (x * m._01) + (y * m._11) + (z * m._21);
	const float rz = (x * m._02) + (y * m._12) + (z * m._22);

	x = rx;
	y = ry;
	z = rz;

	return *this;
}

bool Vertex3::operator ==(const Vertex3 &v) {
	return
		   (x >= v.x - EPSILON && x <= v.x + EPSILON)
		&& (y >= v.y - EPSILON && y <= v.y + EPSILON)
		&& (z >= v.z - EPSILON && z <= v.z + EPSILON)
	;
}

bool Vertex3::operator !=(const Vertex3 &v) {
	return
		   (x < v.x - EPSILON || x > v.x + EPSILON)
		|| (y < v.y - EPSILON || y > v.y + EPSILON)
		|| (z < v.z - EPSILON || z > v.z + EPSILON)
	;
}

float Vertex3::magnitude(const Vertex3 &v) {
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float Vertex3::dot(const Vertex3 &v1, const Vertex3 &v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

Vertex3 Vertex3::cross(const Vertex3 &v1, const Vertex3 &v2) {
	Vertex3 r;
	r.x = (v1.y * v2.z) - (v1.z * v2.y);
	r.y = (v1.z * v2.x) - (v1.x * v2.z);
	r.z = (v1.x * v2.y) - (v1.y * v2.x);

	return r;
}

Vertex3 Vertex3::normal(Vertex3 &v) {
	return v * isqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
