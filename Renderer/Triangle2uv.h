#pragma once
#ifndef TRIANGLE2UV_H
#define TRIANGLE2UV_H

#include "Vertex2uv.h"

struct Triangle2uv {
	union {
		struct { Vertex2uv v1, v2, v3; };
		struct { Vertex2uv v[3]; };
	};

	int texture_id;
};

#endif
