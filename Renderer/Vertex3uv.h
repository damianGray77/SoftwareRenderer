#pragma once
#ifndef VERTEX3UV_H
#define VERTEX3UV_H

#include "Vertex3.h"

struct Vertex3uv : public Vertex3 {
	float u, v;

	Vertex3uv operator+(const Vertex3uv&) const;
	Vertex3uv operator-(const Vertex3uv&) const;
	bool operator==(const Vertex3uv&) const;

	static bool intersects(const Vertex2 &, const Vertex2 &, const Vertex3uv &, const Vertex3uv &, const ClipSide, Vertex3uv &);
};

#endif
