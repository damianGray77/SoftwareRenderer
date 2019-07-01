#include "stdafx.h"
#include "Camera.h"

Camera::Camera() { }

const void Camera::set_pos(const float px, const float py, const float pz, const float vx, const float vy, const float vz, const float ux, const float uy, const float uz) {
	pos.x = px;  pos.y = py;  pos.z = pz;
	view.x = vx; view.y = vy; view.z = vz;
	up.x = ux;   up.y = uy;   up.z = uz;
}

void Camera::move(float s) {
	if(0 == s) { return; }

	Vertex3 v = view - pos;
	v = Vertex3::normal(v);

	pos += v * s;
	view += v *s;
}

void Camera::strafe(float s) {
	if (0 == s) { return; }

	pos -= orth * s;
	view -= orth * s;
}

void Camera::level(float s) {
	if (0 == s) { return; }

	pos += up * s;
	view += up * s;
}

void Camera::rotate(float a, float x, float y, float z) {
	if (0 == a) { return; }

	Vertex3 v = view - pos;

	const int ia = (int)a;

	const float ctheta = COS(ia);
	const float stheta = SIN(ia);
	const float ictheta = (1 - ctheta);
	const float icthetax = ictheta * x;
	const float icthetay = ictheta * y;
	const float icthetaz = ictheta * z;
	const float icthetaxy = icthetax * y;
	const float icthetayz = icthetay * z;
	const float icthetazx = icthetaz * x;
	const float sthetax = stheta * x;
	const float sthetay = stheta * y;
	const float sthetaz = stheta * z;

	Vertex3 n;

	n.x = (ctheta + icthetax * x) * v.x;
	n.x += (icthetaxy - sthetaz) * v.y;
	n.x += (icthetazx + sthetay) * v.z;

	n.y = (icthetaxy + sthetaz) * v.x;
	n.y += (ctheta + icthetay * y) * v.y;
	n.y += (icthetayz - sthetax) * v.z;

	n.z = (icthetazx - sthetay) * v.x;
	n.z += (icthetayz + sthetax) * v.y;
	n.z += (ctheta + icthetaz * z) * v.z;

	view = pos + n;
}

void Camera::revolve(Vertex3 &o, float a, float x, float y, float z) {
	if (0 == a) { return; }

	Vertex3 v = pos - o;

	const int ia = (int)a;

	const float ctheta = COS(ia);
	const float stheta = SIN(ia);
	const float ictheta = (1 - ctheta);
	const float icthetaxy = ictheta * x * y;
	const float icthetayz = ictheta * y * z;
	const float icthetazx = ictheta * z * x;
	const float sthetax = stheta * x;
	const float sthetay = stheta * y;
	const float sthetaz = stheta * z;

	Vertex3 n;

	n.x = (ctheta + ictheta * x * x) * pos.x;
	n.x += (icthetaxy - sthetaz) * pos.y;
	n.x += (icthetazx + sthetay) * pos.z;

	n.y = (icthetaxy + sthetaz) * pos.x;
	n.y += (ctheta + ictheta * y * y) * pos.y;
	n.y += (icthetayz - sthetax) * pos.z;

	n.z = (icthetazx - sthetay) * pos.x;
	n.z += (icthetayz + sthetax) * pos.y;
	n.z += (ctheta + ictheta * z * z) * pos.z;

	pos = o + n;
}

void Camera::update() {
	Vertex3 c = Vertex3::cross(view - pos, up);
	orth = Vertex3::normal(c);
}

Matrix4x4 Camera::look() {
	Vertex3 z = pos - view;
	z = Vertex3::normal(z);

	Vertex3 x = Vertex3::cross(up, z);
	x = Vertex3::normal(x);

	Vertex3 y = Vertex3::cross(z, x);
	y = Vertex3::normal(y);

	const float dx = -Vertex3::dot(x, pos);
	const float dy = -Vertex3::dot(y, pos);
	const float dz = -Vertex3::dot(z, pos);

	const Matrix4x4 m = { x.x, y.x, z.x, 0.0f, x.y, y.y, z.y, 0.0f, x.z, y.z, z.z, 0.0f, dx, dy, dz, 1.0f };

	return m;
}