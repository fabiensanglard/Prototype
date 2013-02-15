#pragma once
#include "Entity.h"

class Fire : public Entity
{
public:
	Fire();
	void Spawn(Vec2 pos);
	Fire* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
	bool bTop;
};