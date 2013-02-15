#pragma once
#include "Entity.h"

class SuperTurret : public Entity
{
public:
	SuperTurret();
	void Spawn(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	SuperTurret* Clone()const;
	Vec2* pTarget;//used for tracking
	bool bDirection;//which way up is the turret?
	float fWaitTime;
	bool bWaitBeforeFire;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
};