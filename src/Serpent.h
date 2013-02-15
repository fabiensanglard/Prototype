#pragma once
#include "Entity.h"
using namespace std;
#define SERPENT_LENGTH 30
class Serpent : public ControlledEntity
{
public:
	Serpent();
	void Spawn(Vec2 pos);
	void SpawnSection(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	Serpent* Clone()const;

	bool bStartDirection;//serpents start outside the top or bottom of the screen
	float fRotation;
	int RateofFire;
	float fLastFireTime;

	vector<Serpent> mSections;
	int iTickSection;//spawn when this is zero
	int iCurSection;
	Vec2 StartPos;
	bool IsHead;
	
	bool IsVisible;
	int iTickToDeath;//so each segment explodes a little after the last
	int iCurSegDeath;//next segment to kill
	bool IsDead;

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
	float fStartRotation;
private:

	int mWaterticks;

};