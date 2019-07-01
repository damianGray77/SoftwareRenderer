#pragma once
#ifndef WINDOWS_H
#define WINDOWS_H

#include <windows.h>
#include <mmsystem.h>

#include "resource.h"
#include "WinBuffer.h"

class WindowsHandler {
public:
	HWND window;
	HINSTANCE instance;
	HDC context;
	RECT rect;
	ulong width;
	ulong height;
	char color_depth;
	bool fullscreen;
	LPCWSTR cname;
	LPCWSTR wname;
	float delta;
	int cores;
	WinBuffer buffer;
	MSG msg;
	bool resizing = false;

	bool (*resize_callback)(int, int);
	void (*draw)();
	void (*keypress_callback)(WPARAM);

	static WindowsHandler *self;

	WindowsHandler();
	~WindowsHandler() { }

	static LRESULT CALLBACK proc(HWND, uint, WPARAM, LPARAM);

	bool init();
	void unload();
	bool full_screen();
	bool swap_buffers();
	bool resize(ulong, ulong);
	bool handle_messages();
	void show_fps();
	bool clamp_fps(const int);
	void update();

	void close();

private:
	LRESULT CALLBACK _proc(HWND, uint, WPARAM, LPARAM);
};

#endif
