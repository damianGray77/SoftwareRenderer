#include "stdafx.h"
#include "SoftwareRenderer.h"

enum bound {
	  TOP
	, LEFT
	, RIGHT
	, BOTTOM
};

SoftwareRenderer::SoftwareRenderer() {
	yoffs = NULL;
	bounds = NULL;
	clip_poly1 = NULL;
	clip_poly2 = NULL;
	spans = NULL;
	textures = NULL;
}

bool SoftwareRenderer::init() {
	//ctris.reserve(255);
	cverts.reserve(767);
	cedges.reserve(767);
	edge_map.reserve(255);

	for (int i = 0; i < 768; ++i) {
		Vertex3uv v;
		cverts.push_back(v);
	}

	for (int i = 0; i < 768; ++i) {
		Edge3uv e;
		cedges.push_back(e);
	}

	for (int i = 0; i < 256; ++i) {
		edge_map.push_back(0);
	}

	bounds = new Vertex2[4];
	clip_poly1 = new Vertex3uv[15];
	clip_poly2 = new Vertex3uv[15];
	spans = new Span[buffer->height];
	
	load_textures();

	return true;
}

bool SoftwareRenderer::load_textures() {
	int len =	1;

	textures = new Bitmap*[len];

	for (int i = 0; i < len; ++i) {
		textures[i] = new Bitmap(texture_files[i]);
	}

	return true;
}

void SoftwareRenderer::unload_textures() {
	if(NULL == textures) { return; }

	int len = sizeof(textures) / sizeof(Bitmap *);
	for (int i = 0; i < len; ++i) {
		textures[i]->unload();
		textures[i] = NULL;
	}

	delete[] textures;
	textures = NULL;
}

int SoftwareRenderer::backfacecull_and_clip() {
	//ctris.clear();
	//edge_map.clear();

	int clip_len = 0;

	int len = tris.size();
	for (int i = 0; i < len; ++i) {
		const Triangle3t t = tris[i];
		const Vertex3uv v1 = sverts[t.v1];
		const Vertex3uv v2 = sverts[t.v2];
		const Vertex3uv v3 = sverts[t.v3];

		float area =
			  v1.x * v2.y - v2.x * v1.y
			+ v2.x * v3.y - v3.x * v2.y
			+ v3.x * v1.y - v1.x * v3.y
		;
		if (area > 0.0f) { continue; }

		/*Edge
		cedges.push_back(e);

		cverts[t.v1] = sverts[t.v1];
		cverts[t.v2] = sverts[t.v2];
		cverts[t.v3] = sverts[t.v3];
		ctris.push_back(tris[i]);*/

		const float vx1 = v1.x;
		const float vx2 = v2.x;
		const float vx3 = v3.x;
		const float vy1 = v1.y;
		const float vy2 = v2.y;
		const float vy3 = v3.y;

		const float minx = MIN(MIN(vx1, vx2), vx3);
		const float miny = MIN(MIN(vy1, vy2), vy3);
		const float maxx = MAX(MAX(vx1, vx2), vx3);
		const float maxy = MAX(MAX(vy1, vy2), vy3);

		if(minx < 0 || miny < 0 || maxx > _w || maxy > _h) {
			cp1len = 3;
			clip_poly1[0] = v1;
			clip_poly1[1] = v2;
			clip_poly1[2] = v3;

			if (minx < 0) {
				slice_poly(bounds[3], bounds[0], left);		// { 0, h }, { 0, 0 }
				SWAP(Vertex3uv *, clip_poly2, clip_poly1);
				SWAP(int, cp2len, cp1len);
			}

			if (miny < 0) {
				slice_poly(bounds[0], bounds[1], top);		// { 0, 0 }, { w, 0 }
				SWAP(Vertex3uv *, clip_poly2, clip_poly1);
				SWAP(int, cp2len, cp1len);
			}

			if (maxx > _w) {
				slice_poly(bounds[1], bounds[2], right);	// { w, 0 }, { w, h }
				SWAP(Vertex3uv *, clip_poly2, clip_poly1);
				SWAP(int, cp2len, cp1len);
			}

			if (maxy > _h) {
				slice_poly(bounds[2], bounds[3], bottom);	// { w, h }, { 0, h }
				SWAP(Vertex3uv *, clip_poly2, clip_poly1);
				SWAP(int, cp2len, cp1len);
			}

			for (int j = 2; j < cp1len; ++j) {
				ctris[clip_len] = { clip_poly1[0], clip_poly1[j], clip_poly1[j - 1], t.texture_id };
				++clip_len;
			}
		} else if(minx >= 0 || miny >= 0 || maxx <= _w || maxy <= _h) {
			ctris[clip_len] = { v1, v2, v3, t.texture_id };
			++clip_len;
		}
	}

	return clip_len;
}

void SoftwareRenderer::slice_poly(const Vertex2 &x0, const Vertex2 &x1, const ClipSide s) {
	cp2len = 0;
	Vertex3uv v0 = clip_poly1[cp1len - 1], v1;
	int s1 = Vertex3uv::left_of(x0, x1, v0), s2;
	
	if (s1 != -1) {
		clip_poly2[cp2len] = v0;
		++cp2len;
	}

	for (int i = 0; i < cp1len; ++i) {
		v1 = clip_poly1[i];
		s2 = Vertex3uv::left_of(x0, x1, v1);
		if (s1 + s2 == 0 && s1) {
			Vertex3uv t;
			if (Vertex3uv::intersects(x0, x1, v0, v1, s, t)) {
				clip_poly2[cp2len] = t;
				++cp2len;
			}
		}

		if(i == cp1len - 1) { break; }
		if (s2 != -1) {
			clip_poly2[cp2len] = v1;
			++cp2len;
		}

		v0 = v1;
		s1 = s2;
	}
}


void SoftwareRenderer::set_fov(float f) {
	fov = f;

	recalc_dist();
}

void SoftwareRenderer::set_clip(long near, long far) {
	cnear = near;
	cfar = far;

	cscale = 1.0f / (ABS(near) + ABS(far));
}

