#include "stdafx.h"
#include "Buffer.h"

bool Buffer::init(const ulong w, const ulong h) {
	width = w;
	mid_width = (ulong)(w * 0.5f);
	height = h;
	mid_height = (ulong)(h * 0.5f);
	size = w * h * sizeof(ulong);

	return true;
}

void Buffer::unload() {
	bits = NULL;

	width = 0;
	height = 0;
}

void Buffer::clear(const ulong c) {
	memset(bits, c, size);
}