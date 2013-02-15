#include "Engine.h"
//================================================================================================//
						/*********************************
						** entity generators predicates **	
						**********************************/
//================================================================================================//
bool RemoveIfActive(Generator& g)
{
	if(gpEngine->Scroll>=g.ScrollOffset)
	{
		g.Trigger();
		if(!gpEngine->mPlayer.bJustSpawned)
		{
			gpEngine->mActiveGenerators.push_back(g);
		}
	}

	if(g.IsActive)
		return true;
	return false;
}
bool RemoveIfInActive(Generator& g)
{
	g.Update();

	if(g.IsActive)
		return false;
	return true;
}
bool RemoveIfOffScreen(Generator g)
{
	if(dynamic_cast<Boss*>(g.mpEntity))
	{
		if(gpEngine->bBossFight)
			return false;
	}

	if(dynamic_cast<BreakScenery*>(g.mpEntity) || dynamic_cast<TriggerSpeedUp*>(g.mpEntity) || dynamic_cast<TriggerSlowDown*>(g.mpEntity))
	{
		if(gpEngine->Scroll>g.ScrollOffset+640)
			return true;
	}
	else if(dynamic_cast<Fire*>(g.mpEntity))
	{
		if(gpEngine->Scroll>(g.ScrollOffset+640+512))
			return true;
	}

	else if(gpEngine->Scroll>g.ScrollOffset)
		return true;
	return false;
}
bool SortGenerators(Generator a, Generator b)
{
	if(a.ScrollOffset< b.ScrollOffset)
		return true;
	return false;
}
//================================================================================================//
						/*******************************
						** anim generators predicates **	
						********************************/
//================================================================================================//
bool SortAnimGenerators(AnimGenerator a, AnimGenerator b)
{
	if(a.ScrollOffset < b.ScrollOffset)
		return true;
	return false;
}
bool UpdateAndRemoveAnim(AnimGenerator g)
{
	float s;
	if(g.iLayer == 1)
		s=1;
	else if(g.iLayer == 2)
		s=0.75f;
	else if(g.iLayer == 3)
		s=0.5f;
	if((gpEngine->Scroll*s) >g.ScrollOffset)
	{
		g.Trigger();
		return true;
	}
	return false;
}
bool RemoveEffectIfOffscreen(AnimGenerator g)
{
	float s;

	if(g.iLayer == 1)
		s=1;
	else if(g.iLayer == 2)
		s=0.75f;
	else if(g.iLayer == 3)
		s=0.5f;

	if((gpEngine->Scroll*s) >g.ScrollOffset+640)
		return true;

	return false;
}
//================================================================================================//
						/***********************
						** simplified factory **	
						************************/
//================================================================================================//
Entity* Engine::CreateEntity(int type)
{
	Entity* pEnt = NULL;
	switch(type)
	{
	case 0://ladybird
		pEnt = new LadyBird;
		break;
	case 1://turret
		pEnt = new Turret;
		((Turret*)pEnt)->pTarget = &mPlayer.Pos;
		break;
	case 2://Serpent
		pEnt = new Serpent;
		break;
	case 3://rhino
		pEnt = new Rhino;
		break;
	case 4://Rocket
		pEnt = new Rocket;
		break;
	case 5://SuperTurret
		pEnt = new SuperTurret;
		break;
	case 6://PowerDroid
		pEnt = new PowerDroid;
		break;
	case 7://Bug
		pEnt = new Bug;
		break;
	case 8://break scene1
		pEnt = new BreakScenery;
		break;
	case 9://StarBurst
		pEnt = new StarBurst;
		break;
	case 10://Fighter
		pEnt = new Fighter;
		break;
	case 11://CheckPoint
		pEnt = new CheckPoint;
		break;
	case 13://boss
		pEnt = new Boss;
		break;
	case 14://midboss
		pEnt = new MidBoss;
		break;
	case 15://trigger speedup
		pEnt = new TriggerSpeedUp;
		break;
	case 16://trigger slowdown
		pEnt = new TriggerSlowDown;
		break;
	case 17://fire
		pEnt = new Fire;
		break;
	case 18://solare flare
		pEnt = new SolarFlare;
		break;
	case 19://fire bug
		pEnt = new FireBug;
		break;
	case 20://fire bug
		pEnt = new SpaceTrash;
		break;
	}
	return pEnt;
}
//================================================================================================//
						/***************************************
						** Pass a new entity in the game list **	
						****************************************/
//================================================================================================//
void Engine::GiveEntityToList(Entity* pEnt)
{
	if(dynamic_cast<BreakScenery*>(pEnt) || dynamic_cast<Boss*>(pEnt) || dynamic_cast<BrainBoss*>(pEnt) || dynamic_cast<Fire*>(pEnt))
		mTopGameEnts.push_back(pEnt);
	else
		mGameEnts.push_back(pEnt);
}
//================================================================================================//
						/**********************
						** Update Generators **	
						***********************/
//================================================================================================//
void Engine::UpdateGenerators()
{
	//update the active generators
	mActiveGenerators.remove_if(RemoveIfInActive);//update and remove 
	//test local generators for triggering
	mLocalGenerators.remove_if(RemoveIfActive);
	mLocalAnimGenerators.remove_if(UpdateAndRemoveAnim);
}
//================================================================================================//
						/**************************
						** Initialize generators **	
						***************************/
//================================================================================================//
void Engine::InitializeGenerators()
{
	//entity generators
	mActiveGenerators.clear();
	mLocalGenerators.clear();
	list<Entity*>::iterator iter;
	for(iter = mGameEnts.begin(); iter != mGameEnts.end(); iter++)
	{
		delete (*iter);
	}
	mGameEnts.clear();
	for(iter = mTopGameEnts.begin(); iter != mTopGameEnts.end(); iter++)
	{
		delete (*iter);
	}
	mTopGameEnts.clear();

	mLocalGenerators = mGenerators;
	mLocalGenerators.sort(SortGenerators);
	mLocalGenerators.remove_if(RemoveIfOffScreen);
	//anim generators
	mLocalAnimGenerators.clear();
	mLocalAnimGenerators = mAnimGenerators;
	mLocalAnimGenerators.sort(SortAnimGenerators);
	mLocalAnimGenerators.remove_if(RemoveEffectIfOffscreen);
}
//================================================================================================//
						/************************************
						** remove generators while editing **	
						*************************************/
//================================================================================================//
bool RemoveEdit(Generator g)
{
	if(/*g.ScrollOffset == (gpEngine->iEdScroll*32) &&*/ g.Pos == (gpEngine->EdPos*32))
		return true;
	return false;
}
bool RemoveAnimEdit(AnimGenerator g)
{
	if(/*g.ScrollOffset == (gpEngine->iEdScroll*32) &&*/ (g.mFX.mPos) == (gpEngine->EdPos*32))
		return true;
	return false;
}
void Engine::RemoveGeneratorsByEdit()
{
	mGenerators.remove_if(RemoveEdit);
	mAnimGenerators.remove_if(RemoveAnimEdit);
}