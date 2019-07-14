#include "stdafx.h"
#include "Vertex3uvc.h"

Vertex3uvc Vertex3uvc::operator-(const Vertex3uvc& vert) const {
	Vertex3uvc nv;
	nv.x = x - vert.x;
	nv.y = y - vert.y;
	nv.z = z - vert.z;
	nv.u = u - vert.u;
	nv.v = v - vert.v;
	nv.r = r; // - vert.r;
	nv.g = g; // - vert.g;
	nv.b = b; // - vert.b;

	return nv;
}

bool Vertex3uvc::intersects(const Vertex2 &x0, const Vertex2 &x1, const Vertex3uvc &y0, const Vertex3uvc &y1, const ClipSide s, Vertex3uvc &r) {
	const Vertex2 dx = x1 - x0;
	const Vertex3uvc dy = y1 - y0;
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
	const float sr = dyx * dy.r;
	const float sg = dyx * dy.g;
	const float sb = dyx * dy.b;

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
	r.r = y0.r * sr;
	r.g = y0.g * sg;
	r.b = y0.b * sb;

	return true;
}