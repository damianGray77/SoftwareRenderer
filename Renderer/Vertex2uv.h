#pragma once
#ifndef VERTEX2UV_H
#define VERTEX2UV_H

#include "Vertex2.h"

struct Vertex2uv : public Vertex2 {
	float u, v;

	Vertex2uv operator+(const Vertex2uv&) const;
	Vertex2uv operator-(const Vertex2uv&) const;
	bool operator==(const Vertex2uv&) const;

	static bool intersects(const Vertex2 &, const Vertex2 &, const Vertex2uv &, const Vertex2uv &, const ClipSide, Vertex2uv &);
};

#endif
