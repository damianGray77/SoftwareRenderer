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
		Vertex3uvc v;
		cverts.push_back(v);
	}

	for (int i = 0; i < 768; ++i) {
		Edge3uv e;
		cedges.push_back(e);
	}

	for (int i = 0; i < 256; ++i) {
		Vertex3 v;
		tnormals.push_back(v);
	}

	for (int i = 0; i < 256; ++i) {
		edge_map.push_back(0);
	}

	bounds = new Vertex2[4];
	clip_poly1 = new Vertex3uvc[15];
	clip_poly2 = new Vertex3uvc[15];
	spans = new Span[buffer->height];
	
	load_textures();

	return true;
}

bool SoftwareRenderer::load_textures() {
	// TODO: remove hardcoded value
	int len = 1;

	textures = new Texture*[len];

	for (int i = 0; i < len; ++i) {
		Bitmap *bmp = new Bitmap(texture_files[i]);
		textures[i] = Texture::load_from_bitmap(*(bmp));
		bmp->unload();
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

	uint len = (uint)tris.size();
	for (uint i = 0; i < len; ++i) {
		const Triangle3t t = tris[i];
		const Vertex3uvc v1 = sverts[t.v1];
		const Vertex3uvc v2 = sverts[t.v2];
		const Vertex3uvc v3 = sverts[t.v3];

		float area =
			  v1.x * v2.y - v2.x * v1.y
			+ v2.x * v3.y - v3.x * v2.y
			+ v3.x * v1.y - v1.x * v3.y
		;
		if (area <= 0.0f) { continue; }

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
				SWAP(&clip_poly2, &clip_poly1);
				SWAP(&cp2len, &cp1len);
			}

			if (miny < 0) {
				slice_poly(bounds[0], bounds[1], top);		// { 0, 0 }, { w, 0 }
				SWAP(&clip_poly2, &clip_poly1);
				SWAP(&cp2len, &cp1len);
			}

			if (maxx > _w) {
				slice_poly(bounds[1], bounds[2], right);	// { w, 0 }, { w, h }
				SWAP(&clip_poly2, &clip_poly1);
				SWAP(&cp2len, &cp1len);
			}

			if (maxy > _h) {
				slice_poly(bounds[2], bounds[3], bottom);	// { w, h }, { 0, h }
				SWAP(&clip_poly2, &clip_poly1);
				SWAP(&cp2len, &cp1len);
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
	Vertex3uvc v0 = clip_poly1[cp1len - 1], v1;
	int s1 = Vertex3uvc::left_of(x0, x1, v0), s2;
	
	if (s1 != -1) {
		clip_poly2[cp2len] = v0;
		++cp2len;
	}

	for (int i = 0; i < cp1len; ++i) {
		v1 = clip_poly1[i];
		s2 = Vertex3uvc::left_of(x0, x1, v1);
		if (s1 + s2 == 0 && s1) {
			Vertex3uvc t;
			if (Vertex3uvc::intersects(x0, x1, v0, v1, s, t)) {
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

void SoftwareRenderer::project_vert(const int i, const Matrix4x4 &m) {
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

void SoftwareRenderer::project_light(const int i, const Matrix4x4 &m) {
	Vertex3c &s = slights[i];
	Vertex3 &w = wlights[i];

	const Vertex3c &v = lights[i];

	w = v * m;
	if (0 == w.z) { return; }

	const float z = dist / w.z;

	s.x = (w.x * z) + buffer->mid_width;
	s.y = (-w.y * z) + buffer->mid_height;
	s.z = w.z;
	s.r = v.r;
	s.g = v.g;
	s.b = v.b;
}

void SoftwareRenderer::calculate_normals(Matrix4x4 &w) {
	uint tlen = (uint)tris.size();
	for (uint i = 0; i < tlen; ++i) {
		const Triangle3t t = tris[i];

		Vertex3 v1 = wverts[t.v1];
		Vertex3 v2 = wverts[t.v2];
		Vertex3 v3 = wverts[t.v3];
		Vertex3 vn = Vertex3::cross(v3 - v1, v2 - v1);
		tnormals[i] = Vertex3::normal(vn);
	}

	uint vlen = (uint)verts.size();
	for (uint i = 0; i < vlen; ++i) {
		Vertex3 v = wverts[i];

		// this is super bad, need to refactor. Find an efficient way to generate vertex normals!
		for (uint j = 0; j < tlen; ++j) {
			Triangle3t t = tris[j];
			if (t.v1 == i) { v += tnormals[j]; }
			if (t.v2 == i) { v += tnormals[j]; }
			if (t.v3 == i) { v += tnormals[j]; }
		}

		vnormals[i] = Vertex3::normal(v);
	}
}

void SoftwareRenderer::calculate_lights() {
	uint vlen = (uint)verts.size();
	uint llen = (uint)lights.size();

	uint instpad = 0;

	for (uint i = 0; i < vlen; ++i) {
		Vertex3 vn = vnormals[i];

		float r = 0;
		float g = 0;
		float b = 0;

		for (uint j = 0; j < llen; ++j) {
			Vertex3 to_light = wlights[j] - wverts[i];
			to_light = Vertex3::normal(to_light);

			float cos_theta = 0;
			cos_theta = Vertex3::dot(vn, to_light);
			cos_theta = CLAMP(cos_theta, 0, 1);

			Vertex3c l = lights[j];
			r += l.r * cos_theta;
			g += l.g * cos_theta;
			b += l.b * cos_theta;
			++instpad;
		}

		sverts[i].r = r;
		sverts[i].g = g;
		sverts[i].b = b;
	}
}

void SoftwareRenderer::render() {
	buffer->clear(0x00000000);

	static float rx = 0, ry = 0, rz = 0, r2 = 0, r3 = 0, r4 = 0;

	//rx += 0.275f; if (rx > SINCOSMAX) { rx -= SINCOSMAX; }
	//ry += 0.125f; if (ry > SINCOSMAX) { ry -= SINCOSMAX; }
	//rz += 0.05f;  if (rz > SINCOSMAX) { rz -= SINCOSMAX; }

	rx += 11.0f; if (rx > SINCOSMAX) { rx -= SINCOSMAX; }
	ry += 2.5f;  if (ry > SINCOSMAX) { ry -= SINCOSMAX; }
	rz += 1.0f;  if (rz > SINCOSMAX) { rz -= SINCOSMAX; }

	r2 += 20.0f;  if (r2 > SINCOSMAX) { r2 -= SINCOSMAX; }
	r3 += 14.0f;  if (r2 > SINCOSMAX) { r2 -= SINCOSMAX; }
	r4 += 3.0f;  if (r2 > SINCOSMAX) { r2 -= SINCOSMAX; }

	Matrix4x4 l;
	Matrix4x4::identity(l);
	Matrix4x4::rotate(l, 0, r2, 0);
	Matrix4x4::translate(l, 0, 0, 15.0f);
	project_light(0, l);

	Matrix4x4::identity(l);
	Matrix4x4::rotate(l, r2, r3, 0);
	Matrix4x4::translate(l, 0, 0, 17.0f);
	project_light(1, l);

	Matrix4x4::identity(l);
	Matrix4x4::rotate(l, r3, 0, r4);
	Matrix4x4::translate(l, 0, 0, 13.0f);
	project_light(2, l);
	//Matrix4x4 l;
	//Matrix4x4::identity(l);
	//Matrix4x4::translate(l, 0, 0, 2.0f);
	//Matrix4x4::rotate(l, rx, ry, rz);
	//Matrix4x4::translate(l, 0, 0, 15.0f);


	uint llen = (uint)lights.size();
	/*for (uint i = 0; i < llen; ++i) {
		project_light(i, l);
	}*/

	Matrix4x4 p;
	Matrix4x4::identity(p);
	Matrix4x4::rotate(p, rx, ry, rz);
	Matrix4x4::translate(p, 0, 0, 15.0f);
	//Matrix4x4::rotate(p, 0, 0, rz);
	//Matrix4x4::translate(p, 5, 0, 0);
	//Matrix4x4::rotate(p, 0, ry, 0);
	//Matrix4x4::translate(p, 0, 0, 20.0f);

	uint vlen = (uint)verts.size();
	for (uint i = 0; i < vlen; ++i) {
		project_vert(i, p);
	}

	calculate_normals(p);
	calculate_lights();

	int ctlen = backfacecull_and_clip();
	float itlen = 1.0f / (float)ctlen;
	for (int i = 0; i < ctlen; ++i) {
		Triangle3uvc t = ctris[i];
		draw_triangle_texture_p(t);

		//float c = (float)(i + 1) * itlen;
		//draw_triangle_flat(t.v1, t.v2, t.v3, 0x00ff0000); // RGBf(c, c, c));
		//draw_triangle_flat_fixed(t.v1, t.v2, t.v3, 0x00ff0000);
	}

	/*uint tlen = (uint)tris.size();
	for (uint i = 0; i < tlen; ++i) {
		Triangle3t t = tris[i];

		Vertex3 vn_off = (sverts[t.v1] + sverts[t.v2] + sverts[t.v3]) * 0.333f;
		draw_line(vn_off, vn_off + tnormals[i] * 40.0f, 0xfffffff);

		for (uint j = 0; j < llen; ++j) {
			draw_line(slights[j], vn_off, 0x00ff0000);
		}
	}*/

	Vertex2 v1, v2;
	for (uint i = 0; i < llen; ++i) {
		Vertex3c v = slights[i];

		const ulong c = RGBf(v.r, v.g, v.b);

		v1.x = v.x - 9.0f; v1.y = v.y;
		v2.x = v.x + 9.0f; v2.y = v.y;
		draw_line(v1, v2, c);

		v1.x = v.x; v1.y = v.y - 9.0f;
		v2.x = v.x; v2.y = v.y + 9.0f;
		draw_line(v1, v2, c);
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
			SWAP(&vy1, &vy2);
			SWAP(&vx1, &vx2);
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
			SWAP(&vx1, &vx2);
			SWAP(&vy1, &vy2);
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

void SoftwareRenderer::draw_triangle_flat(Vertex2 &v1, Vertex2 &v2, Vertex2 &v3, const ulong c) {
	if (v1.y > v2.y) { SWAP(&v1, &v2); }
	if (v1.y > v3.y) { SWAP(&v1, &v3); }
	if (v2.y > v3.y) { SWAP(&v2, &v3); }

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
			SWAP(&sx1, &sx2);
			xswap = true;
		}

		xx1 = x1;
		xx2 = x1;

		int ye = yy2 - 1;
		for (int y = yy1; y <= ye; ++y) {
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
			SWAP(&sx1, &sx2);
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

void SoftwareRenderer::draw_triangle_texture_a(Triangle3uv &t) {
	if (t.v1.y > t.v2.y) { SWAP(&t.v1, &t.v2); }
	if (t.v1.y > t.v3.y) { SWAP(&t.v1, &t.v3); }
	if (t.v2.y > t.v3.y) { SWAP(&t.v2, &t.v3); }

	const int yy1 = CEIL(t.v1.y);
	const int yy2 = CEIL(t.v2.y);
	const int yy3 = CEIL(t.v3.y);

	if (yy3 <= yy1) { return; }

	Texture *texture = textures[t.texture_id];
	const int tw = texture->width;
	const int th = texture->height;

	const float x1 = t.v1.x, y1 = t.v1.y;
	const float x2 = t.v2.x, y2 = t.v2.y;
	const float x3 = t.v3.x, y3 = t.v3.y;

	const float u1 = t.v1.u * tw, v1 = t.v1.v * th;
	const float u2 = t.v2.u * tw, v2 = t.v2.v * th;
	const float u3 = t.v3.u * tw, v3 = t.v3.v * th;

	bool xswap = false;

	const float dy1 = 1.0f / (y3 - y1);
	float sx2, sx1 = (x3 - x1) * dy1;
	float su2, su1 = (u3 - u1) * dy1;
	float sv2, sv1 = (v3 - v1) * dy1;
	float xx2, xx1 = x1;
	float uu2, uu1 = u1;
	float vv2, vv1 = v1;

	if (yy2 > yy1) {
		const float dy2 = 1.0f / (y2 - y1);
		sx2 = (x2 - x1) * dy2;

		if (sx2 < sx1) {
			su2 = su1; su1 = (u2 - u1) * dy2;
			sv2 = sv1; sv1 = (v2 - v1) * dy2;

			SWAP(&sx1, &sx2);
			xswap = true;
		}
		else {
			su2 = (u2 - u1) * dy2;
			sv2 = (v2 - v1) * dy2;
		}

		xx1 = x1; xx2 = x1;
		uu1 = u1; uu2 = u1;
		vv1 = v1; vv2 = v1;

		int ye = yy2 - 1;
		for (int y = yy1; y <= ye; ++y) {
			const float dx = 1.0f / (xx2 - xx1);

			Span *s = &spans[y];
			s->x1 = CEIL(xx1); s->x2 = FLOOR(xx2);
			s->u = uu1; s->su = (uu2 - uu1) * dx;
			s->v = vv1; s->sv = (vv2 - vv1) * dx;

			xx1 += sx1; xx2 += sx2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}

		if (xswap) {
			xx1 = xx2; sx1 = sx2;
			uu1 = uu2; su1 = su2;
			vv1 = vv2; sv1 = sv2;
		}
	} else if (x2 < x1) {
		xswap = true;
		xx2 = x1; sx2 = sx1;
		uu2 = u1; su2 = su1;
		vv2 = v1; sv2 = sv1;
	}

	if (yy3 > yy2) {
		const float dy2 = 1.0f / (y3 - y2);

		if (xswap) {
			sx2 = sx1; sx1 = (x3 - x2) * dy2;
			su2 = su1; su1 = (u3 - u2) * dy2;
			sv2 = sv1; sv1 = (v3 - v2) * dy2;

			xx1 = x2;
			uu1 = u2;
			vv1 = v2;
		} else {
			sx2 = (x3 - x2) * dy2;
			su2 = (u3 - u2) * dy2;
			sv2 = (v3 - v2) * dy2;

			xx2 = x2;
			uu2 = u2;
			vv2 = v2;
		}

		for (int y = yy2; y <= yy3; ++y) {
			const float dx = 1.0f / (xx2 - xx1);

			Span *s = &spans[y];
			s->x1 = CEIL(xx1); s->x2 = FLOOR(xx2);
			s->u = uu1; s->su = (uu2 - uu1) * dx;
			s->v = vv1; s->sv = (vv2 - vv1) * dx;

			xx1 += sx1; xx2 += sx2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}
	}

	const ulong pitch = buffer->width;
	ulong *ybits = buffer->bits + yy1 * pitch;

	for (int y = yy1; y <= yy3; ++y) {
		const Span *s = &spans[y];
		const int xs = s->x1;
		const int xe = s->x2;

		const float su = s->su;
		const float sv = s->sv;

		float u = s->u;
		float v = s->v;

		for (int x = xs; x <= xe; ++x) {
			//const int iu = (int)u;
			//const int iv = (int)v;

			//ybits[x] = texture->point(iv, iu);
			ybits[x] = texture->bi_point(u, v);

			u += su;
			v += sv;
		}

		ybits += pitch;
	}
}

void SoftwareRenderer::draw_triangle_texture_p(Triangle3uvc &t) {
	if (t.v1.y > t.v2.y) { SWAP(&t.v1, &t.v2); }
	if (t.v1.y > t.v3.y) { SWAP(&t.v1, &t.v3); }
	if (t.v2.y > t.v3.y) { SWAP(&t.v2, &t.v3); }
	
	const int yy1 = CEIL(t.v1.y);
	const int yy2 = CEIL(t.v2.y);
	const int yy3 = CEIL(t.v3.y);

	if (yy3 <= yy1) { return; }

	Texture *texture = textures[t.texture_id];
	const int tw = texture->width;
	const int th = texture->height;

	const float x1 = t.v1.x, y1 = t.v1.y;
	const float x2 = t.v2.x, y2 = t.v2.y;
	const float x3 = t.v3.x, y3 = t.v3.y;

	const float z1 = 1.0f / t.v1.z, u1 = t.v1.u * z1 * tw, v1 = t.v1.v * z1 * th;
	const float z2 = 1.0f / t.v2.z, u2 = t.v2.u * z2 * tw, v2 = t.v2.v * z2 * th;
	const float z3 = 1.0f / t.v3.z, u3 = t.v3.u * z3 * tw, v3 = t.v3.v * z3 * th;

	const float r1 = t.v1.r, g1 = t.v1.g, b1 = t.v1.b;
	const float r2 = t.v2.r, g2 = t.v2.g, b2 = t.v2.b;
	const float r3 = t.v3.r, g3 = t.v3.g, b3 = t.v3.b;

	bool xswap = false;

	const float dy1 = 1.0f / (y3 - y1);
	float sx2, sx1 = (x3 - x1) * dy1;
	float sz2, sz1 = (z3 - z1) * dy1;
	float su2, su1 = (u3 - u1) * dy1;
	float sv2, sv1 = (v3 - v1) * dy1;
	float sr2, sr1 = (r3 - r1) * dy1;
	float sg2, sg1 = (g3 - g1) * dy1;
	float sb2, sb1 = (b3 - b1) * dy1;
	float xx2, xx1 = x1;
	float zz2, zz1 = z1;
	float uu2, uu1 = u1;
	float vv2, vv1 = v1;
	float rr2, rr1 = r1;
	float gg2, gg1 = g1;
	float bb2, bb1 = b1;

	if (yy2 > yy1) {
		const float dy2 = 1.0f / (y2 - y1);
		sx2 = (x2 - x1) * dy2;

		if (sx2 < sx1) {
			sz2 = sz1; sz1 = (z2 - z1) * dy2;
			su2 = su1; su1 = (u2 - u1) * dy2;
			sv2 = sv1; sv1 = (v2 - v1) * dy2;
			sr2 = sr1; sr1 = (r2 - r1) * dy2;
			sg2 = sg1; sg1 = (g2 - g1) * dy2;
			sb2 = sb1; sb1 = (b2 - b1) * dy2;

			SWAP(&sx1, &sx2);
			xswap = true;
		} else {
			sz2 = (z2 - z1) * dy2;
			su2 = (u2 - u1) * dy2;
			sv2 = (v2 - v1) * dy2;
			sr2 = (r2 - r1) * dy2;
			sg2 = (g2 - g1) * dy2;
			sb2 = (b2 - b1) * dy2;
		}

		xx1 = x1; xx2 = x1;
		zz1 = z1; zz2 = z1;
		uu1 = u1; uu2 = u1;
		vv1 = v1; vv2 = v1;
		rr1 = r1; rr2 = r1;
		gg1 = g1; gg2 = g1;
		bb1 = b1; bb2 = b1;

		int ye = yy2 - 1;
		for (int y = yy1; y <= ye; ++y) {
			const float dx = 1.0f / (xx2 - xx1);

			Span *s = &spans[y];
			s->x1 = CEIL(xx1); s->x2 = FLOOR(xx2);
			s->z = zz1; s->sz = (zz2 - zz1) * dx;
			s->u = uu1; s->su = (uu2 - uu1) * dx;
			s->v = vv1; s->sv = (vv2 - vv1) * dx;
			s->r = rr1; s->sr = (rr2 - rr1) * dx;
			s->g = gg1; s->sg = (gg2 - gg1) * dx;
			s->b = bb1; s->sb = (bb2 - bb1) * dx;

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
		}

		if (xswap) {
			xx1 = xx2; sx1 = sx2;
			zz1 = zz2; sz1 = sz2;
			uu1 = uu2; su1 = su2;
			vv1 = vv2; sv1 = sv2;
			rr1 = rr2; sr1 = sr2;
			gg1 = gg2; sg1 = sg2;
			bb1 = bb2; sb1 = sb2;
		}
	} else if (x2 < x1) {
		xswap = true;
		xx2 = x1; sx2 = sx1;
		zz2 = z1; sz2 = sz1;
		uu2 = u1; su2 = su1;
		vv2 = v1; sv2 = sv1;
		rr2 = r1; sr2 = sr1;
		gg2 = g1; sg2 = sg1;
		bb2 = b1; sb2 = sb1;
	}

	if (yy3 > yy2) {
		const float dy2 = 1.0f / (y3 - y2);

		if (xswap) {
			sx2 = sx1; sx1 = (x3 - x2) * dy2;
			sz2 = sz1; sz1 = (z3 - z2) * dy2;
			su2 = su1; su1 = (u3 - u2) * dy2;
			sv2 = sv1; sv1 = (v3 - v2) * dy2;
			sr2 = sr1; sr1 = (r3 - r2) * dy2;
			sg2 = sg1; sg1 = (g3 - g2) * dy2;
			sb2 = sb1; sb1 = (b3 - b2) * dy2;

			xx1 = x2;
			zz1 = z2;
			uu1 = u2;
			vv1 = v2;
			rr1 = r2;
			gg1 = g2;
			bb1 = b2;
		} else {
			sx2 = (x3 - x2) * dy2;
			sz2 = (z3 - z2) * dy2;
			su2 = (u3 - u2) * dy2;
			sv2 = (v3 - v2) * dy2;
			sr2 = (r3 - r2) * dy2;
			sg2 = (g3 - g2) * dy2;
			sb2 = (b3 - b2) * dy2;

			xx2 = x2;
			zz2 = z2;
			uu2 = u2;
			vv2 = v2;
			rr2 = r2;
			gg2 = g2;
			bb2 = b2;
		}

		for (int y = yy2; y <= yy3; ++y) {
			const float dx = 1.0f / (xx2 - xx1);

			Span *s = &spans[y];
			s->x1 = CEIL(xx1); s->x2 = FLOOR(xx2);
			s->z = zz1; s->sz = (zz2 - zz1) * dx;
			s->u = uu1; s->su = (uu2 - uu1) * dx;
			s->v = vv1; s->sv = (vv2 - vv1) * dx;
			s->r = rr1; s->sr = (rr2 - rr1) * dx;
			s->g = gg1; s->sg = (gg2 - gg1) * dx;
			s->b = bb1; s->sb = (bb2 - bb1) * dx;

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
			rr1 += sr1; rr2 += sr2;
			gg1 += sg1; gg2 += sg2;
			bb1 += sb1; bb2 += sb2;
		}
	}

	const ulong pitch = buffer->width;
	ulong *ybits = buffer->bits + yy1 * pitch;

	for (int y = yy1; y <= yy3; ++y) {
		const Span s = spans[y];
		const int xs = s.x1;
		const int xe = s.x2;

		const float sz = s.sz;
		const float su = s.su;
		const float sv = s.sv;
		const float sr = s.sr;
		const float sg = s.sg;
		const float sb = s.sb;

		float z = s.z;
		float u = s.u;
		float v = s.v;
		float r = s.r;
		float g = s.g;
		float b = s.b;

		for (int x = xs; x <= xe; ++x) {
			const float zp = 1.0f / z;
			uint iu = (int)(u * zp); //iu = WRAP(iu, 0, tw);
			uint iv = (int)(v * zp); //iv = WRAP(iv, 0, th);
			
			//ulong c = texture->point(iu, iv);
			ulong c = texture->bi_point(u * zp, v * zp);
			c =
				  (uchar)(r * (c >> 16 & 0xff)) << 16
				| (uchar)(g * (c >> 8 & 0xff)) << 8
				| (uchar)(b * (c & 0xff))
			;
			ybits[x] = c;

			z += sz;
			u += su;
			v += sv;
			r += sr;
			g += sg;
			b += sb;
		}

		ybits += pitch;
	}
}

/*void SoftwareRenderer::draw_triangle_texture_p(Triangle3uv &t) {
	if (t.v1.y > t.v2.y) { SWAP(&t.v1, &t.v2); }
	if (t.v1.y > t.v3.y) { SWAP(&t.v1, &t.v3); }
	if (t.v2.y > t.v3.y) { SWAP(&t.v2, &t.v3); }

	const int yy1 = CEIL(t.v1.y);
	const int yy2 = CEIL(t.v2.y);
	const int yy3 = CEIL(t.v3.y);

	if (yy3 <= yy1) { return; }

	Texture *texture = textures[t.texture_id];
	const int tw = texture->width;
	const int th = texture->height;

	const float x1 = t.v1.x, y1 = t.v1.y;
	const float x2 = t.v2.x, y2 = t.v2.y;
	const float x3 = t.v3.x, y3 = t.v3.y;

	const float z1 = 1.0f / t.v1.z, u1 = t.v1.u * z1 * tw, v1 = t.v1.v * z1 * th;
	const float z2 = 1.0f / t.v2.z, u2 = t.v2.u * z2 * tw, v2 = t.v2.v * z2 * th;
	const float z3 = 1.0f / t.v3.z, u3 = t.v3.u * z3 * tw, v3 = t.v3.v * z3 * th;

	bool xswap = false;

	const float dy1 = 1.0f / (y3 - y1);
	float sx2, sx1 = (x3 - x1) * dy1;
	float sz2, sz1 = (z3 - z1) * dy1;
	float su2, su1 = (u3 - u1) * dy1;
	float sv2, sv1 = (v3 - v1) * dy1;
	float xx2, xx1 = x1;
	float zz2, zz1 = z1;
	float uu2, uu1 = u1;
	float vv2, vv1 = v1;

	if (yy2 > yy1) {
		const float dy2 = 1.0f / (y2 - y1);
		sx2 = (x2 - x1) * dy2;

		if (sx2 < sx1) {
			sz2 = sz1; sz1 = (z2 - z1) * dy2;
			su2 = su1; su1 = (u2 - u1) * dy2;
			sv2 = sv1; sv1 = (v2 - v1) * dy2;

			SWAP(&sx1, &sx2);
			xswap = true;
		}
		else {
			sz2 = (z2 - z1) * dy2;
			su2 = (u2 - u1) * dy2;
			sv2 = (v2 - v1) * dy2;
		}

		xx1 = x1; xx2 = x1;
		zz1 = z1; zz2 = z1;
		uu1 = u1; uu2 = u1;
		vv1 = v1; vv2 = v1;

		int ye = yy2 - 1;
		for (int y = yy1; y <= ye; ++y) {
			const float dx = 1.0f / (xx2 - xx1);

			Span *s = &spans[y];
			s->x1 = CEIL(xx1); s->x2 = FLOOR(xx2);
			s->z = zz1; s->sz = (zz2 - zz1) * dx;
			s->u = uu1; s->su = (uu2 - uu1) * dx;
			s->v = vv1; s->sv = (vv2 - vv1) * dx;

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
	}
	else if (x2 < x1) {
		xswap = true;
		xx2 = x1; sx2 = sx1;
		zz2 = z1; sz2 = sz1;
		uu2 = u1; su2 = su1;
		vv2 = v1; sv2 = sv1;
	}

	if (yy3 > yy2) {
		const float dy2 = 1.0f / (y3 - y2);

		if (xswap) {
			sx2 = sx1; sx1 = (x3 - x2) * dy2;
			sz2 = sz1; sz1 = (z3 - z2) * dy2;
			su2 = su1; su1 = (u3 - u2) * dy2;
			sv2 = sv1; sv1 = (v3 - v2) * dy2;

			xx1 = x2;
			zz1 = z2;
			uu1 = u2;
			vv1 = v2;
		}
		else {
			sx2 = (x3 - x2) * dy2;
			sz2 = (z3 - z2) * dy2;
			su2 = (u3 - u2) * dy2;
			sv2 = (v3 - v2) * dy2;

			xx2 = x2;
			zz2 = z2;
			uu2 = u2;
			vv2 = v2;
		}

		for (int y = yy2; y <= yy3; ++y) {
			const float dx = 1.0f / (xx2 - xx1);

			Span *s = &spans[y];
			s->x1 = CEIL(xx1); s->x2 = FLOOR(xx2);
			s->z = zz1; s->sz = (zz2 - zz1) * dx;
			s->u = uu1; s->su = (uu2 - uu1) * dx;
			s->v = vv1; s->sv = (vv2 - vv1) * dx;

			xx1 += sx1; xx2 += sx2;
			zz1 += sz1; zz2 += sz2;
			uu1 += su1; uu2 += su2;
			vv1 += sv1; vv2 += sv2;
		}
	}

	const ulong pitch = buffer->width;
	ulong *ybits = buffer->bits + yy1 * pitch;

	for (int y = yy1; y <= yy3; ++y) {
		const Span *s = &spans[y];
		const int xs = s->x1;
		const int xe = s->x2;

		const float sz = s->sz;
		const float su = s->su;
		const float sv = s->sv;

		float z = s->z;
		float u = s->u;
		float v = s->v;

		for (int x = xs; x <= xe; ++x) {
			const float zp = 1.0f / z;
			uint iu = (int)(u * zp); //iu = WRAP(iu, 0, tw);
			uint iv = (int)(v * zp); //iv = WRAP(iv, 0, th);

			ybits[x] = texture->point(iu, iv);
			//ybits[x] = texture->bi_point(u * zp, v * zp);

			z += sz;
			u += su;
			v += sv;
		}

		ybits += pitch;
	}
}*/

void SoftwareRenderer::draw_bitmap(const Vertex2 &v, const int tid) {
	Texture *texture = textures[tid];

	const uint pitch = texture->pitch;
	const int w = texture->width;
	const int h = texture->height;

	ulong *ybits = buffer->bits + yoffs[(int)v.y];
	ulong *xbits;

	for (int y = 0; y <= h; ++y) {
		xbits = ybits + (int)v.x;

		for (int x = 0; x <= w; ++x) {
			*(xbits + x) = *(texture->bits + pitch * y + x);
		}

		ybits += buffer->width;
	}
}