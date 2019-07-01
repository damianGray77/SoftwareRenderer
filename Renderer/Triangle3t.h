#pragma once
#ifndef TRIANGLE3T_H
#define TRIANGLE3T_H

struct Triangle3t {
	union {
		struct { int v1, v2, v3; };
		struct { int v[3]; };
	};

	int texture_id;
};

#endif
