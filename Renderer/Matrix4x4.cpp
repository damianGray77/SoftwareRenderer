#include "stdafx.h"
#include "Matrix4x4.h"

Matrix4x4 Matrix4x4::operator +(const Matrix4x4 &m) const {
	Matrix4x4 r;
	for (int i = 0; i <= 3; ++i) {
		r.matrix[i][0] = matrix[i][0] + m.matrix[i][0];
		r.matrix[i][1] = matrix[i][1] + m.matrix[i][1];
		r.matrix[i][2] = matrix[i][2] + m.matrix[i][2];
		r.matrix[i][3] = matrix[i][3] + m.matrix[i][3];
	}

	return r;
}

Matrix4x4 Matrix4x4::operator -(const Matrix4x4 &m) const {
	Matrix4x4 r;
	for (int i = 0; i <= 3; ++i) {
		r.matrix[i][0] = matrix[i][0] - m.matrix[i][0];
		r.matrix[i][1] = matrix[i][1] - m.matrix[i][1];
		r.matrix[i][2] = matrix[i][2] - m.matrix[i][2];
		r.matrix[i][3] = matrix[i][3] - m.matrix[i][3];
	}

	return r;
}

Matrix4x4 Matrix4x4::operator *(const Matrix4x4 &m) const {
	Matrix4x4 r;
	float e1, e2, e3, e4;
	for (int i = 0; i <= 3; ++i) {
		e1 = m.matrix[i][0];
		e2 = m.matrix[i][1];
		e3 = m.matrix[i][2];
		e4 = m.matrix[i][3];

		r.matrix[i][0] = (e1 * _00) + (e2 * _10) + (e3 * _20) + (e4 * _30);
		r.matrix[i][1] = (e1 * _01) + (e2 * _11) + (e3 * _21) + (e4 * _31);
		r.matrix[i][2] = (e1 * _02) + (e2 * _12) + (e3 * _22) + (e4 * _32);
		r.matrix[i][3] = (e1 * _03) + (e2 * _13) + (e3 * _23) + (e4 * _33);
	}

	return r;
}

Matrix4x4 &Matrix4x4::operator +=(const Matrix4x4 &m) {
	for (int i = 0; i <= 3; ++i) {
		matrix[i][0] += m.matrix[i][0];
		matrix[i][1] += m.matrix[i][1];
		matrix[i][2] += m.matrix[i][2];
		matrix[i][3] += m.matrix[i][3];
	}

	return *this;
}

Matrix4x4 &Matrix4x4::operator -=(const Matrix4x4 &m) {
	for (int i = 0; i <= 3; ++i) {
		matrix[i][0] -= m.matrix[i][0];
		matrix[i][1] -= m.matrix[i][1];
		matrix[i][2] -= m.matrix[i][2];
		matrix[i][3] -= m.matrix[i][3];
	}

	return *this;
}

Matrix4x4 &Matrix4x4::operator *=(const Matrix4x4 &m) {
	float e1, e2, e3, e4;
	for (int i = 0; i <= 3; ++i) {
		e1 = m.matrix[i][0];
		e2 = m.matrix[i][1];
		e3 = m.matrix[i][2];
		e4 = m.matrix[i][3];

		matrix[i][0] = (e1 * _00) + (e2 * _10) + (e3 * _20) + (e4 * _30);
		matrix[i][1] = (e1 * _01) + (e2 * _11) + (e3 * _21) + (e4 * _31);
		matrix[i][2] = (e1 * _02) + (e2 * _12) + (e3 * _22) + (e4 * _32);
		matrix[i][3] = (e1 * _03) + (e2 * _13) + (e3 * _23) + (e4 * _33);
	}

	return *this;
}

void Matrix4x4::set(Matrix4x4 &m, float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23, float _30, float _31, float _32, float _33) {
	m._00 = _00; m._01 = _01; m._02 = _02; m._03 = _03;
	m._10 = _10; m._11 = _11; m._12 = _12; m._13 = _13;
	m._20 = _20; m._21 = _21; m._22 = _22; m._23 = _23;
	m._30 = _30; m._31 = _31; m._32 = _32; m._33 = _33;
}

void Matrix4x4::copy(Matrix4x4 &m1, Matrix4x4 &m2) {
	set(m1, m2._00, m2._01, m2._02, m2._03, m2._10, m2._11, m2._12, m2._13, m2._20, m2._21, m2._22, m2._23, m2._30, m2._31, m2._32, m2._33);
}

