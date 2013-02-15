#pragma once
#include "Entity.h"

class RandExplosion : public Entity
{
public:
	RandExplosion();
	void Spawn(Vec2 pos);
	RandExplosion* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int iNextExplosion;
	int iNextLargeExplosion;

};