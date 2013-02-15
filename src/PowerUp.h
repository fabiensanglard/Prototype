#pragma once
#include "Entity.h"

class PowerUp : public Entity
{
public:
	PowerUp();
	void Spawn(Vec2 pos);
	PowerUp* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	int iType;//what am I?

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
};