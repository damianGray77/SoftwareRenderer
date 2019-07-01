#pragma once
#ifndef POLY2_H
#define POLY2_H

#include <vector>
#include "Vertex2.h"
#include "Triangle2.h"

class Poly2 {
public:
	Poly2();
	Poly2(const Triangle2 &);

	~Poly2() { }

	void append(const Vertex2 &v);

	std::vector<Vertex2> verts;
	int length;
	int max;
};

#endif