void Matrix4x4::identity(Matrix4x4 &m) {
	set(m, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
}

void Matrix4x4::zero(Matrix4x4 &m) {
	set(m, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

void Matrix4x4::invert(Matrix4x4 &m1, Matrix4x4 &m2) {
	const float s0 = m2._00 * m2._11 - m2._10 * m2._01;
	const float s1 = m2._00 * m2._12 - m2._10 * m2._02;
	const float s2 = m2._00 * m2._13 - m2._10 * m2._03;
	const float s3 = m2._01 * m2._12 - m2._11 * m2._02;
	const float s4 = m2._01 * m2._13 - m2._11 * m2._03;
	const float s5 = m2._02 * m2._13 - m2._12 * m2._03;

	const float c5 = m2._22 * m2._33 - m2._32 * m2._23;
	const float c4 = m2._21 * m2._33 - m2._31 * m2._23;
	const float c3 = m2._21 * m2._32 - m2._31 * m2._22;
	const float c2 = m2._20 * m2._33 - m2._30 * m2._23;
	const float c1 = m2._20 * m2._32 - m2._30 * m2._22;
	const float c0 = m2._20 * m2._31 - m2._30 * m2._21;

	// Should check for 0 determinant

	const float d = 1.0f / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

	set(m1
		, ( m2._11 * c5 - m2._12 * c4 + m2._13 * c3) * d
		, ( m2._11 * c5 - m2._12 * c4 + m2._13 * c3) * d
		, ( m2._31 * s5 - m2._32 * s4 + m2._33 * s3) * d
		, (-m2._21 * s5 + m2._22 * s4 - m2._23 * s3) * d
		, (-m2._10 * c5 + m2._12 * c2 - m2._13 * c1) * d
		, ( m2._00 * c5 - m2._02 * c2 + m2._03 * c1) * d
		, (-m2._30 * s5 + m2._32 * s2 - m2._33 * s1) * d
		, ( m2._20 * s5 - m2._22 * s2 + m2._23 * s1) * d
		, ( m2._10 * c4 - m2._11 * c2 + m2._13 * c0) * d
		, (-m2._00 * c4 + m2._01 * c2 - m2._03 * c0) * d
		, ( m2._30 * s4 - m2._31 * s2 + m2._33 * s0) * d
		, (-m2._20 * s4 + m2._21 * s2 - m2._23 * s0) * d
		, (-m2._10 * c3 + m2._11 * c1 - m2._12 * c0) * d
		, ( m2._00 * c3 - m2._01 * c1 + m2._02 * c0) * d
		, (-m2._30 * s3 + m2._31 * s1 - m2._32 * s0) * d
		, ( m2._20 * s3 - m2._21 * s1 + m2._22 * s0) * d
	);
}

void Matrix4x4::scale(Matrix4x4 &m, float x, float y, float z) {
	Matrix4x4 id;
	identity(id);
	id._00 = x;
	id._11 = y;
	id._22 = z;

	m = id * m;
}

void Matrix4x4::translate(Matrix4x4 &m, float x, float y, float z) {
	Matrix4x4 id;
	identity(id);
	id._30 = x;
	id._31 = y;
	id._32 = z;

	m = id * m;
}

void Matrix4x4::rotate(Matrix4x4 &m, float x, float y, float z) {
	Matrix4x4 rx, ry, rz;

	identity(rx);
	identity(ry);
	identity(rz);

	int ix = (int)x;
	int iy = (int)y;
	int iz = (int)z;

	rx._11 = COS(ix);
	rx._12 = SIN(ix);
	rx._21 = -rx._12;
	rx._22 = rx._11;

	ry._00 = COS(iy);
	ry._02 = -SIN(iy);
	ry._20 = -ry._02;
	ry._22 = ry._00;

	rz._00 = COS(iz);
	rz._01 = SIN(iz);
	rz._10 = -rz._01;
	rz._11 = rz._00;

	m = rx * m;
	m = ry * m;
	m = rz * m;
}

void Matrix4x4::rotate_x(Matrix4x4 &m, float num) {
	Matrix4x4 rx;

	identity(rx);

	int i = (int)num;

	rx._11 = COS(i);
	rx._12 = SIN(i);
	rx._21 = -rx._12;
	rx._22 = rx._11;

	m = rx * m;
}

void Matrix4x4::rotate_y(Matrix4x4 &m, float num) {
	Matrix4x4 ry;

	identity(ry);

	int i = (int)num;

	ry._00 = COS(i);
	ry._02 = -SIN(i);
	ry._20 = -ry._02;
	ry._22 = ry._00;

	m = ry * m;
}

void Matrix4x4::rotate_z(Matrix4x4 &m, float num) {
	Matrix4x4 rz;

	identity(rz);

	int i = (int)num;

	rz._00 = COS(i);
	rz._01 = SIN(i);
	rz._10 = -rz._01;
	rz._11 = rz._00;

	m = rz * m;
}