#pragma once
#ifndef WINBUFFER_H
#define WINBUFFER_H

#include <windows.h>

class WinBuffer {
public:
	WinBuffer();

	bool init(HDC, ulong, ulong);
	void unload();

	inline void clear(ulong);
	inline bool is_buffer();

	~WinBuffer() { }

	HBITMAP dib;
	HDC dc;
	void *bits;
	ulong width, height;
	ulong mid_width, mid_height;
	size_t size;
};

#endif