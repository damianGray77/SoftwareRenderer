#pragma once
#ifndef SOFTWARERENDERER_H
#define SOFTWARERENDERER_H

#include "Buffer.h"

#include "Span.h"
#include "Vertex2.h"
#include "Vertex2c.h"
#include "Vertex2uv.h"
#include "Vertex3uv.h"
#include "Triangle3t.h"
#include "Triangle3uv.h"
#include "Matrix4x4.h"
#include "Bitmap.h"
#include "Texture.h"
#include <vector>

class SoftwareRenderer {
public:
	SoftwareRenderer();
	~SoftwareRenderer() {}

	bool init();
	void render();
	int backfacecull_and_clip();
	bool resize(const int, const int);
	void unload();
	void slice_poly(const Vertex2 &, const Vertex2 &, const ClipSide);

	void set_fov(float);
	void set_clip(long, long);
	void recalc_dist();
	void project(const int, const Matrix4x4 &);

	void draw_line(const Vertex2 &, const Vertex2 &, const ulong);
	void draw_triangle_flat(Vertex2 &, Vertex2 &, Vertex2 &, const ulong);
	void draw_triangle_texture(Vertex2uv &, Vertex2uv &, Vertex2uv &, const int);
	void draw_triangle_texture_a(Triangle3uv &);
	void draw_triangle_texture_p(Triangle3uv &);
	void draw_bitmap(const Vertex2 &, const int);

	//void draw_quad_flat(Vertex2 &, Vertex2 &, Vertex2 &, Vertex2 &, const ulong);


	bool load_textures();
	void unload_textures();

	Buffer *buffer;

	/*ulong *bits;
	ulong width, height;
	ulong mwidth, mheight;*/

	float _w;
	float _h;

	float fov, dist, cscale;
	long cnear, cfar;
	Matrix4x4 world;
	Matrix4x4 iworld;

	long *yoffs;

	Vertex2 *bounds;

	Triangle3uv ctris[255];
	std::vector<Vertex3uv> cverts;
	std::vector<Edge3uv> cedges;
	std::vector<uint> edge_map;

	Vertex3uv *clip_poly1;
	Vertex3uv *clip_poly2;
	Span *spans;
	int cp1len, cp2len;

	Texture **textures;
};

#endif
