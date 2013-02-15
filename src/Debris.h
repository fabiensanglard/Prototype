#pragma once
#include "Entity.h"

class Debris : public Entity
{
public:
	Debris();
	void Spawn(Vec2 pos);
	Debris* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int mWaterticks;
	float fRotation;
	bool bRotation;//which way to rotate?
	float fRot;
};