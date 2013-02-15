#pragma once
#include "Entity.h"

class CheckPoint : public Entity
{
public:
	CheckPoint();
	void Spawn(Vec2 pos);
	CheckPoint* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
};