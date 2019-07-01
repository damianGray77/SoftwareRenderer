#include "stdafx.h"
#include "Matrix3x3.h"

const inline void Matrix3x3::set(Matrix3x3 &m, float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22) {
	m._00 = _00; m._01 = _01; m._02 = _02;
	m._10 = _10; m._11 = _11; m._12 = _12;
	m._20 = _20; m._21 = _21; m._22 = _22;
}

const inline void Matrix3x3::copy(Matrix3x3 &m1, Matrix4x4 &m2) {
	set(m1, m2._00, m2._01, m2._02, m2._10, m2._11, m2._12, m2._20, m2._21, m2._22);
}