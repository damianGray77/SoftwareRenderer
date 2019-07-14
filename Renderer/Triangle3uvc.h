#pragma once
#ifndef TRIANGLE3UVC_H
#define TRIANGLE3UVC_H

#include "Vertex3uvc.h"

struct Triangle3uvc {
	union {
		struct { Vertex3uvc v1, v2, v3; };
		struct { Vertex3uvc v[3]; };
	};

	int texture_id;
};

#endif
