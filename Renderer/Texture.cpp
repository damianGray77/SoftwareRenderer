#include "stdafx.h"
#include "Texture.h"

void Texture::init(const uint w, const uint h) {
	width = w - 1;
	height = h - 1;
	pitch = w;

	bits = new ulong[w * h];
}

void Texture::unload() {
	if (NULL != bits) {
		delete[](bits);
		bits = NULL;
	}
}

Texture* Texture::load_from_bitmap(Bitmap &bitmap) {
	uint w = bitmap.w;
	uint h = bitmap.h;

	Texture *t = new Texture();
	t->init(w, h);

	ulong *tybits = t->bits;
	ulong *bybits = bitmap.data;
	for (uint y = 0, ylen = h - 1; y <= ylen; ++y) {
		for(uint x = 0, xlen = w - 1; x <= xlen; ++x) {
			//*t->tile_point(x, y) = bybits[x];
			tybits[x] = bybits[x];
		}

		tybits += w;
		bybits += w;
	}

	return t;
}