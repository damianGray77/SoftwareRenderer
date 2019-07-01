#pragma once
#ifndef POLYGON3CUV_H
#define POLYGON3CUV_H

#include "Vertex2.h"
#include "Color4f.h"

struct Polygon3cuv {
	int i1, i2, i3;
	int tId;
	Vertex2 uv1, uv2, uv3;
	Color4f c1, c2, c3;
};

#endif
