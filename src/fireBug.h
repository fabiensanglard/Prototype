#pragma once
#include "Entity.h"

class FireBug : public Entity
{
public:
	FireBug();
	void Spawn(Vec2 pos);
	FireBug* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	int RateofFire;
	float fLastFireTime;
	bool bStartMovement;//start from left or right

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int mWaterticks;
	float rot;
	bool bDive;
	bool bDiveDirection;
};