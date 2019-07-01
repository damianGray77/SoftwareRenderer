#include "stdafx.h"
#include "Edge3uv.h"

void Edge3uv::init(int _y1, int _y2) {
	y1 = _y1;
	y2 = _y2;

	int len = MAX(_y1, _y2);

	z = new float[len];
	u = new float[len];
	v = new float[len];
	x = new float[len];

	initd = true;
}

void Edge3uv::unload() {
	if (!initd) { return; }

	delete[] z;
	delete[] u;
	delete[] v;
	delete[] x;

	initd = false;
}