#include "stdafx.h"
#include "Win32.h"

Win32 *Win32::self = NULL;

Win32::Win32() {
	self = this;

	window = NULL;
	front_dc = NULL;
	back_dc = NULL;
	instance = NULL;

	width = 640;
	height = 480;
	rect = { 0, 0, (long)width, (long)height };
	color_depth = 32;
	fullscreen = true;
	cname = L"SoftwareRenderer";
	wname = L"SoftwareRenderer";
	cores = 1;

	memset(&msg, 0, sizeof(msg));
}

// This is a static method. This is my workaround to get it to play nicely within an instanced object.
LRESULT CALLBACK Win32::proc(HWND window, uint msg, WPARAM wparam, LPARAM lparam) {
	return self->_proc(window, msg, wparam, lparam);
}

LRESULT CALLBACK Win32::_proc(HWND window, uint msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_SIZE:
			{
				short raw_w = LOWORD(lparam);
				short raw_h = HIWORD(lparam);

				uint w = raw_w <= 64 ? 64 : raw_w;
				uint h = raw_h <= 64 ? 64 : raw_h;

				if (resize(w, h)) {
					resize_callback(w, h);
				}
			}

			return 0;
		case WM_KEYDOWN:
			keypress_callback(wparam);

			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);

			return 0;
	}

	return DefWindowProc(window, msg, wparam, lparam);
}

bool Win32::init() {
	timeBeginPeriod(1);

	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);

	if (!init_window()) {
		MessageBoxW(window, L"Cannot init window!", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!init_buffer()) {
		MessageBoxW(window, L"Cannot init buffer!", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(window, SW_SHOWNORMAL);
	UpdateWindow(window);

	SetFocus(window);
	if (!fullscreen) {
		SetForegroundWindow(window);
	}

	GetClientRect(window, &rect);

	return true;
}

bool Win32::init_window() {
	instance = (HINSTANCE)GetModuleHandleW(NULL);

	WNDCLASSEXW wc = {
		  sizeof(WNDCLASSEX)
		, CS_HREDRAW | CS_VREDRAW
		, proc
		, 0
		, 0
		, instance
		, LoadIconW(instance, MAKEINTRESOURCE(IDI_RENDERER))
		, LoadCursorW(nullptr, IDC_ARROW)
		, (HBRUSH)(COLOR_WINDOW + 1)
		, MAKEINTRESOURCEW(IDC_RENDERER)
		, cname
		, LoadIconW(instance, MAKEINTRESOURCE(IDI_SMALL))
	};

	RegisterClassExW(&wc);

	fullscreen = IDYES == MessageBoxW(NULL, L"Click Yes to go to full screen (Recommended)", L"Options", MB_YESNO | MB_ICONQUESTION);

	ulong style, style_ex;
	if (fullscreen) {
		style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		style_ex = 0;

		if (!full_screen()) { return false; }

		ShowCursor(false);
	} else {
		style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		style_ex = WS_EX_CLIENTEDGE;
	}

	AdjustWindowRectEx(&rect, style, false, style_ex);

	window = CreateWindowExW(style_ex, wc.lpszClassName, wname, style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, wc.hInstance, NULL);
	if (!window) { return false; }

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	cores = sysinfo.dwNumberOfProcessors;

	front_dc = GetDC(window);

	memset(&info, 0, sizeof(info));

	info.bmiHeader = {
		  sizeof(BITMAPINFOHEADER)
		, (long)width
		, -(long)height
		, 1
		, color_depth
		, BI_RGB
		, width * height * 4UL
		, 0
		, 0
		, 0
		, 0
	};

	return true;
}

bool Win32::init_buffer() {
	info.bmiHeader.biWidth = (long)width;
	info.bmiHeader.biHeight = -(long)height;
	info.bmiHeader.biSizeImage = width * height * 4UL;

	void *bits;

	dib = CreateDIBSection(front_dc, &info, DIB_RGB_COLORS, &bits, NULL, 0);
	if (NULL == dib) { return false; }

	buffer->bits = (ulong *)bits;

	back_dc = CreateCompatibleDC(front_dc);
	if (NULL == back_dc) { return false; }

	SelectObject(back_dc, dib);

	return true;
}

void Win32::unload() {
	timeEndPeriod(1);

	if (fullscreen) {
		ChangeDisplaySettingsW(NULL, 0);
		ShowCursor(true);
	}

	unload_buffer();

	UnregisterClassW(cname, instance);
}

void Win32::unload_buffer() {
	if (NULL != back_dc) {
		DeleteDC(back_dc);
		back_dc = NULL;
	}

	if (NULL != dib) {
		DeleteObject(dib);
		dib = NULL;
	}

	buffer->unload();
}

bool Win32::full_screen() {
	DEVMODE settings;
	memset(&settings, 0, sizeof(settings));

	if (!EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &settings)) {
		MessageBoxW(NULL, L"Could Not Enum Display Settings", L"Error", MB_OK);
		return false;
	}

	settings.dmPelsWidth = width;
	settings.dmPelsHeight = height;
	settings.dmColor = color_depth;

	int result = ChangeDisplaySettingsW(&settings, CDS_FULLSCREEN);

	if (DISP_CHANGE_SUCCESSFUL != result) {
		MessageBoxW(NULL, L"Display Mode Not Compatible", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Win32::swap_buffers() {
	/*return 0 == StretchDIBits(front_dc
		, 0, 0, width, height
		, 0, 0, width, height
		, buffer->bits
		, &info
		, DIB_RGB_COLORS
		, SRCCOPY
	);*/

	return BitBlt(front_dc, 0, 0, width, height, back_dc, 0, 0, SRCCOPY);
}

bool Win32::resize(ulong w, ulong h) {
	width = w;
	height = h;

	GetWindowRect(window, &rect);

	unload_buffer();
	return init_buffer();
}

bool Win32::update() {
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (WM_QUIT == msg.message) { return false; }

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

void Win32::close() {
	SendMessageW(window, WM_CLOSE, 0, 0);
}

ulong Win32::get_system_ticks() {
	return timeGetTime();
}

void Win32::set_title(wchar_t *str) {
	SetWindowTextW(window, str);
}