void SoftwareRenderer::recalc_dist() {
	int m = (int)(fov * 0.5f);

	float cot = COS(m) * ISIN(m);

	dist = cot * buffer->mid_height;
}

void SoftwareRenderer::project(const int i, const Matrix4x4 &m) {
	Vertex3uv &s = sverts[i];
	Vertex3 &w = wverts[i];

	const Vertex3uv &v = verts[i];

	w = v * m;
	if(0 == w.z) { return; }

	const float z = dist / w.z;

	s.x = (w.x * z) + buffer->mid_width;
	s.y = (-w.y * z) + buffer->mid_height;
	s.z = w.z;
	s.u = v.u;
	s.v = v.v;
}

void SoftwareRenderer::render() {
	buffer->clear(RGBb(0, 0, 0));

	static float rx = 0, ry = 0, rz = 0;

	//rx += 0.275f; if (rx > SINCOSMAX) { rx -= SINCOSMAX; }
	//ry += 0.125f; if (ry > SINCOSMAX) { ry -= SINCOSMAX; }
	//rz += 0.05f;  if (rz > SINCOSMAX) { rz -= SINCOSMAX; }

	rx += 11.0f; if (rx > SINCOSMAX) { rx -= SINCOSMAX; }
	ry += 2.5f; if (ry > SINCOSMAX) { ry -= SINCOSMAX; }
	rz += 1.0f;  if (rz > SINCOSMAX) { rz -= SINCOSMAX; }

	Matrix4x4 p;
	Matrix4x4::identity(p);
	Matrix4x4::rotate(p, rx, ry, rz);
	Matrix4x4::translate(p, 0, 0, 9.0f);
	//Matrix4x4::rotate(p, 0, 0, rz);
	//Matrix4x4::translate(p, 5, 0, 0);
	//Matrix4x4::rotate(p, 0, ry, 0);
	//Matrix4x4::translate(p, 0, 0, 20.0f);

	int vlen = verts.size();
	for(int i = 0; i < vlen; ++i) {
		project(i, p);
	}

	int tlen = backfacecull_and_clip();
	float itlen = 1.0f / (float)tlen;
	for(int i = 0; i < tlen; ++i) {
		//draw_triangle_texture(ctris[i]);

		
		//float c = (float)(i + 1) * itlen;
		Triangle3uv tri = ctris[i];
		draw_triangle_flat(tri.v1, tri.v2, tri.v3, 0x00ff0000); // RGBf(c, c, c));
		//draw_triangle_flat_fixed(tri.v1, tri.v2, tri.v3, 0x00ff0000);
	}
}

bool SoftwareRenderer::resize(const int w, const int h) {
	_w = (float)(w - 1);
	_h = (float)(h - 1);

	if (NULL != yoffs) {
		delete[] yoffs;
	}

	yoffs = new long[h];
	for (int i = 0; i < h; ++i) {
		yoffs[i] = buffer->width * i;
	}

	if (NULL != spans) {
		delete[] spans;
	}
	spans = new Span[h];

	bounds[0] = {  0,  0 };
	bounds[1] = { _w,  0 };
	bounds[2] = { _w, _h };
	bounds[3] = {  0, _h };

	set_fov(SINCOSMAX / 16);
	set_clip(0, 100);

	return true;
}

void SoftwareRenderer::unload() {
	if (NULL != clip_poly1) {
		delete[] clip_poly1;
		clip_poly1 = NULL;
	}

	if (NULL != clip_poly2) {
		delete[] clip_poly2;
		clip_poly2 = NULL;
	}
	
	if (NULL != bounds) {
		delete[] bounds;
		bounds = NULL;
	}

	if (NULL != yoffs) {
		delete[] yoffs;
		yoffs = NULL;
	}

	if(NULL != spans) {
		delete[] spans;
		spans = NULL;
	}

	unload_textures();
}

