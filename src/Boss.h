#pragma once
#include "Entity.h"
#include "Serpent.h"

class Boss : public Entity
{
public:
	Boss();
	void Spawn(Vec2 pos);
	Boss* Clone()const;
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);

	Serpent *mSerpentAttackPattern1;
	Serpent *mSerpentAttackPattern2;
	Serpent *mSerpentAttackPattern3;
private:
	bool bIsDead;
	bool bJustSpawned;
	bool bComingToLife;
	int nextExplosionTick;
	int iDeathTicks;

	float Eye1Frame;
	float Eye2Frame;
	float Eye3Frame;
	float Eye4Frame;
	float Eye5Frame;
	bool bEye1Blink;
	bool bEye2Blink;
	bool bEye3Blink;
	bool bEye4Blink;
	bool bEye5Blink;
	float Eye1BlinkTime;
	float Eye2BlinkTime;
	float Eye3BlinkTime;
	float Eye4BlinkTime;
	float Eye5BlinkTime;

	Vec2 Tube[4][3];
	float tubeSineWave;

	int iAttackWaveTicks;
	int iAttackPattern;
	float fAttackTicks;
	float fAttackRatio;

	float fChargeFrame;
	float fTopGunRateOfFire;
	float fLowGunRateOfFire;
	float fTopGunFireTime;
	float fLowGunFireTime;
	float fTopCharge;
	float fLowCharge;
	Vec2 targetPos;

	Sphere mBounds[9];//collision bounds
};