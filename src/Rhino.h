#pragma once
#include "Entity.h"

class Rhino : public ControlledEntity
{
public:
	Rhino();
	void Spawn(Vec2 pos);
	Rhino* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	int RateofFire;
	float fLastFireTime;
	int nBulletsFired;
	bool bDirection;//facing left or right
	bool bStartDirection;
	bool bIsTurning;
	bool bIsAiming;
	bool bDropAim;
	int iStartOnScreen;//0 from the right, 1 from the left, 2 from above

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
};