#include "stdafx.h"
#include "WindowsHandler.h"

WindowsHandler *WindowsHandler::self = NULL;

WindowsHandler::WindowsHandler() {
	self = this;

	window = NULL;
	context = NULL;
	instance = NULL;

	width = 640;
	height = 480;
	rect = { 0, 0, (long)width, (long)height };
	color_depth = 32;
	fullscreen = true;
	cname = L"SoftwareRenderer";
	wname = L"SoftwareRenderer";
	delta = 0.0f;
	cores = 1;

	memset(&msg, 0, sizeof(msg));
}

LRESULT CALLBACK WindowsHandler::proc(HWND window, uint msg, WPARAM wparam, LPARAM lparam) {
	return self->_proc(window, msg, wparam, lparam);
}

LRESULT CALLBACK WindowsHandler::_proc(HWND window, uint msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		//case WM_SIZING:
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

			InvalidateRect(window, NULL, FALSE);

			return 0;
		case WM_ERASEBKGND: return 0;
		/*case WM_PAINT:
			if(clamp_fps(30)) {
				show_fps();

				paint_callback();

				if (!swap_buffers()) {
					PostQuitMessage(0);
				}
			}

			return 0;*/
		case WM_KEYDOWN:
			keypress_callback(wparam);

			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);

			return 0;
	}

	return DefWindowProc(window, msg, wparam, lparam);
}

bool WindowsHandler::init() {
	timeBeginPeriod(2);

	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);

	instance = (HINSTANCE)GetModuleHandleW(NULL);

	WNDCLASSEXW wc = {
		sizeof(WNDCLASSEX)
		, CS_HREDRAW | CS_VREDRAW
		, proc
		, 0L
		, 0L
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
	}
	else {
		style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		style_ex = WS_EX_CLIENTEDGE;
	}

	AdjustWindowRectEx(&rect, style, false, style_ex);

	window = CreateWindowExW(style_ex, cname, wname, style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, wc.hInstance, NULL);
	if (!window) { return false; }

	this->window = window;

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	cores = sysinfo.dwNumberOfProcessors;

	context = GetDC(window);

	if (!buffer.init(context, width, height)) {
		MessageBoxW(window, L"Cannot init buffer!", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(window, SW_SHOWNORMAL);
	UpdateWindow(window);

	SetFocus(window);
	if (!fullscreen) {
		SetForegroundWindow(window);
	}

	return true;
}

void WindowsHandler::unload() {
	if (fullscreen) {
		ChangeDisplaySettingsW(NULL, 0);
		ShowCursor(true);
	}

	buffer.unload();

	UnregisterClassW(cname, instance);
}

bool WindowsHandler::full_screen() {
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

bool WindowsHandler::swap_buffers() {
	GetClientRect(window, &rect);

	if (!BitBlt(context, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, buffer.dc, 0, 0, SRCCOPY)) {
		MessageBoxW(window, L"Cannot swap buffers!", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool WindowsHandler::resize(ulong w, ulong h) {
	width = w;
	height = h;
	GetWindowRect(window, &rect);

	buffer.unload();
	if (!buffer.init(context, w, h)) {
		MessageBoxW(window, L"Cannot reinit buffer!", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return false;
	}

	return true;
}

void WindowsHandler::update() {
	/*if (!clamp_fps(120)) {
		Sleep(0);
		return;
	}*/

	show_fps();

	draw();

	if (!swap_buffers()) {
		PostQuitMessage(0);
	}
}

bool WindowsHandler::handle_messages() {
	while (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		if (WM_QUIT == msg.message) { return false; }

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}

void WindowsHandler::close() {
	SendMessageW(window, WM_CLOSE, 0, 0);
}

void WindowsHandler::show_fps() {
	static int fps = 0;
	static float last = 0;
	static float frame = 0;

	const float current = timeGetTime() * 0.001f;

	frame = current;
	++fps;

	if (current - last > 1) {
		last = current;

		wchar_t frame_rate[50];
		swprintf(frame_rate, sizeof(wchar_t) * 50, L"FPS: %d", fps);
		SetWindowTextW(window, frame_rate);
		fps = 0;
	}
}

bool WindowsHandler::clamp_fps(const int rate) {
	static float last = timeGetTime();
	static float elapsed = 0;

	const float current = timeGetTime();
	const float delta = current - last;
	const float fps = 1000.0f / rate;

	elapsed += delta;
	last = current;

	if (elapsed > fps) {
		elapsed -= fps;

		return true;
	}

	return false;
}