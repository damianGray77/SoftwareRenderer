#pragma once
#ifndef WINDOWS_H
#define WINDOWS_H

#include <windows.h>
#include <mmsystem.h>

#include "resource.h"
#include "Buffer.h"

class Win32 {
public:
	HWND window;
	HINSTANCE instance;
	HDC front_dc;
	HDC back_dc;
	HBITMAP dib;
	BITMAPINFO info;

	RECT rect;
	ulong width;
	ulong height;
	ushort color_depth;
	bool fullscreen;
	LPCWSTR cname;
	LPCWSTR wname;
	int cores;
	Buffer *buffer;
	MSG msg;

	bool (*resize_callback)(int, int);
	void (*draw)();
	void (*keypress_callback)(uint);

	static Win32 *self;

	Win32();
	~Win32() { }

	static LRESULT CALLBACK proc(HWND, uint, WPARAM, LPARAM);

	bool init();
	bool init_window();
	bool init_buffer();
	void unload();
	void unload_buffer();
	bool full_screen();
	bool swap_buffers();
	bool resize(ulong, ulong);
	bool update();

	void close();
	ulong get_system_ticks();
	void set_title(wchar_t *);
private:
	LRESULT CALLBACK _proc(HWND, uint, WPARAM, LPARAM);
};

#endif
