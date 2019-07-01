#pragma once
#ifndef VERTEX2C_H
#define VERTEX2C_H

#include "Color4f.h"
#include "Vertex2.h"

struct Vertex2c : public Vertex2 {
	float u, v;
	Color4f c;
};

#endif
