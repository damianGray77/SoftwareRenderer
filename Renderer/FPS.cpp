#include "stdafx.h"
#include "FPS.h"


void FPS::init(Win32 *w) {
	window = w;

	frames_since = 0;
	last_calc_ticks = 0;
	last_ticks = window->get_system_ticks();
	frame_ticks = 0;
	current_ticks = 0;
	total_frames = 0;
	seconds = 0;
	target = 0;
}

void FPS::set_rate(const int rate) {
	target = 1000.0f / rate;
}

void FPS::update() {
	current_ticks = window->get_system_ticks();

	frame_ticks += (float)(current_ticks - last_ticks);
	last_ticks = current_ticks;

	if (frame_ticks >= target) {
		++frames_since;

		frame_ticks -= target;

		update_frame = true;
	}
	else {
		update_frame = false;
	}

	if (current_ticks - last_calc_ticks > 1000) {
		last_calc_ticks = current_ticks;
		total_frames += frames_since;

		swprintf(frame_rate, sizeof(wchar_t) * 50, L"FPS: %d Avg.: %d Ms/F: %f", frames_since, CEIL(total_frames / (float)seconds), 1000.0f / frames_since);
		window->set_title(frame_rate);

		frames_since = 0;
		++seconds;
	}
}