void SoftwareRenderer::draw_line(const Vertex2 &v1, const Vertex2 &v2, const ulong color) {
	float vx1 = v1.x, vy1 = v1.y;
	float vx2 = v2.x, vy2 = v2.y;

	const float dy = vy1 - vy2;
	const float dx = vx1 - vx2;

	if (ABS(dy) > ABS(dx)) {
		if (vy1 > vy2) {
			SWAP(float, vy1, vy2);
			SWAP(float, vx1, vx2);
		}

		if (vy2 < 0 || vy1 > _h) {
			return;
		}

		const float sy = dx / dy;

		// constrain to bounds
		if ((vy1 < 0 || vy2 > _h) || (vx1 < 0 || vx1 > _w) || (vx2 < 0 || vx2 > _w)) {
			if ((vx1 < 0 && vx2 < 0) || (vx1 > _w && vx2 > _w)) {
				return;
			}

			const float sx = dy / dx;

			if (vy1 < 0) {
				if (vx1 > _w) {
					const float t = vy1 + (_w - vx1) * sx;

					if (t < 0) {
						vx1 -= vy1 * sy;
						vy1 = 0;
					} else {
						vy1 = t;
						vx1 = _w;
					}
				} else if (vx1 < 0) {
					const float t = vy1 - vx1 * sx;

					if (t < 0) {
						vx1 -= vy1 * sy;
						vy1 = 0;
					} else {
						vy1 = t;
						vx1 = 0;
					}
				} else {
					vx1 -= vy1 * sy;
					vy1 = 0;
				}
			} else if (vx1 < 0) {
				vy1 -= vx1 * sx;
				vx1 = 0;
			} else if (vx1 > _w) {
				vy1 -= (vx1 - _w) * sx;
				vx1 = _w;
			}

			if (vy2 > _h) {
				if (vx2 > _w) {
					const float t = vy2 + (_w - vx2) * sx;

					vy2 = t > _h ? _h : t;
				} else if(vx2 < 0) {
					const float t = vy2 - vx2 * sx;

					vy2 = t > _h ? _h : t;
				} else {
					vy2 = _h;
				}
			} else if (vx2 < 0) {
				vy2 -= vx2 * sx;
			} else if (vx2 > _w) {
				vy2 -= (vx2 - _w) * sx;
			}
		}

		const int y1 = CEIL(vy1);
		const int y2 = CEIL(vy2);

		float x = vx1;
		for (int y = y1; y <= y2; ++y) {
			*(buffer->bits + yoffs[y] + (int)x) = color;

			x += sy;
		}
	} else {
		if (vx1 > vx2) {
			SWAP(float, vx1, vx2);
			SWAP(float, vy1, vy2);
		}

		if (vx2 < 0 || vx1 > _w) {
			return;
		}

		const float sx = dy / dx;

		// constrain to bounds
		if ((vx1 < 0 || vx2 > _w) || (vy1 < 0 || vy1 > _h) || (vy2 < 0 || vy2 > _h)) {
			if ((vy1 < 0 && vy2 < 0) || (vy1 > _h && vy2 > _h)) {
				return;
			}

			const float sy = dx / dy;

			if (vx1 < 0) {
				if (vy1 > _h) {
					const float t = vx1 + (_h - vy1) * sy;

					if (t < 0) {
						vy1 -= vx1 * sx;
						vx1 = 0;
					} else {
						vx1 = t;
						vy1 = _h;
					}
				} else if (vy1 < 0) {
					const float t = vx1 - vy1 * sy;

					if (t < 0) {
						vy1 -= vx1 * sx;
						vx1 = 0;
					} else {
						vx1 = t;
						vy1 = 0;
					}
				} else {
					vy1 -= vx1 * sx;
					vx1 = 0;
				}
			} else if (vy1 < 0) {
				vx1 -= vy1 * sy;
				vy1 = 0;
			} else if (vy1 > _h) {
				vx1 -= (vy1 - _h) * sy;
				vy1 = _h;
			}

			if (vx2 > _w) {
				if (vy2 > _h) {
					const float t = vx2 + (_h - vy2) * sy;

					vx2 = t > _w ? _w : t;
				} else if(vy2 < 0) {
					const float t = vx2 - vy2 * sy;

					vx2 = t > _w ? _w : t;
				} else {
					vx2 = _w;
				}
			} else if (vy2 < 0) {
				vx2 -= vy2 * sy;
			} else if (vy2 > _h) {
				vx2 -= (vy2 - _h) * sy;
			}
		}

		const int x1 = CEIL(vx1);
		const int x2 = CEIL(vx2);

		float y = vy1;
		for (int x = x1; x <= x2; ++x) {
			*(buffer->bits + yoffs[(int)y] + x) = color;

			y += sx;
		}
	}
}

//void SoftwareRenderer::draw_triangle_flat(Vertex2 &v1, Vertex2 &v2, Vertex2 &v3, const ulong c) {
//	if (v1.y > v2.y) { SWAP(Vertex2, v1, v2); }
//	if (v1.y > v3.y) { SWAP(Vertex2, v1, v3); }
//	if (v2.y > v3.y) { SWAP(Vertex2, v2, v3); }
//
//	const int yy1 = CEIL(v1.y + 0.5f);
//	const int yy3 = CEIL(v3.y + 0.5f) - 1;
//
//	const ulong pitch = buffer->width;
//
//	if (yy3 <= yy1) { return; }
//
//	const int yy2 = CEIL(v2.y + 0.5f);
//
//	const fixed8 x1 = int_f8(FLOOR(v1.x + 0.5f)), y1 = int_f8(FLOOR(v1.y + 0.5f));
//	const fixed8 x2 = int_f8(FLOOR(v2.x + 0.5f)), y2 = int_f8(FLOOR(v2.y + 0.5f));
//	const fixed8 x3 = int_f8(FLOOR(v3.x + 0.5f)), y3 = int_f8(FLOOR(v3.y + 0.5f));
//	
//	bool xswap = false;
//
//	fixed8 sx2, sx1 = f8_div(x3 - x1, y3 - y1);
//	fixed8 xx2, xx1 = x1;
//
//	//Span* s;
//
//	if (yy2 > yy1) {
//		sx2 = f8_div(x2 - x1, y2 - y1);
//
//		if (sx2 < sx1) {
//			SWAP(fixed8, sx1, sx2);
//			xswap = true;
//		}
//
//		xx1 = x1;
//		xx2 = x1;
//
//		for (int y = yy1; y < yy2; ++y) {
//			Span* s = spans + y;
//
//			if (xx1 >= xx2) {
//				s->x1 = f8_int(xx2);
//				s->x2 = f8_int(xx1);
//			} else {
//				s->x1 = f8_int(xx1);
//				s->x2 = f8_int(xx2);
//			}
//
//			xx1 += sx1;
//			xx2 += sx2;
//		}
//
//		if (xswap) {
//			xx1 = xx2;
//			sx1 = sx2;
//		}
//	} else if (x2 < x1) {
//		xswap = true;
//		xx2 = x1;
//		sx2 = sx1;
//	}
//
//	if (yy3 > yy2) {
//		sx2 = f8_div(x3 - x2, y3 - y2);
//
//		if (xswap) {
//			SWAP(fixed8, sx1, sx2);
//			xx1 = x2;
//		} else {
//			xx2 = x2;
//		}
//
//		for (int y = yy2; y <= yy3; ++y) {
//			Span* s = spans + y;
//
//			if (xx1 >= xx2) {
//				s->x1 = f8_int(xx2);
//				s->x2 = f8_int(xx1);
//			} else {
//				s->x1 = f8_int(xx1);
//				s->x2 = f8_int(xx2);
//			}
//
//			xx1 += sx1;
//			xx2 += sx2;
//		}
//	}
//	
//	ulong *ybits = buffer->bits + yy1 * pitch;
//	for (int y = yy1; y <= yy3; ++y) {
//		const Span* s = &spans[y];
//
//		int xx1 = s->x1;
//		int xx2 = s->x2 - 1;
//
//		for (int x = xx1; x <= xx2; ++x) {
//			ybits[x] = c;
//
//		}
//
//		ybits += pitch;
//	}
//}

