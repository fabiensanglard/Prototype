#include "Generator.h"
#include "Engine.h"
Generator::Generator()
{
	IsActive = false;
	ScrollOffset = 0;
	curTick = 0;
	tickDelay = 0;
	numEnts = 1;
	bEasySpawn = false;
}
Generator::~Generator()
{

}
//================================================================================================//
						/*************************
						** Trigger a new entity **	
						**************************/
//================================================================================================//
void Generator::Trigger()
{

	if(numEnts<=0)
	{
		IsActive = false;
		return;
	}
	IsActive = true;
	numEnts--;
	curTick = 0;
	if(gpEngine->mPlayer.bJustSpawned && !dynamic_cast<PowerDroid*>(mpEntity) && !dynamic_cast<BreakScenery*>(mpEntity) && !dynamic_cast<Fire*>(mpEntity) && !dynamic_cast<Boss*>(mpEntity))
		return;
	//dont spawn it if we are recording.
	if(gpEngine->bRecording && mpEntity == gpEngine->pRecordEnt)
		return;
	if(gpEngine->fGameDifficulty == 3)
	{
		bEasySpawn = !bEasySpawn;
		if(!bEasySpawn)
			return;
	}

	Entity* pEnt=NULL;
	pEnt = mpEntity->Clone();
	if(pEnt==NULL)
		return;
	pEnt->Spawn(Pos);
	gpEngine->GiveEntityToList(pEnt);
}
//================================================================================================//
						/*************************
						** update the generator **	
						**************************/
//================================================================================================//
void Generator::Update()
{
	curTick++;
	if(curTick>=tickDelay)
		Trigger();
}

void Generator::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&iEntType,1);
	fIO.ReadBinary(&ScrollOffset,1);
	fIO.ReadBinary(&Pos.x,1);
	fIO.ReadBinary(&Pos.y,1);
	fIO.ReadBinary(&numEnts,1);
	fIO.ReadBinary(&tickDelay,1);
	if(!(mpEntity = gpEngine->CreateEntity(iEntType)))
		return;
	mpEntity->LoadFromFile(fIO);
}
void Generator::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&iEntType,1);
	fIO.WriteBinary(&ScrollOffset,1);
	fIO.WriteBinary(&Pos.x,1);
	fIO.WriteBinary(&Pos.y,1);
	fIO.WriteBinary(&numEnts,1);
	fIO.WriteBinary(&tickDelay,1);
	mpEntity->WriteToFile(fIO);
}
