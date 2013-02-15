#pragma once
#include "Entity.h"
using namespace std;
#define SolarFlare_LENGTH 15
class SolarFlare : public ControlledEntity
{
public:
	SolarFlare();
	void Spawn(Vec2 pos);
	void SpawnSection(Vec2 pos);
	void Update();
	void NeedsToBeRemoved();
	void Draw(const float interp);
	bool CheckCollided(Sphere s, float damage);
	int InWater();
	SolarFlare* Clone()const;

	bool bStartDirection;//SolarFlares start outside the top or bottom of the screen
	float fRotation;

	vector<SolarFlare> mSections;
	int iTickSection;//spawn when this is zero
	int iCurSection;
	Vec2 StartPos;
	bool IsHead;
	

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
	float fStartRotation;
private:

	int mWaterticks;

};