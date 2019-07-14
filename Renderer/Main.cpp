#include "stdafx.h"
#include "Main.h"

SoftwareRenderer renderer;
Camera camera;
Win32 window;
FPS fps;

bool paused = false;
bool running = true;

int main(int argc, char *argv[]) {
	init();

	renderer.buffer = &buffer;
	window.bits = (void **)(&buffer.bits);
	window.resize_callback = resize;
	window.keypress_callback = keypress;
	window.draw = draw;
	fps.get_system_ticks = Win32::get_system_ticks;

	if (
		renderer.init()
		&& buffer.init(window.width, window.height)
		&& window.init()
	) {
		camera.set_pos(0.0f, 0.0f, 9.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		fps.init();
		//fps.set_rate(60);

		run();
	}

	window.unload();
	buffer.unload();
	renderer.unload();

	return 0;
}

void init() {
	init_lookups();
	init_geom();
}

void init_geom() {
	lights.reserve(255);
	wlights.reserve(255);
	slights.reserve(255);

	verts.reserve(255);
	wverts.reserve(255);
	sverts.reserve(255);
	vnormals.reserve(255);

	tris.reserve(255);

	Vertex3uvc v1, v2, v3, v4, v5, v6, v7, v8;

	v1.x =  1.0f; v1.y =  1.0f; v1.z =  1.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x =  1.0f; v2.y =  1.0f; v2.z = -1.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x =  1.0f; v3.y = -1.0f; v3.z =  1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x =  1.0f; v4.y = -1.0f; v4.z = -1.0f; v4.u = 0.0f; v4.v = 1.0f;
	v5.x = -1.0f; v5.y =  1.0f; v5.z =  1.0f; v5.u = 0.0f; v5.v = 0.0f;
	v6.x = -1.0f; v6.y =  1.0f; v6.z = -1.0f; v6.u = 1.0f; v6.v = 0.0f;
	v7.x = -1.0f; v7.y = -1.0f; v7.z =  1.0f; v7.u = 1.0f; v7.v = 1.0f;
	v8.x = -1.0f; v8.y = -1.0f; v8.z = -1.0f; v8.u = 0.0f; v8.v = 1.0f;

	verts.push_back(v1);
	verts.push_back(v2);
	verts.push_back(v3);
	verts.push_back(v4);
	verts.push_back(v5);
	verts.push_back(v6);
	verts.push_back(v7);
	verts.push_back(v8);

	for(int i = 0; i < 8; ++i) {
		Vertex3uvc v1, v2;
		Vertex3 v3;

		wverts.push_back(v1);
		sverts.push_back(v2);
		vnormals.push_back(v3);
	}

	tris.push_back({ 1, 0, 2, 0 });
	tris.push_back({ 3, 1, 2, 0 });
	tris.push_back({ 5, 1, 3, 0 });
	tris.push_back({ 7, 5, 3, 0 });
	tris.push_back({ 4, 5, 7, 0 });
	tris.push_back({ 6, 4, 7, 0 });
	tris.push_back({ 0, 4, 6, 0 });
	tris.push_back({ 2, 0, 6, 0 });
	tris.push_back({ 7, 3, 2, 0 });
	tris.push_back({ 6, 7, 2, 0 });
	tris.push_back({ 0, 1, 5, 0 });
	tris.push_back({ 4, 0, 5, 0 });

	Vertex3c l1;
	l1.x = 0.0f; l1.y = 1.0f; l1.z = 2.0f; l1.r = 1.0f; l1.g = 0.0f; l1.b = 0.0f;
	lights.push_back(l1);

	Vertex3c l2;
	l2.x = 0.0f; l2.y = -1.0f; l2.z = -2.0f; l2.r = 0.0f; l2.g = 1.0f; l2.b = 1.0f;
	lights.push_back(l2);

	Vertex3c l3;
	l3.x = 0.0f; l3.y = 0.0f; l3.z = -3.0f; l3.r = 0.0f; l3.g = 1.0f; l3.b = 0.0f;
	lights.push_back(l3);

	for (int i = 0; i < 3; ++i) {
		Vertex3c l1, l2;
		wlights.push_back(l1);
		slights.push_back(l2);
	}
}

void init_lookups() {
	const float mul = PI / (SINCOSMAX * 0.5f);

	float val;
	for (int i = 0; i < SINCOSMAX; ++i) {
		val = i * mul;

		sins[i] = (float)sin(val);
		isins[i] = 1.0f / sins[i];

		coss[i] = (float)cos(val);
		icoss[i] = 1.0f / coss[i];
	}
}

void run() {
	do {
		// input
		if (!window.update()) { running = false; }

		if (!paused) {
			fps.update();

			// simulate
			update();

			if (fps.update_frame) {
				// render
				draw();
			}
		}
	} while (running);

	window.close();
}

inline bool resize(int w, int h) {
	if (!buffer.init(w, h)) { return false; }
	bool res = renderer.resize(w, h);
	draw();

	return res;
}

void keypress(uint wparam) {
	switch (wparam) {
		case 0x1B:
			running = false;
			break;
		case 0x20:
			paused = !paused;
			break;
	}
}

inline void update() {
	camera.update();

	renderer.world = camera.look();
}

inline void draw() {
	renderer.render();

	window.swap_buffers();
}