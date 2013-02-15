#pragma once
#include "Entity.h"

class TriggerSlowDown : public Entity
{
public:
	TriggerSlowDown();
	void Spawn(Vec2 pos);
	TriggerSlowDown* Clone()const;
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