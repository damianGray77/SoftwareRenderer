#include "stdafx.h"
#include "Core.h"

std::vector<Vertex3c> lights;
std::vector<Vertex3> wlights;
std::vector<Vertex3c> slights;
std::vector<Vertex3uvc> verts;
std::vector<Vertex3> wverts;
std::vector<Vertex3uvc> sverts;
std::vector<Vertex3> vnormals;
std::vector<Triangle3t> tris;

const char *texture_files[] = { "t2.bmp" };

float sins[SINCOSMAX] = { 0 };
float coss[SINCOSMAX] = { 0 };
float isins[SINCOSMAX] = { 0 };
float icoss[SINCOSMAX] = { 0 };

float isqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = *(long *)&y;							// evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);					// what the fuck?
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));		// 1st iteration
	y = y * (threehalfs - (x2 * y * y));		// 2nd iteration, this can be removed

	return y;
}