void SoftwareRenderer::draw_triangle_flat(Vertex2 &v1, Vertex2 &v2, Vertex2 &v3, const ulong c) {
	if (v1.y > v2.y) { SWAP(Vertex2, v1, v2); }
	if (v1.y > v3.y) { SWAP(Vertex2, v1, v3); }
	if (v2.y > v3.y) { SWAP(Vertex2, v2, v3); }

	const int yy1 = CEIL(v1.y);
	const int yy2 = CEIL(v2.y);
	const int yy3 = CEIL(v3.y);

	if (yy3 <= yy1) { return; }

	const float x1 = v1.x, y1 = v1.y;
	const float x2 = v2.x, y2 = v2.y;
	const float x3 = v3.x, y3 = v3.y;

	bool xswap = false;

	float sx2, sx1 = (x3 - x1) / (y3 - y1);
	float xx2, xx1 = x1;

	if (yy2 > yy1) {
		sx2 = (x2 - x1) / (y2 - y1);

		if (sx2 < sx1) {
			SWAP(float, sx1, sx2);
			xswap = true;
		}

		xx1 = x1;
		xx2 = x1;

		for (int y = yy1; y < yy2; ++y) {
			Span *s = &spans[y];
			s->x1 = CEIL(xx1);
			s->x2 = FLOOR(xx2);

			xx1 += sx1;
			xx2 += sx2;
		}

		if (xswap) {
			xx1 = xx2;
			sx1 = sx2;
		}
	} else if (x2 < x1) {
		xswap = true;
		xx2 = x1;
		sx2 = sx1;
	}

	if (yy3 > yy2) {
		sx2 = (x3 - x2) / (y3 - y2);

		if (xswap) {
			SWAP(float, sx1, sx2);
			xx1 = x2;
		} else {
			xx2 = x2;
		}

		for (int y = yy2; y <= yy3; ++y) {
			Span *s = &spans[y];
			s->x1 = CEIL(xx1);
			s->x2 = FLOOR(xx2);

			xx1 += sx1;
			xx2 += sx2;
		}
	}

	const ulong pitch = buffer->width;
	ulong *ybits = buffer->bits + yy1 * pitch;

	for (int y = yy1; y <= yy3; ++y) {
		Span *s = &spans[y];
		int xs = s->x1;
		int xe = s->x2;

		for (int x = xs; x <= xe; ++x) {
			//assert(0 != ybits[x]);
			ybits[x] = c;
		}

		ybits += pitch;
	}
}

void SoftwareRenderer::draw_triangle_flat_fixed(Vertex2 &v1, Vertex2 &v2, Vertex2 &v3, const ulong c) {
	if (v1.y > v2.y) { SWAP(Vertex2, v1, v2); }
	if (v1.y > v3.y) { SWAP(Vertex2, v1, v3); }
	if (v2.y > v3.y) { SWAP(Vertex2, v2, v3); }

	const int yy1 = CEIL(v1.y);
	const int yy2 = CEIL(v2.y);
	const int yy3 = CEIL(v3.y);

	if (yy3 <= yy1) { return; }

	const fixed20 x1 = fl_f20(v1.x), y1 = fl_f20(v1.y);
	const fixed20 x2 = fl_f20(v2.x), y2 = fl_f20(v2.y);
	const fixed20 x3 = fl_f20(v3.x), y3 = fl_f20(v3.y);

	bool xswap = false;

	fixed20 sx2, sx1 = f20_div(x3 - x1, y3 - y1);
	fixed20 xx2, xx1 = x1;

	if (yy2 > yy1) {
		sx2 = f20_div(x2 - x1, y2 - y1);

		if (sx2 < sx1) {
			SWAP(fixed20, sx1, sx2);
			xswap = true;
		}

		xx1 = x1;
		xx2 = x1;

		for (int y = yy1; y < yy2; ++y) {
			Span *s = &spans[y];
			const float f = f20_fl(xx1);
			s->x1 = CEIL(f);
			s->x2 = f20_int(xx2);

			xx1 += sx1;
			xx2 += sx2;
		}

		if (xswap) {
			xx1 = xx2;
			sx1 = sx2;
		}
	} else if (x2 < x1) {
		xswap = true;
		xx2 = x1;
		sx2 = sx1;
	}

	if (yy3 > yy2) {
		sx2 = f20_div(x3 - x2, y3 - y2);

		if (xswap) {
			SWAP(fixed20, sx1, sx2);
			xx1 = x2;
		} else {
			xx2 = x2;
		}

		for (int y = yy2; y <= yy3; ++y) {
			Span *s = &spans[y];
			const float f = f20_fl(xx1);
			s->x1 = CEIL(f);
			s->x2 = f20_int(xx2);

			xx1 += sx1;
			xx2 += sx2;
		}
	}

	const ulong pitch = buffer->width;
	ulong *ybits = buffer->bits + yy1 * pitch;

	for (int y = yy1; y <= yy3; ++y) {
		Span *s = &spans[y];
		int xs = s->x1;
		int xe = s->x2;

		for (int x = xs; x <= xe; ++x) {
			//assert(0 != ybits[x]);
			ybits[x] = 0 == ybits[x] ? 0x00ffffff : 0x00000000; // c;
		}

		ybits += pitch;
	}
}

