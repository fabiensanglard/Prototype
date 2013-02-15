#pragma once
#include "Entity.h"

class Rocket : public Entity
{
public:
	Rocket();
	void Spawn(Vec2 pos);
	Rocket* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	float BlastOffPos;
	bool bDirection;//start on floor or roof
	float rotation;
	bool bBlastOff;
	bool bTurnNow;
	bool bFinishedTurning;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
};