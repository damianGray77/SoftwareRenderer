#pragma once
#ifndef MAIN_H
#define MAIN_H

#include "Buffer.h"
#include "Win32.h"
#include "FPS.h"
#include "Camera.h"
#include "Vertex3uvc.h"
#include "Edge3uv.h"
#include "SoftwareRenderer.h"

void init();
void init_geom();
void init_lookups();
void run();
inline bool resize(int, int);
void keypress(uint);
inline void draw();
inline void update();

Buffer buffer;

#endif
