#pragma once
#ifndef TRIANGLE2_H
#define TRIANGLE2_H

#include "Vertex2.h"

struct Triangle2 {
	union {
		struct { Vertex2 v1, v2, v3; };
		struct { Vertex2 v[3]; };
	};
};

#endif
