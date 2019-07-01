#include "stdafx.h"
#include "WinBuffer.h"

WinBuffer::WinBuffer() {
	dib = NULL;
	dc = NULL;
}

bool WinBuffer::init(HDC context, ulong w, ulong h) {
	BITMAPINFO info;
	memset(&info, 0, sizeof(info));

	info.bmiHeader = {
		  sizeof(BITMAPINFOHEADER)
		, (long)w
		, -(long)h
		, 1
		, 32
		, BI_RGB
		, (ulong)(w * h * 4)
		, 0L
		, 0L
		, (ulong)0
		, (ulong)0
	};

	dib = CreateDIBSection(context, &info, DIB_RGB_COLORS, &bits, NULL, 0x0);
	if (NULL == dib) {
		return false;
	}

	dc = CreateCompatibleDC(context);
	if (NULL == dc) {
		return false;
	}

	SelectObject(dc, dib);

	width = w;
	mid_width = (ulong)(w * 0.5f);
	height = h;
	mid_height = (ulong)(h * 0.5f);
	size = w * h * sizeof(ulong);

	return true;
}

void WinBuffer::unload() {
	if (NULL != dc) {
		DeleteDC(dc);
		dc = NULL;
	}

	if (NULL != dib) {
		DeleteObject(dib);
		dib = NULL;
	}

	bits = NULL;

	width = 0;
	height = 0;
}

inline void WinBuffer::clear(ulong c) {
	memset(bits, c, size);
}

inline bool WinBuffer::is_buffer() {
	return NULL != dib;
}