void SoftwareRenderer::draw_quad_flat(Vertex2 &v1, Vertex2 &v2, Vertex2 &v3, Vertex2 &v4, const ulong c) {
	if (v1.y > v2.y) { SWAP(Vertex2, v1, v2); }
	if (v1.y > v3.y) { SWAP(Vertex2, v1, v3); }
	if (v1.y > v4.y) { SWAP(Vertex2, v1, v4); }
	if (v2.y > v3.y) { SWAP(Vertex2, v2, v3); }
	if (v2.y > v4.y) { SWAP(Vertex2, v2, v4); }
	if (v3.y > v4.y) { SWAP(Vertex2, v3, v4); }

	const int yy1 = CEIL(v1.y + 0.5f);
	const int yy4 = CEIL(v4.y + 0.5f) - 1;

	if (yy4 <= yy1) { return; }

	const int yy2 = CEIL(v2.y + 0.5f);
	const int yy3 = CEIL(v3.y + 0.5f);

	const fixed8 x1 = fl_f8(v1.x + 0.5f), y1 = fl_f8(v1.y + 0.5f);
	const fixed8 x2 = fl_f8(v2.x + 0.5f), y2 = fl_f8(v2.y + 0.5f);
	const fixed8 x3 = fl_f8(v3.x + 0.5f), y3 = fl_f8(v3.y + 0.5f);
	const fixed8 x4 = fl_f8(v4.x + 0.5f), y4 = fl_f8(v4.y + 0.5f);

	bool xswap = false;

	fixed8 sx3, sx2, sx1 = f8_div(x3 - x1, y3 - y1);
	fixed8 xx3, xx2, xx1 = x1;

	Span* s;

	if (yy2 > yy1) {
		sx2 = f8_div(x2 - x1, y2 - y1);

		if (sx2 < sx1) {
			SWAP(fixed8, sx1, sx2);
			xswap = true;
		}

		xx1 = x1;
		xx2 = x1;

		for (int y = yy1; y < yy2; ++y) {
			s = spans + y;
			s->x1 = f8_int(xx1);
			s->x2 = f8_int(xx2);

			xx1 += sx1;
			xx2 += sx2;
		}

		if (xswap) {
			xx1 = xx2;
			sx1 = sx2;
		}
	} else if (x2 < x1) {
		xswap = true;
		xx2 = x1;
		sx2 = sx1;
	}
}

/*void SoftwareRenderer::draw_triangle_flat2(int v1, int v2, int v3, const ulong c) {
	if (v1.y > v2.y) { SWAP(Vertex2, v1, v2); }
	if (v1.y > v3.y) { SWAP(Vertex2, v1, v3); }
	if (v2.y > v3.y) { SWAP(Vertex2, v2, v3); }

	const float x1 = v1.x, y1 = v1.y;
	const float x2 = v2.x, y2 = v2.y;
	const float x3 = v3.x, y3 = v3.y;

	bool xswap = false;

	const int yy1 = CEIL(y1);
	const int yy2 = CEIL(y2);
	const int yy3 = CEIL(y3);

	float suby = ((float)yy1) - y1;
	float sx2, sx1 = (x3 - x1) / (y3 - y1);
	float xx2, xx1 = x1 + (sx1 * suby);

	if (y2 != y1) {
		sx2 = (x2 - x1) / (y2 - y1);

		if (sx2 < sx1) {
			SWAP(float, sx1, sx2);
			xswap = true;
		}

		xx1 = x1 + (sx1 * suby);
		xx2 = x1 + (sx2 * suby);

		for (int y = yy1; y < yy2; ++y) {
			spans[y].x1 = CEIL(xx1);
			spans[y].x2 = CEIL(xx2);

			xx1 += sx1;
			xx2 += sx2;
		}

		if (xswap) {
			xx1 = xx2;
			sx1 = sx2;
		}
	}

	if (y3 != y2) {
		suby = ((float)yy2) - y2;
		sx2 = (x3 - x2) / (y3 - y2);

		if (xswap) {
			xx1 = x2 + (sx2 * suby);
			SWAP(float, sx1, sx2);
		}
		else {
			xx2 = x2 + (sx2 * suby);
		}

		for (int y = yy2; y <= yy3; ++y) {
			spans[y].x1 = CEIL(xx1);
			spans[y].x2 = CEIL(xx2);

			xx1 += sx1;
			xx2 += sx2;
		}
	}

	ulong *ybits = bits + yy1 * width; //yoffs[yy1];

	for (int y = yy1; y <= yy3; ++y) {
		for (int x = spans[y].x1; x < spans[y].x2; ++x) {
			*(ybits + x) = c;
		}

		ybits += width;
	}
}*/

