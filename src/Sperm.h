#pragma once
#include "Entity.h"

class Sperm : public Entity
{
public:
	Sperm();
	void Spawn(Vec2 pos);
	Sperm* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
	float fRotation;
private:
	int iLifeTicks;
};