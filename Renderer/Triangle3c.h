#pragma once
#ifndef TRIANGLE3C_H
#define TRIANGLE3C_H

#include "Vertex3c.h"

struct Triangle3c {
	union {
		struct { Vertex3c v1, v2, v3; };
		struct { Vertex3c v[3]; };
	};

	int texture_id;
};

#endif
