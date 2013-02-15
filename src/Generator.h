#pragma once
#include "Entity.h"

class Generator
{
public:
	Vec2 Pos;
	Generator();
	~Generator();
	Entity* mpEntity;
	int iEntType;

	int ScrollOffset;//once the map has scrolled past this point, trigger the entity.
	int numEnts;//max ents to spawn
	int tickDelay;//number of ticks to delay between spawns
	int curTick;
	bool IsActive;

	void Trigger();//creates a new entity of member type
	void Update();

	void LoadFromFile(CFileIO &fIO);
	void WriteToFile(CFileIO &fIO);
private:
	bool bEasySpawn;

};