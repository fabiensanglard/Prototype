#pragma once
#include "Entity.h"
#include "Orb.h"
class Player : public ControlledEntity 
{
public:

	void StartGame();
	void Spawn(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	Player* Clone()const;

	int iForceCharge;
	float fForceFrame;
	float fOrbFrame;
	float Speed;
	int iMissileType;
	float fLastMissileFired;

	bool bHasOrb;
	Orb mOrb;

	float fLastFireTime;//basic bullet
	float fLastPlasmaTime;//plasma

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);

	int iScore;
	int iLives;
	int iSpawnOffset;//last checkpoint
	int iDelayBeforeRespawn;
	bool bIsDead;

	bool bSpawnOrb;
	int iSpawnOrbTicks;
	bool bJustSpawned;

	int numBits;

	bool bFullChargeFlash;//make the force charge flash when its full
	int iChargeFlash;

private:
	void FireArc();
	float fArcCharge;
	float justSpawnedTime;
	int mSpawnBeamTicks;
	int mWaterticks;
	int iMaxPulseShots;

	int bit1Ticks, bit2Ticks;
	float bitFrame;

	int iTicksToScreenCenter;
	Vec2 StoredPos;//stored position at game end
	bool bDamagedControls;
	int iDamagedControlTicks;

	int numShields;
	float shieldTime;
};