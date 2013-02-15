#pragma once
#include "Entity.h"

class Turret : public Entity
{
public:
	Turret();
	void Spawn(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	Turret* Clone()const;
	Vec2* pTarget;//used for tracking
	bool bDirection;//which way up is the turret?
	int RateofFire;
	float fLastFireTime;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
};