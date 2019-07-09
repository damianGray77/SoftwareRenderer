#pragma once
#ifndef CORE_H
#define CORE_H

#include "Vertex3uv.h"
#include "Edge3uv.h"
#include "Triangle3t.h"
#include <vector>

#define EPSILON 0 //1e-4
#define PI 3.14159265358f
#define SINCOSMAX 32767

#define SWAP(type, i, j) { type _temp_var = i; i = j; j = _temp_var; }
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define CEIL(f) ((int)(f) + (f > (int)(f)))
#define FLOOR(f) ((int)(f) - (f < (int)(f)))
#define MIN(x, y) (x <= y ? x : y)
#define MAX(x, y) (x >= y ? x : y)
#define CLAMP(x, min, max) (x <= min ? min : x >= max ? max : x)
#define WRAP(x, min, max) (x <= min ? x + max : x >= max ? x - max : x)

typedef int fixed8;
typedef int fixed12;
typedef int fixed16;
typedef int fixed24;

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t  int8;

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;

typedef unsigned long long ulonglong;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

inline fixed8 fl_f8(const float f) {
	return (fixed8)(f * 256.0f);
}

inline fixed8 f8_div(const fixed8 a, const fixed8 b) {
	return (a << 8) / b;
}

inline fixed8 f8_mul(const fixed8 a, const fixed8 b) {
	return (a * b) >> 8;
}

inline int f8_int(const fixed8 f) {
	return f >> 8;
}

inline float f8_fl(const fixed8 f) {
	return ((float)f) * 0.00390625f; // 1 / 256th
}

inline fixed16 fl_f16(const float f) {
	return (fixed16)(f * 65536.0f);
}

inline fixed16 f16_div(const fixed16 a, const fixed16 b) {
	return (((long long)a) << 16) / b;
}

inline fixed16 f16_mul(const fixed16 a, const fixed16 b) {
	return (fixed16)((((long long)a) * ((long long)b)) >> 16);
}

inline int f16_int(const fixed16 f) {
	return f >> 16;
}

inline float f16_fl(const fixed16 f) {
	return ((float)f) * 0.0000152587890625f; // 1 / 65536th
}


#define RGBAb(r, g, b, a) (((ulong)a) << 24 | ((ulong)r) << 16 | ((ulong)g) << 8 | ((ulong)b))
#define RGBAf(r, g, b, a) (((ulong)(a * 255)) << 24 | ((ulong)(r * 255)) << 16 | ((ulong)(g * 255)) << 8 | ((ulong)(b * 255)))

#define RGBb(r, g, b) (4278190080 | ((ulong)r) << 16 | ((ulong)g) << 8 | ((ulong)b))
#define RGBf(r, g, b) (4278190080 | ((ulong)(r * 255)) << 16 | ((ulong)(g * 255)) << 8 | ((ulong)(b * 255)))

#define SIN(x) (sins[x & SINCOSMAX])
#define COS(x) (coss[x & SINCOSMAX])
#define ISIN(x) (isins[x & SINCOSMAX])
#define ICOS(x) (icoss[x & SINCOSMAX])

extern float sins[SINCOSMAX];
extern float coss[SINCOSMAX];
extern float isins[SINCOSMAX];
extern float icoss[SINCOSMAX];

extern std::vector<Vertex3uv> verts;
extern std::vector<Vertex3> wverts;
extern std::vector<Vertex3uv> sverts;
extern std::vector<Triangle3t> tris;

extern const char *texture_files[];

float isqrt(float);

#endif
