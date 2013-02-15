#pragma once
#include "Entity.h"

class BrainBoss : public Entity
{
public:
	BrainBoss();
	void Spawn(Vec2 pos);
	BrainBoss* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:

	Sphere mBodySphere;
	int iAttackWave;
	int iAttackTicks;
	bool bJustSpawned;

	float fRateOfFire;
	float fRateOfSpawn;

	bool bIsDead;
	int iDeathTicks;
};