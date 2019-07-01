#include "stdafx.h"
#include "Poly2.h"


Poly2::Poly2() {
	verts.reserve(16);
	length = 0;
	max = 16;
}

Poly2::Poly2(const Triangle2 &tri) {
	verts.reserve(3);
	length = 3;
	max = 3;

	verts[0] = tri.v1;
	verts[1] = tri.v2;
	verts[2] = tri.v3;
}

void Poly2::append(const Vertex2 &v) {
	verts[length] = v;
	++length;

	if (length == max) {
		max *= 2;
		verts.reserve(max);
	}
}