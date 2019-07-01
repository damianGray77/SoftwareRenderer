#include "stdafx.h"
#include "Vertex3uv.h"

Vertex3uv Vertex3uv::operator+(const Vertex3uv& vert) const {
	Vertex3uv r;
	r.x = x + vert.x;
	r.y = y + vert.y;
	r.z = z + vert.z;
	r.u = u + vert.u;
	r.v = v + vert.v;

	return r;
}

Vertex3uv Vertex3uv::operator-(const Vertex3uv& vert) const {
	Vertex3uv r;
	r.x = x - vert.x;
	r.y = y - vert.y;
	r.z = z - vert.z;
	r.u = u - vert.u;
	r.v = v - vert.v;

	return r;
}

bool Vertex3uv::operator==(const Vertex3uv& vert) const {
	return vert.x == x && vert.y == y && vert.z == z && vert.u == u && vert.v == v;
}

bool Vertex3uv::intersects(const Vertex2 &x0, const Vertex2 &x1, const Vertex3uv &y0, const Vertex3uv &y1, const ClipSide s, Vertex3uv &r) {
	const Vertex2 dx = x1 - x0;
	const Vertex3uv dy = y1 - y0;
	const Vertex2 d = x0 - y0;

	float dyx = Vertex2::cross(dy, dx);
	if (0 == dyx) { return false; }

	dyx = Vertex2::cross(d, dx) / dyx;
	if (dyx <= 0 || dyx >= 1) { return false; }

	const float sx = dyx * dy.x;
	const float sy = dyx * dy.y;
	const float sz = dyx * dy.z;
	const float su = dyx * dy.u;
	const float sv = dyx * dy.v;

	// deal with float point errors, since we know that the intersection is at
	// the particular bounds we are checking for we can just assign it directly
	// which saves the potential of calculating outside and causing an overflow
	switch (s) {
		case top:
		case bottom:
			r.x = y0.x + sx;
			r.y = x0.y;
			break;
		case left:
		case right:
			r.x = x0.x;
			r.y = y0.y + sy;
			break;
	}

	r.z = y0.z + sz;
	r.u = y0.u + su;
	r.v = y0.v + sv;

	return true;
}