#pragma once
#include "UTIL_OpenGL.h"
#include "SpriteSheet.h"

void RenderSprite(SpriteSheet spr, int frame, float x, float y, float w, float h);
void RenderRotatedSprite(SpriteSheet spr, int frame, float x, float y, float w, float h, float rot);