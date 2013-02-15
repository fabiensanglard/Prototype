#pragma once
#include "SpriteEffect.h"
#include "FileIO.h"

class AnimGenerator
{
public:
	AnimGenerator();
	SpriteEffect mFX;
	float ScrollOffset;
	int iLayer;
	int iAnimType;//solely for editor
	void Trigger();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
};