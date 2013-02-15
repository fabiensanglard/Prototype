#pragma once
#include "Entity.h"

class TriggerSpeedUp : public Entity
{
public:
	TriggerSpeedUp();
	void Spawn(Vec2 pos);
	TriggerSpeedUp* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
	int iMapSpeedTicks;
	int iMaxTicks;
};