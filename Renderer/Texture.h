#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "Bitmap.h"

class Texture {
public:
	void init(uint, uint);
	void unload();
	
	inline ulong point(uint x, uint y) {
		return bits[y * pitch + x];
	}

	uint width;
	uint height;
	uint pitch;
	ulong *bits;

	static Texture* load_from_bitmap(Bitmap &);
};

#endif
