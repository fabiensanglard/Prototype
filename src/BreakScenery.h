#pragma once
#include "Entity.h"

class BreakScenery : public Entity
{
public:
	BreakScenery();
	void Spawn(Vec2 pos);
	BreakScenery* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	SpriteSheet spr;
	int iRotation;//so the geometry can be flipped

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);

private:
	float internalFrame;
};