void SoftwareRenderer::draw_triangle_color(Vertex2c &v1, Vertex2c &v2, Vertex2c &v3) {
	if (v1.y > v2.y) { SWAP(Vertex2c, v1, v2); }
	if (v1.y > v3.y) { SWAP(Vertex2c, v1, v3); }
	if (v2.y > v3.y) { SWAP(Vertex2c, v2, v3); }

	if (v3.y < v1.y) { return; }

	const float vx1 = v1.x; const float vy1 = v1.y; const float vr1 = v1.c.r; const float vg1 = v1.c.g; const float vb1 = v1.c.b;
	const float vx2 = v2.x; const float vy2 = v2.y; const float vr2 = v2.c.r; const float vg2 = v2.c.g; const float vb2 = v2.c.b;
	const float vx3 = v3.x; const float vy3 = v3.y; const float vr3 = v3.c.r; const float vg3 = v3.c.g; const float vb3 = v3.c.b;

	bool xswap = false;

	const float dy1 = 1.0f / (vy3 - vy1);
	float sx2, sx1 = (vx3 - vx1) * dy1;
	float sr2, sr1 = (vr3 - vr1) * dy1;
	float sg2, sg1 = (vg3 - vg1) * dy1;
	float sb2, sb1 = (vb3 - vb1) * dy1;
	float xx1, xx2, rr1, rr2, gg1, gg2, bb1, bb2;

	const int y1 = (int)(vy1 + 0.5f);
	const int y2 = (int)(vy2 + 0.5f);
	const int y3 = (int)(vy3 + 0.5f);

	if (vy1 != vy2) {
		const float dy2 = 1.0f / (vy2 - vy1);

		sx2 = (vx2 - vx1) * dy2;
		sr2 = (vr2 - vr1) * dy2;
		sg2 = (vg2 - vg1) * dy2;
		sb2 = (vb2 - vb1) * dy2;

		if (sx2 < sx1) {
			SWAP(float, sx1, sx2);
			SWAP(float, sr1, sr2);
			SWAP(float, sg1, sg2);
			SWAP(float, sb1, sb2);
			xswap = true;
		}

		const float suby = (float)y1 - vy1;

		xx1 = vx1 + sx1 * suby; xx2 = vx1 + sx2 * suby;
		rr1 = vr1 + sr1 * suby; rr2 = vr1 + sr2 * suby;
		gg1 = vg1 + sg1 * suby; gg2 = vg1 + sg2 * suby;
		bb1 = vb1 + sb1 * suby; bb2 = vb1 + sb2 * suby;

		for (int y = y1; y <= y2; ++y) {
			spans[y].x1 = (int)(xx1 + 0.5f);
			spans[y].x2 = (int)(xx2 + 0.5f);
			spans[y].r1 = rr1; spans[y].r2 = rr2;
			spans[y].g1 = gg1; spans[y].g2 = gg2;
			spans[y].b1 = bb1; spans[y].b2 = bb2;

			xx1 += sx1; xx2 += sx2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
		}

		if (xswap) {
			xx1 = xx2; rr1 = rr2; gg1 = gg2; bb1 = bb2;
			sx1 = sx2; sr1 = sr2; sg1 = sg2; sb1 = sb2;
		}
	}
	else {
		xswap = vx2 < vx1;
	}

	if (vy2 != vy3) {
		float dy2 = 1.0f / (vy3 - vy2);
		sx2 = (vx3 - vx2) * dy2;
		sr2 = (vr3 - vr2) * dy2;
		sg2 = (vg3 - vg2) * dy2;
		sb2 = (vb3 - vb2) * dy2;

		const float suby = (float)y2 - vy2;
		const float dy = vy2 == vy1 ? suby : (float)(y2 - y1);

		if (xswap) {
			SWAP(float, sx1, sx2);
			SWAP(float, sr1, sr2);
			SWAP(float, sg1, sg2);
			SWAP(float, sb1, sb2);

			xx1 = vx2 + sx1 * suby; xx2 = vx1 + sx2 * dy;
			rr1 = vr2 + sr1 * suby; rr2 = vr1 + sr2 * dy;
			gg1 = vg2 + sg1 * suby; gg2 = vg1 + sg2 * dy;
			bb1 = vb2 + sb1 * suby; bb2 = vb1 + sb2 * dy;
		}
		else {
			xx1 = vx1 + sx1 * dy; xx2 = vx2 + sx2 * suby;
			rr1 = vr1 + sr1 * dy; rr2 = vr2 + sr2 * suby;
			gg1 = vg1 + sg1 * dy; gg2 = vg2 + sg2 * suby;
			bb1 = vb1 + sb1 * dy; bb2 = vb2 + sb2 * suby;
		}

		for (int y = y2; y <= y3; ++y) {
			spans[y].x1 = (int)(xx1 + 0.5f);
			spans[y].x2 = (int)(xx2 + 0.5f);
			spans[y].r1 = rr1; spans[y].r2 = rr2;
			spans[y].g1 = gg1; spans[y].g2 = gg2;
			spans[y].b1 = bb1; spans[y].b2 = bb2;

			xx1 += sx1; xx2 += sx2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
		}
	}

	ulong *ybits = buffer->bits + yoffs[y1];
	ulong *xbits;
	int x1, x2;
	int xl, ex, xe;
	float r, g, b;

	for (int y = y1; y <= y3; ++y) {
		x1 = spans[y].x1;
		x2 = spans[y].x2;
		r = spans[y].r1;
		g = spans[y].g1;
		b = spans[y].b1;

		xbits = ybits + x1;

		xl = x2 - x1;
		ex = xl % 4;
		xe = xl - ex;

		const float dx = 1.0f / (x2 - x1);

		const float srx = (spans[y].r2 - spans[y].r1) * dx;
		const float sgx = (spans[y].g2 - spans[y].g1) * dx;
		const float sbx = (spans[y].b2 - spans[y].b1) * dx;

		for (int x = 0; x < xe; x += 4) {
			*(xbits) = RGBf(r, g, b);
			r += srx; g += sgx; b += sbx;

			*(xbits + 1) = RGBf(r, g, b);
			r += srx; g += sgx; b += sbx;

			*(xbits + 2) = RGBf(r, g, b);
			r += srx; g += sgx; b += sbx;

			*(xbits + 3) = RGBf(r, g, b);
			r += srx; g += sgx; b += sbx;

			xbits += 4;
		}
		for (int x = 0; x <= ex; ++x) {
			*(xbits + x) = RGBf(r, g, b);
			r += srx; g += sgx; b += sbx;
		}

		ybits += buffer->width;
	}
}

