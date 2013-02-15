#pragma once

#define		FLIP_HORIZONTAL		1<<1
#define		FLIP_VERTICAL		1<<2
#define		NEEDS_WATER_SHADE	1<<3
#define		WATER_SURFACE		1<<4

typedef struct tile
{
	int ID;
	unsigned char properties;
}Tile;