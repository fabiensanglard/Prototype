#pragma once
#include "Entity.h"
#include "PowerUp.h"
class PowerDroid : public ControlledEntity
{
public:
	PowerDroid();
	void Spawn(Vec2 pos);
	PowerDroid* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	bool bDirection;//facing left or right
	bool bStartDirection;
	bool bIsTurning;
	bool bIsFlying;

	int iPowerUp;//what im carrying

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	int mWaterticks;
};