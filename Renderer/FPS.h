#pragma once
#ifndef FPS_H
#define FPS_H

class FPS {
	ulong frames_since;
	ulong next_calc_ticks;
	ulong last_ticks;
	float frame_ticks;
	ulong current_ticks;
	ulong total_frames;
	ulong seconds;

	float target;
public:
	bool update_frame;

	ulong(*get_system_ticks)();

	void init();
	void set_rate(const int);
	void update();
};

#endif