void SoftwareRenderer::draw_triangle_texture(Vertex2uv &v1, Vertex2uv &v2, Vertex2uv &v3, const int tid) {
	if (v1.y > v2.y) { SWAP(Vertex2uv, v1, v2); }
	if (v1.y > v3.y) { SWAP(Vertex2uv, v1, v3); }
	if (v2.y > v3.y) { SWAP(Vertex2uv, v2, v3); }

	if (v3.y < v1.y) { return; }

	Bitmap *texture = textures[tid];
	const int tw = texture->w - 1;
	const int th = texture->h - 1;

	const float v_x1 = v1.x; const float v_y1 = v1.y; const float v_u1 = v1.u * tw; const float v_v1 = v1.v * th;
	const float v_x2 = v2.x; const float v_y2 = v2.y; const float v_u2 = v2.u * tw; const float v_v2 = v2.v * th;
	const float v_x3 = v3.x; const float v_y3 = v3.y; const float v_u3 = v3.u * tw; const float v_v3 = v3.v * th;

	bool xswap = false;

	const float dy1 = 1.0f / (v_y3 - v_y1);
	float sx2, sx1 = (v_x3 - v_x1) * dy1;
	float su2, su1 = (v_u3 - v_u1) * dy1;
	float sv2, sv1 = (v_v3 - v_v1) * dy1;
	float xx1, xx2, uu1, uu2, vv1, vv2;

	const int y1 = (int)(v_y1 + 0.5f);
	const int y2 = (int)(v_y2 + 0.5f);
	const int y3 = (int)(v_y3 + 0.5f);

	if (v_y1 != v_y2) {
		const float dy2 = 1.0f / (v_y2 - v_y1);

		sx2 = (v_x2 - v_x1) * dy2;
		su2 = (v_u2 - v_u1) * dy2;
		sv2 = (v_v2 - v_v1) * dy2;

		if (sx2 < sx1) {
			SWAP(float, sx1, sx2);
			SWAP(float, su1, su2);
			SWAP(float, sv1, sv2);
			xswap = true;
		}

		const float suby = (float)y1 - v_y1;

		xx1 = v_x1 + sx1 * suby; xx2 = v_x1 + sx2 * suby;
		uu1 = v_u1 + su1 * suby; uu2 = v_u1 + su2 * suby;
		vv1 = v_v1 + sv1 * suby; vv2 = v_v1 + sv2 * suby;

		for (int y = y1; y <= y2; ++y) {
			spans[y].x1 = (int)(xx1 + 0.5f);
			spans[y].x2 = (int)(xx2 + 0.5f);
			spans[y].u1 = uu1; spans[y].u2 = uu2;
			spans[y].v1 = vv1; spans[y].v2 = vv2;

			xx1 += sx1; xx2 += sx2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}

		if (xswap) {
			xx1 = xx2; uu1 = uu2; vv1 = vv2;
			sx1 = sx2; su1 = su2; sv1 = sv2;
		}
	} else {
		xswap = v_x2 < v_x1;
	}

	if (v_y2 != v_y3) {
		float dy2 = 1.0f / (v_y3 - v_y2);
		sx2 = (v_x3 - v_x2) * dy2;
		su2 = (v_u3 - v_u2) * dy2;
		sv2 = (v_v3 - v_v2) * dy2;

		const float suby = (float)y2 - v_y2;
		const float dy = v_y2 == v_y1 ? suby : (float)(y2 - y1);

		if (xswap) {
			SWAP(float, sx1, sx2);
			SWAP(float, su1, su2);
			SWAP(float, sv1, sv2);

			xx1 = v_x2 + sx1 * suby; xx2 = v_x1 + sx2 * dy;
			uu1 = v_u2 + su1 * suby; uu2 = v_u1 + su2 * dy;
			vv1 = v_v2 + sv1 * suby; vv2 = v_v1 + sv2 * dy;
		} else {
			xx1 = v_x1 + sx1 * dy; xx2 = v_x2 + sx2 * suby;
			uu1 = v_u1 + su1 * dy; uu2 = v_u2 + su2 * suby;
			vv1 = v_v1 + sv1 * dy; vv2 = v_v2 + sv2 * suby;
		}

		for (int y = y2; y <= y3; ++y) {
			spans[y].x1 = (int)(xx1 + 0.5f);
			spans[y].x2 = (int)(xx2 + 0.5f);
			spans[y].u1 = uu1; spans[y].u2 = uu2;
			spans[y].v1 = vv1; spans[y].v2 = vv2;

			xx1 += sx1; xx2 += sx2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}
	}

	ulong *ybits = buffer->bits + yoffs[y1];
	int x1, x2;
	float u, v;
	int iu, iv;

	for (int y = y1; y <= y3; ++y) {
		x1 = spans[y].x1;
		x2 = spans[y].x2;
		u = spans[y].u1;
		v = spans[y].v1;



		const float dx = 1.0f / (x2 - x1);

		const float sux = (spans[y].u2 - spans[y].u1) * dx;
		const float svx = (spans[y].v2 - spans[y].v1) * dx;
		
		for (int x = x1; x <= x2; ++x) {
			iu = CEIL(u - 0.5f);
			iv = CEIL(v - 0.5f);

			/*if (iu >= tw || iu <= 0) {
				iu = ABS(iu) & tw;
			}
			if (iv >= th || iv <= 0) {
				iv = ABS(iv) & th;
			}*/

			const long toff = texture->yoffs[iv] + iu;

			*(ybits + x) = texture->data[toff];

			u += sux; v += svx;
		}

		ybits += buffer->width;
	}
}

