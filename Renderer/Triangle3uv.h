#pragma once
#ifndef TRIANGLE3UV_H
#define TRIANGLE3UV_H

#include "Vertex3uv.h"

struct Triangle3uv {
	union {
		struct { Vertex3uv v1, v2, v3; };
		struct { Vertex3uv v[3]; };
	};

	int texture_id;
};

#endif
