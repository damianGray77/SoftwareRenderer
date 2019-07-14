#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Bitmap.h"

class Texture {
public:
	void init(uint, uint);
	void unload();

	inline ulong point(uint x, uint y) {
		return *(bits + y * pitch + x);
		//return *tile_point(x, y);
	}

	inline ulong *tile_point(uint x, uint y) {
		uint no_tiles = (width + 3) >> 2;

		uint tileX = x >> 2;
		uint tileY = y >> 2;
		uint inTileX = x & 3;
		uint inTileY = y & 3;

		return &bits[((tileY * no_tiles + tileX) << 4) + (inTileY << 2) + inTileX];
	}

	inline ulong bi_point(const float u, const float v) {
		//u -= 0.5f; v -= 0.5f;

		const int x = (int)u;
		const int y = (int)v;
		const float up = u - x;
		const float vp = v - y;
		const float iup = 1.0f - up;
		const float ivp = 1.0f - vp;

		const ulong y1 = y * pitch;
		const ulong y2 = y1 + pitch;
		const ulong *x1 = bits + x;
		const ulong *x2 = x1 + 1;

		const uchar *c00 = (uchar *)(x1 + y1);
		const uchar *c01 = (uchar *)(x2 + y1);
		const uchar *c10 = (uchar *)(x1 + y2);
		const uchar *c11 = (uchar *)(x2 + y2);

		//const uchar *c00 = (uchar *)tile_point(x, y);
		//const uchar *c01 = (uchar *)tile_point(x + 1, y);
		//const uchar *c10 = (uchar *)tile_point(x, y + 1);
		//const uchar *c11 = (uchar *)tile_point(x + 1, y + 1);

		return
			  (uchar)(((c00[0] * iup + c01[0] * up) * ivp) + ((c10[0] * iup + c11[0] * up) * vp))
			| (uchar)(((c00[1] * iup + c01[1] * up) * ivp) + ((c10[1] * iup + c11[1] * up) * vp)) << 8
			| (uchar)(((c00[2] * iup + c01[2] * up) * ivp) + ((c10[2] * iup + c11[2] * up) * vp)) << 16
		;
	}

	uint width;
	uint height;
	uint pitch;
	ulong *bits;

	static Texture* load_from_bitmap(Bitmap &);
};

#endif
