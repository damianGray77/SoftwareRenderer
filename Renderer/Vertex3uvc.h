#pragma once
#ifndef VERTEX3UVC_H
#define VERTEX3UVC_H

#include "Vertex3uv.h"
#include "Matrix4x4.h"

struct Vertex3uvc : public Vertex3uv {
	float r, g, b;

	Vertex3uvc operator-(const Vertex3uvc&) const;

	static bool intersects(const Vertex2 &, const Vertex2 &, const Vertex3uvc &, const Vertex3uvc &, const ClipSide, Vertex3uvc &);
};

#endif
