#include "stdafx.h"
#include "Main.h"

SoftwareRenderer renderer;
Camera camera;
WindowsHandler window;

int main(int argc, char *argv[]) {
	init();

	window.resize_callback = resize;
	window.keypress_callback = keypress;
	window.draw = draw;

	if (renderer.init() && window.init()) {
		camera.set_pos(0.0f, 0.0f, 9.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		renderer.bits = (ulong *)window.buffer.bits;
		run();
	}

	window.unload();
	renderer.unload();

	return 0;
}

void init() {
	init_lookups();
	init_geom();
}

void init_geom() {
	verts.reserve(255);
	wverts.reserve(255);
	sverts.reserve(255);

	tris.reserve(255);

	Vertex3uv v1, v2, v3, v4, v5, v6, v7, v8;

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
		Vertex3uv v1, v2;

		wverts.push_back(v1);
		sverts.push_back(v2);
	}

	tris.push_back({ 0, 1, 2, 0 });
	tris.push_back({ 1, 3, 2, 0 });
	tris.push_back({ 1, 5, 3, 0 });
	tris.push_back({ 5, 7, 3, 0 });
	tris.push_back({ 5, 4, 7, 0 });
	tris.push_back({ 4, 6, 7, 0 });
	tris.push_back({ 4, 0, 6, 0 });
	tris.push_back({ 0, 2, 6, 0 });
	tris.push_back({ 3, 7, 2, 0 });
	tris.push_back({ 7, 6, 2, 0 });
	tris.push_back({ 1, 0, 5, 0 });
	tris.push_back({ 0, 4, 5, 0 });
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
		if (!window.handle_messages()) {
			break;
		}

		window.update();
	} while (true);
}

inline bool resize(int w, int h) {
	return renderer.resize(w, h, (ulong *)window.buffer.bits);
}

void keypress(uint wparam) {
	switch (wparam) {
		case 0x1B:
			window.close();
			break;
	}
}

inline void draw() {
	camera.update();

	renderer.world = camera.look();

	renderer.render();
}