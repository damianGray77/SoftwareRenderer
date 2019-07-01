#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "Vertex3.h"
#include "Matrix4x4.h"

class Camera {
public:
	Camera();
	~Camera() {}

	const void set_pos(const float, const float, const float, const float, const float, const float, const float, const float, const float);
	void move(float);
	void strafe(float);
	void level(float);
	void rotate(float, float, float, float);
	void revolve(Vertex3 &, float, float, float, float);

	void update();
	Matrix4x4 look();

	Vertex3 pos;
	Vertex3 view;
	Vertex3 up;
	Vertex3 orth;
};

#endif
