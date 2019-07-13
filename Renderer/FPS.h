#pragma once
#ifndef FPS_H
#define FPS_H

#include "Win32.h"

class FPS {
	ulong frames_since;
	ulong last_calc_ticks;
	ulong last_ticks;
	float frame_ticks;
	ulong current_ticks;
	ulong total_frames;
	ulong seconds;

	float target;

	wchar_t frame_rate[50];

	Win32 *window;
public:
	bool update_frame;

	void init(Win32 *);
	void set_rate(const int);
	void update();
};

#endif