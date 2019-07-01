#include "stdafx.h"
#include "Vertex2uv.h"

Vertex2uv Vertex2uv::operator+(const Vertex2uv& vert) const {
	Vertex2uv r;
	r.x = x + vert.x;
	r.y = y + vert.y;
	r.u = u + vert.u;
	r.v = v + vert.v;

	return r;
}

Vertex2uv Vertex2uv::operator-(const Vertex2uv& vert) const {
	Vertex2uv r;
	r.x = x - vert.x;
	r.y = y - vert.y;
	r.u = u - vert.u;
	r.v = v - vert.v;

	return r;
}

bool Vertex2uv::operator==(const Vertex2uv& vert) const {
	return vert.x == x && vert.y == y && vert.u == u && vert.v == v;
}

bool Vertex2uv::intersects(const Vertex2 &x0, const Vertex2 &x1, const Vertex2uv &y0, const Vertex2uv &y1, const ClipSide s, Vertex2uv &r) {
	const Vertex2 dx = x1 - x0;
	const Vertex2uv dy = y1 - y0;
	const Vertex2 d = x0 - y0;

	float dyx = cross(dy, dx);
	if (0 == dyx) { return false; }

	dyx = cross(d, dx) / dyx;
	if (dyx <= 0 || dyx >= 1) { return false; }

	const float sx = dyx * dy.x;
	const float sy = dyx * dy.y;
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

	r.u = y0.u + su;
	r.v = y0.v + sv;

	return true;
}