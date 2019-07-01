#pragma once
#ifndef SPAN_H
#define SPAN_H

struct Span {
	int x1, x2;
	float subx;

	float z1, z2;

	fixed8 fz1, fz2;

	float r1, r2;
	float g1, g2;
	float b1, b2;
	float a1, a2;

	float u1, u2;
	float v1, v2;

	fixed8 fu1, fu2;
	fixed8 fv1, fv2;

	int off;
	int len;

	ulong *texture;
};

#endif
