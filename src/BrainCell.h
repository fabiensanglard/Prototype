#pragma once
#include "Entity.h"

class BrainCell : public Entity
{
public:
	BrainCell();
	void Spawn(Vec2 pos);
	BrainCell* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:

	int iAttackTicks;
	float pulsate;
};