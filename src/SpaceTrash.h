#pragma once
#include "Entity.h"

class SpaceTrash : public Entity
{
public:
	SpaceTrash();
	void Spawn(Vec2 pos);
	SpaceTrash* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	int SpreadRadius;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int mWaterticks;
	float fGravity;
	float fRotation;
	bool bRotation;//which way to rotate?
	float fRot;
};