void SoftwareRenderer::draw_triangle_texture(Triangle3uv &t) {
	if (t.v1.y > t.v2.y) { SWAP(Vertex3uv, t.v1, t.v2); }
	if (t.v1.y > t.v3.y) { SWAP(Vertex3uv, t.v1, t.v3); }
	if (t.v2.y > t.v3.y) { SWAP(Vertex3uv, t.v2, t.v3); }
	
	const int yy1 = CEIL(t.v1.y + 0.5f);
	const int yy3 = CEIL(t.v3.y + 0.5f) - 1;

	if (yy3 <= yy1) { return; }

	const int yy2 = CEIL(t.v2.y + 0.5f);

	Bitmap *texture = textures[t.texture_id];
	const int tw = texture->w - 1;
	const int th = texture->h - 1;
	const int *toffs = texture->yoffs;
	const ulong *tdata = texture->data;

	const fixed8 x1 = fl_f8(t.v1.x + 0.5f), y1 = fl_f8(t.v1.y + 0.5f);
	const fixed8 x2 = fl_f8(t.v2.x + 0.5f), y2 = fl_f8(t.v2.y + 0.5f);
	const fixed8 x3 = fl_f8(t.v3.x + 0.5f), y3 = fl_f8(t.v3.y + 0.5f);

	const float fy1 = t.v1.y + 0.5f;
	const float fy2 = t.v2.y + 0.5f;
	const float fy3 = t.v3.y + 0.5f;
	const float z1 = 1.0f / t.v1.z, u1 = t.v1.u * z1 * tw, v1 = t.v1.v * z1 * th;
	const float z2 = 1.0f / t.v2.z, u2 = t.v2.u * z2 * tw, v2 = t.v2.v * z2 * th;
	const float z3 = 1.0f / t.v3.z, u3 = t.v3.u * z3 * tw, v3 = t.v3.v * z3 * th;

	/*const float dz = ABS(MAX(z1, MAX(z2, z3)) - MIN(z1, MIN(z2, z3)));
	int zstep;
	float izstep;
	if (dz < 0.00625f) {
		zstep = 16;
		izstep = 0.0625f;
	} else if(dz < 0.0125f) {
		zstep = 8;
		izstep = 0.125f;
	} else if (dz < 0.025f) {
		zstep = 4;
		izstep = 0.25f;
	} else if (dz < 0.05f) {
		zstep = 2;
		izstep = 0.5f;
	} else {
		zstep = 1;
		izstep = 1.0f;
	}*/

	bool xswap = false;

	const float dy1 = 1.0f / (fy3 - fy1);

	fixed8 sx2, sx1 = f8_div(x3 - x1, y3 - y1);
	float sz2, sz1 = (z3 - z1) * dy1;
	float su2, su1 = (u3 - u1) * dy1;
	float sv2, sv1 = (v3 - v1) * dy1;
	fixed8 xx2, xx1 = x1;
	float zz2, zz1 = z1;
	float uu2, uu1 = u1;
	float vv2, vv1 = v1;

	if (yy2 > yy1) {
		const float dy2 = 1.0f / (fy2 - fy1);
		sx2 = f8_div(x2 - x1, y2 - y1);

		if (sx2 < sx1) {
			sz2 = sz1; sz1 = (z2 - z1) * dy2;
			su2 = su1; su1 = (u2 - u1) * dy2;
			sv2 = sv1; sv1 = (v2 - v1) * dy2;

			SWAP(fixed8, sx1, sx2);
			xswap = true;
		} else {
			sz2 = (z2 - z1) * dy2;
			su2 = (u2 - u1) * dy2;
			sv2 = (v2 - v1) * dy2;
		}

		xx1 = x1; xx2 = x1;
		zz1 = z1; zz2 = z1;
		uu1 = u1; uu2 = u1;
		vv1 = v1; vv2 = v1;

		for (int y = yy1; y < yy2; ++y) {
			Span* s = spans + y;
			if (xx1 == xx2) {
				s->x1 = 0;
				s->x2 = 0;
			} else {
				s->x1 = f8_int(xx1); s->x2 = f8_int(xx2);
				s->z1 = zz1; s->z2 = zz2;
				s->u1 = uu1; s->u2 = uu2;
				s->v1 = vv1; s->v2 = vv2;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}

		if (xswap) {
			xx1 = xx2; sx1 = sx2;
			zz1 = zz2; sz1 = sz2;
			uu1 = uu2; su1 = su2;
			vv1 = vv2; sv1 = sv2;
		}
	} else if (x2 < x1) {
		xswap = true;
		xx2 = x1; sx2 = sx1;
		zz2 = z1; sz2 = sz1;
		uu2 = u1; su2 = su1;
		vv2 = v1; sv2 = sv1;
	}

	if (yy3 >= yy2) {
		const float dy2 = 1.0f / (fy3 - fy2);

		if (xswap) {
			sx2 = sx1; sx1 = f8_div(x3 - x2, y3 - y2);
			sz2 = sz1; sz1 = (z3 - z2) * dy2;
			su2 = su1; su1 = (u3 - u2) * dy2;
			sv2 = sv1; sv1 = (v3 - v2) * dy2;

			xx1 = x2;
			zz1 = z2;
			uu1 = u2;
			vv1 = v2;
		} else {
			sx2 = f8_div(x3 - x2, y3 - y2);
			sz2 = (z3 - z2) * dy2;
			su2 = (u3 - u2) * dy2;
			sv2 = (v3 - v2) * dy2;

			xx2 = x2;
			zz2 = z2;
			uu2 = u2;
			vv2 = v2;
		}

		for (int y = yy2; y <= yy3; ++y) {
			Span* s = spans + y;
			if (xx1 == xx2) {
				s->x1 = 0;
				s->x2 = 0;
			} else {
				s->x1 = f8_int(xx1); s->x2 = f8_int(xx2);
				s->z1 = zz1; s->z2 = zz2;
				s->u1 = uu1; s->u2 = uu2;
				s->v1 = vv1; s->v2 = vv2;
			}

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}
	}

	ulong *ybits = buffer->bits + buffer->width * yy1;
	for (int y = yy1; y <= yy3; ++y) {
		Span* s = spans + y;

		if (s->x2 == s->x1) {
			ybits += buffer->width;
			continue;
		}

		//draw_span(s, ybits, texture);

		const int xl = s->x2 - s->x1;
		const float dx = 1.0f / (float)xl;
		const float szx = (s->z2 - s->z1) * dx;
		const float sux = (s->u2 - s->u1) * dx;
		const float svx = (s->v2 - s->v1) * dx;

		float zx = s->z1;
		float ux = s->u1;
		float vx = s->v1;

		ulong *xbits = ybits + s->x1;
		ulong *xlen = xbits + xl;

		for (int i = 0, ilen = CEIL(xl / 4.0f); i <= ilen; ++i) {
			const float zp = 1.0f / zx;

			for (ulong *xxlen = MIN(xbits + 4, xlen); xbits <= xxlen; ++xbits) {
				int iu = (int)(ux * zp); /*iu = WRAP(iu, 0, tw);*/ iu = CLAMP(iu, 0, tw);
				int iv = (int)(vx * zp); /*iv = WRAP(iv, 0, th);*/ iv = CLAMP(iv, 0, th);

				const ulong c = *(tdata + tw * iv + iu);
				*(xbits) = c;

				zx += szx;
				ux += sux;
				vx += svx;
			}
		}

		ybits += buffer->width;
	}
}

void SoftwareRenderer::draw_bitmap(const Vertex2 &v, const int tid) {
	Bitmap *texture = textures[tid];

	const int w = texture->w - 1;
	const int h = texture->h - 1;

	ulong *ybits = buffer->bits + yoffs[(int)v.y];
	ulong *xbits;

	for (int y = 0; y <= h; ++y) {
		xbits = ybits + (int)v.x;

		for (int x = 0; x <= w; ++x) {
			*(xbits + x) = *(texture->data + texture->yoffs[y] + x);
		}

		ybits += buffer->width;
	}
}