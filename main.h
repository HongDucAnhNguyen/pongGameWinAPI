#pragma once
#include <stdint.h>

#include "resource.h"
#define BITMAP_WIDTH 400
#define BITMAP_HEIGHT 256
#define GAME_BITSPERPIXEL 32 //1 pixel is 4 bytes large
//total memory size we need to allocate for stretch bitmap to window
#define GAME_DRAWING_AREA_MEMORY_SIZE (BITMAP_WIDTH *BITMAP_HEIGHT * (GAME_BITSPERPIXEL /8))

typedef struct PIXEL32 {
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Alpha; //lower value = more transparency
}PIXEL32;

typedef struct GAMEBITMAP {
	BITMAPINFO BitmapInfo;
	void* Memory;

}GAMEBITMAP;



typedef struct PLAYER {

	int32_t WorldPositionX;
	int32_t WorldPositionY;

	int32_t characterWidth;
	int32_t characterHeight;

}PLAYER;


