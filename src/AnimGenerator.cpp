#include "AnimGenerator.h"
#include "Engine.h"
AnimGenerator::AnimGenerator()
{
	iLayer = 1;
}
void AnimGenerator::Trigger()
{
//	SpriteEffect s;
//	s.Spawn(mFX.mPos,mFX.mSize,mFX.pSpr,mFX.fDuration,mFX.bGlow,mFX.mpTarget,mFX.bPlayOnce);
	switch(iLayer)
	{
	case 1:
		gpEngine->mEffectsLayer1.push_back(mFX);
		break;
	case 2:
		gpEngine->mEffectsLayer2.push_back(mFX);
		break;
	case 3:
		gpEngine->mEffectsLayer3.push_back(mFX);
		break;
	}
//	gpEngine->SpawnSpriteEffect(iLayer,mFX.mPos,mFX.mSize,mFX.pSpr,mFX.fDuration,mFX.bGlow,mFX.mpTarget,mFX.bPlayOnce);
}

void AnimGenerator::LoadFromFile(CFileIO &fIO)
{
	fIO.ReadBinary(&ScrollOffset,1);
	fIO.ReadBinary(&iLayer,1);
	fIO.ReadBinary(&iAnimType,1);
	Vec2 Pos;
	fIO.ReadBinary(&Pos.x,1);
	fIO.ReadBinary(&Pos.y,1);
	gpEngine->iTileFrame = iAnimType;
	mFX = gpEngine->GetAnimForEditor(Pos);
	mFX.LoadFromFile(fIO);
}
void AnimGenerator::WriteToFile(CFileIO &fIO)
{
	fIO.WriteBinary(&ScrollOffset,1);
	fIO.WriteBinary(&iLayer,1);
	fIO.WriteBinary(&iAnimType,1);
	Vec2 Pos(mFX.mPos);
	fIO.WriteBinary(&Pos.x,1);
	fIO.WriteBinary(&Pos.y,1);
	mFX.WriteToFile(fIO);
}
