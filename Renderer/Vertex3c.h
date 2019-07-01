#pragma once
#ifndef VERTEX3C_H
#define VERTEX3C_H

#include "Color4f.h"
#include "Vertex3.h"

struct Vertex3c : public Vertex3 {
	float u, v;
	Color4f c;
};

#endif
