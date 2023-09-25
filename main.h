#pragma once

#include "resource.h"
#define BITMAP_WIDTH 400
#define BITMAP_HEIGHT 256


typedef struct GAMEBITMAP {
	BITMAPINFO BitmapInfo;
	void* Memory;

}GAMEBITMAP;