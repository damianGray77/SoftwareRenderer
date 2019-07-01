#pragma once
#ifndef EDGE3UV_H
#define EDGE3UV_H

struct Edge3uv {
	int y1, y2;
	float* z;
	float* u;
	float* v;
	float* x;

	bool initd;

	void init(int, int);
	void unload();
};

#endif;
