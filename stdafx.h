#pragma once
#ifndef STDAFX_H
#define STDAFX_H

#define WIN32_LEAN_AND_MEAN

/*#define ulong unsigned long
#define uint unsigned int
#define ushort unsigned short
#define uchar unsigned char*/

enum ClipSide {
	  top
	, left
	, bottom
	, right
};

#include "targetver.h"

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "Core.h"
#include <assert.h>

#endif
