#pragma once
#include "Entity.h"

class Bug : public ControlledEntity
{
public:
	Bug();
	void Spawn(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	Bug* Clone()const;
	int InWater();

	bool bStartDirection;//serpents start outside the top or bottom of the screen
	float fRotation;
	int RateofFire;
	float fLastFireTime;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int mWaterticks;
};