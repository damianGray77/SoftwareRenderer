#pragma once
#ifndef BUFFER_H
#define BUFFER_H

class Buffer {
public:
	Buffer() { }

	bool init(const ulong, const ulong);
	void unload();

	void clear(const ulong);

	ulong *bits;
	ulong width, height;
	ulong mid_width, mid_height;
	size_t size;
};

#endif
