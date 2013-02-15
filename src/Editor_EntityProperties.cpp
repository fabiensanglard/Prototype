#include "Engine.h"
//================================================================================================//
						/**********************
						** properties Update **	
						***********************/
//================================================================================================//
void Engine::UpdateEntityProps()
{
	if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
		iTileFrame--;
	if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
		iTileFrame++;

	switch(iTileFrame)
	{
	case 0://num ents of generator
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
			mpTempGenerator->numEnts++;
		if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
			mpTempGenerator->numEnts--;
		break;
	case 1://delay of generator
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
			mpTempGenerator->tickDelay+=10;
		if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
			mpTempGenerator->tickDelay-=10;
		break;
	}

	switch(mpTempGenerator->iEntType)
	{
	case 0://ladybird
		{
		if(iTileFrame>4)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 4;
			LadyBird* plbird = (LadyBird*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					plbird->RateofFire++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					plbird->RateofFire--;
				break;
			case 3://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					plbird->bStartMovement = !plbird->bStartMovement;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					plbird->bStartMovement = !plbird->bStartMovement;
				break;
			case 4://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 1://turret
		{
		if(iTileFrame>4)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 4;
			Turret* pTurret = (Turret*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pTurret->RateofFire++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pTurret->RateofFire--;
				break;
			case 3://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pTurret->bDirection = !pTurret->bDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pTurret->bDirection = !pTurret->bDirection;
				break;
			case 4://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 640;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 2://serpent
		{
		if(iTileFrame>5)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 5;
			Serpent* pSerpent = (Serpent*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pSerpent->RateofFire++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pSerpent->RateofFire--;
				break;
			case 3://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pSerpent->bStartDirection = !pSerpent->bStartDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pSerpent->bStartDirection = !pSerpent->bStartDirection;
				break;
			case 4://record pattern
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					pRecordEnt = (ControlledEntity*)mpTempGenerator->mpEntity;
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(mpTempGenerator->Pos);
				}
				break;
			case 5://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 3://Rhino
		{
		if(iTileFrame>5)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 5;
			Rhino* pRhino = (Rhino*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pRhino->bStartDirection = !pRhino->bStartDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pRhino->bStartDirection = !pRhino->bStartDirection;
				break;
			case 3://record attack pattern
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					pRecordEnt = (ControlledEntity*)mpTempGenerator->mpEntity;
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(mpTempGenerator->Pos);
				}
				break;
			case 4://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pRhino->iStartOnScreen<2? pRhino->iStartOnScreen++:pRhino->iStartOnScreen=0;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pRhino->iStartOnScreen>0? pRhino->iStartOnScreen--:pRhino->iStartOnScreen=2;
				break;

			case 5://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					if(pRhino->iStartOnScreen==0)
					{
						mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 640;
						if(mpTempGenerator->ScrollOffset<0)
							mpTempGenerator->ScrollOffset = 0;
					}
					else if(pRhino->iStartOnScreen==1)
					{
						mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x+64;
					}
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 4://Rocket
		{
		if(iTileFrame>3)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 3;
			Rocket* pRocket = (Rocket*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pRocket->bDirection = !pRocket->bDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pRocket->bDirection = !pRocket->bDirection;
				break;
			case 3://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					pRocket->BlastOffPos =(float) mpTempGenerator->ScrollOffset;
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 640;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 5://Superturret
		{
		if(iTileFrame>3)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 3;
			SuperTurret* pTurret = (SuperTurret*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pTurret->bDirection = !pTurret->bDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pTurret->bDirection = !pTurret->bDirection;
				break;
			case 3://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 640;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 6://PowerDroid
		{
		if(iTileFrame>5)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 5;
			PowerDroid* pPowerDroid = (PowerDroid*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pPowerDroid->bStartDirection = !pPowerDroid->bStartDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pPowerDroid->bStartDirection = !pPowerDroid->bStartDirection;
				break;
			case 3://record attack pattern
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					pRecordEnt = (ControlledEntity*)mpTempGenerator->mpEntity;
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(mpTempGenerator->Pos);
				}
				break;
			case 4://what this droid will spawn
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pPowerDroid->iPowerUp<7? pPowerDroid->iPowerUp++:pPowerDroid->iPowerUp=0;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pPowerDroid->iPowerUp>0? pPowerDroid->iPowerUp--:pPowerDroid->iPowerUp=7;
				break;

			case 5://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 640;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 7://Bug
		{
		if(iTileFrame>5)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 5;
			Bug* pBug = (Bug*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pBug->RateofFire++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pBug->RateofFire--;
				break;
			case 3://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pBug->bStartDirection = !pBug->bStartDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pBug->bStartDirection = !pBug->bStartDirection;
				break;
			case 4://record pattern
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					pRecordEnt = (ControlledEntity*)mpTempGenerator->mpEntity;
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(mpTempGenerator->Pos);
				}
				break;
			case 5://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 8://breakscenery1
		{
		if(iTileFrame>4)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 4;
			BreakScenery* pBreak = (BreakScenery*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2:
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pBreak->frame<sprBreak1.numFrames-1? pBreak->frame++: pBreak->frame=0;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pBreak->frame>0? pBreak->frame--: pBreak->frame=(float)sprBreak1.numFrames-1;
				break;
			case 3:
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pBreak->iRotation<3? pBreak->iRotation++: pBreak->iRotation=0;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pBreak->iRotation>0? pBreak->iRotation--: pBreak->iRotation=3;
				break;
			case 4://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 640;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
					pBreak->spr = sprBreak1;
				}
				break;
			}
		}
		break;

	case 9://StarBurst
		{
		if(iTileFrame>4)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 4;
			StarBurst* pStarBurst = (StarBurst*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pStarBurst->RateofFire++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pStarBurst->RateofFire--;
				break;
			case 3://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pStarBurst->bStartMovement = !pStarBurst->bStartMovement;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pStarBurst->bStartMovement = !pStarBurst->bStartMovement;
				break;
			case 4://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 10://Fighter
		{
		if(iTileFrame>5)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 5;
			Fighter* pFighter = (Fighter*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pFighter->RateofFire++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pFighter->RateofFire--;
				break;
			case 3://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pFighter->bStartDirection = !pFighter->bStartDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pFighter->bStartDirection = !pFighter->bStartDirection;
				break;
			case 4://record pattern
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					pRecordEnt = (ControlledEntity*)mpTempGenerator->mpEntity;
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(mpTempGenerator->Pos);
				}
				break;
			case 5://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 11://checkpoint
		{
		if(iTileFrame>2)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 2;
			switch(iTileFrame)
			{
			case 2://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 13://Boss
		{
		if(iTileFrame>5)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 5;
			switch(iTileFrame)
			{
			case 2://record serpent1 attack pattern 1
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
//					gpEngine->InitializeGenerators();
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					if(!(((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern1 = (Serpent*)CreateEntity(2)))
						return;
					Serpent* pSerpent = ((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern1;
					pSerpent->bStartDirection = false;
					pSerpent->RateofFire = 1;
					pRecordEnt = (ControlledEntity*)(((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern1);
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(mpTempGenerator->mpEntity->Pos - Vec2(400,0));
					Boss* boss = new Boss;
					boss->Spawn(mpTempGenerator->mpEntity->Pos);
					GiveEntityToList(boss);
				}
				break;
			case 3://record serpent1 attack pattern 2
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
			//		gpEngine->InitializeGenerators();
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					if(!(((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern2 = (Serpent*)CreateEntity(2)))
						return;
					Serpent* pSerpent = ((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern2;
					pSerpent->bStartDirection = true;
					pSerpent->RateofFire = 1;
					pRecordEnt = (ControlledEntity*)(((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern2);
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(Vec2(mpTempGenerator->mpEntity->Pos.x-400,480));
					Boss* boss = new Boss;
					boss->Spawn(mpTempGenerator->mpEntity->Pos);
					GiveEntityToList(boss);
				}
				break;
			case 4://record serpent1 attack pattern 3
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
				//	gpEngine->InitializeGenerators();
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					if(!(((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern3 = (Serpent*)CreateEntity(2)))
						return;
					Serpent* pSerpent = ((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern3;
					pSerpent->bStartDirection = true;
					pSerpent->RateofFire = 1;
					pRecordEnt = (ControlledEntity*)(((Boss*)mpTempGenerator->mpEntity)->mSerpentAttackPattern3);
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(Vec2(mpTempGenerator->mpEntity->Pos.x+40,480));
					((Serpent*)pRecordEnt)->fRotation = -135;
					Boss* boss = new Boss;
					boss->Spawn(mpTempGenerator->mpEntity->Pos);
					GiveEntityToList(boss);
				}
				break;
			case 5://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 1100;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 14://MidBoss
		{
		if(iTileFrame>2)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 2;
			switch(iTileFrame)
			{
			case 2://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 750;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 15://trigger speedup
		{
		if(iTileFrame>3)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 3;
			TriggerSpeedUp* ptrigger = (TriggerSpeedUp*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2:
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					ptrigger->iMaxTicks+=10;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					ptrigger->iMaxTicks-=10;
				if(ptrigger->iMaxTicks<10)
					ptrigger->iMaxTicks=10;
				break;
			case 3://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;
	case 16://trigger slowdown
		{
		if(iTileFrame>3)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 3;
			TriggerSlowDown* ptrigger = (TriggerSlowDown*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2:
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					ptrigger->iMaxTicks+=10;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					ptrigger->iMaxTicks-=10;
				if(ptrigger->iMaxTicks<10)
					ptrigger->iMaxTicks=10;
				break;
			case 3://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 17://fire
		{
		if(iTileFrame>3)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 3;
			Fire* pfire = (Fire*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pfire->bTop = !pfire->bTop;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pfire->bTop = !pfire->bTop;
				break;

			case 3:
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					mpTempGenerator->ScrollOffset = (int)mpTempGenerator->Pos.x - 640;
					if(mpTempGenerator->ScrollOffset<0)
						mpTempGenerator->ScrollOffset = 0;
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 18://SolarFlare
		{
		if(iTileFrame>4)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 4;
			SolarFlare* pSolarFlare = (SolarFlare*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pSolarFlare->bStartDirection = !pSolarFlare->bStartDirection;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pSolarFlare->bStartDirection = !pSolarFlare->bStartDirection;
				break;
			case 3://record pattern
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bRecording = true;
					bUseEditor = false;
					bPlayBackMode = true;
					bShowEntityProps = false;
					Scroll = (float)(iEdScroll*32);
					pRecordEnt = (ControlledEntity*)mpTempGenerator->mpEntity;
					pRecordEnt->RegisterControl(mButtons);
					pRecordEnt->StartRecording();
					pRecordEnt->Spawn(mpTempGenerator->Pos);
				}
				break;
			case 4://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 19://FireBug
		{
		if(iTileFrame>4)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 4;
			FireBug* pFireBug = (FireBug*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pFireBug->RateofFire++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pFireBug->RateofFire--;
				break;
			case 3://direction
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pFireBug->bStartMovement = !pFireBug->bStartMovement;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pFireBug->bStartMovement = !pFireBug->bStartMovement;
				break;
			case 4://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;

	case 20://SpaceTrash
		{
		if(iTileFrame>3)
			iTileFrame = 0;
		if(iTileFrame<0)
			iTileFrame = 3;
			SpaceTrash* pSpaceTrash = (SpaceTrash*)mpTempGenerator->mpEntity;
			switch(iTileFrame)
			{
			case 2://ROF
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
					pSpaceTrash->SpreadRadius++;
				if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
					pSpaceTrash->SpreadRadius--;
				break;
			case 3://DONE
				if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
				{
					bShowEntityProps = false;
					bShowEntityTool = true;
				}
				break;
			}
		}
		break;
	}
}
//================================================================================================//
						/**********************
						** properties Render **	
						***********************/
//================================================================================================//
void Engine::RenderEntityProps()
{
	if(!mpTempGenerator)
		return;

	char sz[128];
	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.SetColor(1,1,1,1);
	mFont1.Print("Generator Properties",250,60);
	iTileFrame==0?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
	sprintf(sz,"Num Entities   %i",mpTempGenerator->numEnts);
	mFont1.Print(sz,120,80);
	iTileFrame==1?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
	sprintf(sz,"Delay spawn ticks    %i",mpTempGenerator->tickDelay);
	mFont1.Print(sz,120,100);	

	switch(mpTempGenerator->iEntType)
	{
	case 0://ladybird
		{
			LadyBird* plbird = (LadyBird*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("LadyBird properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Fire every %i Seconds",plbird->RateofFire);
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			plbird->bStartMovement?
				sprintf(sz,"Move up Initially"):sprintf(sz,"Move Down Initially");
			mFont1.Print(sz,120,250);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 1://turret
		{
			Turret* pturret = (Turret*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Turret properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Fire every %i Seconds",pturret->RateofFire);
			mFont1.Print(sz,120,210);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pturret->bDirection?
				sprintf(sz,"Turret sits on floor"):sprintf(sz,"Turret hangs from roof");
			mFont1.Print(sz,120,230);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 2://Serpent
		{
			Serpent* pSerpent = (Serpent*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Serpent properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Fire every %i Seconds",pSerpent->RateofFire);
			mFont1.Print(sz,120,210);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pSerpent->bStartDirection?
				sprintf(sz,"Start From Bottom"):sprintf(sz,"Start From Top");
			mFont1.Print(sz,120,230);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Attack Pattern.",120,250);
			iTileFrame==5?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 3://Rhino
		{
			Rhino* pRhino = (Rhino*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Rhino properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pRhino->bStartDirection?
				sprintf(sz,"Start Facing Right"):sprintf(sz,"Start Facing Left");
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Attack Pattern.",120,250);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			switch(pRhino->iStartOnScreen)
			{
			case 0:
				mFont1.Print("Start From Right.",120,270); break;
			case 1:
				mFont1.Print("Start From Left.",120,270); break;
			case 2:
				mFont1.Print("Start From Above.",120,270); break;
			}
			iTileFrame==5?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 4://rocket
		{
			Rocket* pRocket = (Rocket*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Rocket properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pRocket->bDirection?
				sprintf(sz,"Rocket sits on floor"):sprintf(sz,"Rocket hangs from roof");
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 5://Superturret
		{
			SuperTurret* pturret = (SuperTurret*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("SuperTurret properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pturret->bDirection?
				sprintf(sz,"Turret sits on floor"):sprintf(sz,"Turret hangs from roof");
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 6://PowerDroid
		{
			PowerDroid* pPowerDroid = (PowerDroid*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("PowerDroid properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pPowerDroid->bStartDirection?
				sprintf(sz,"Start Facing Right"):sprintf(sz,"Start Facing Left");
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Attack Pattern.",120,250);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			switch(pPowerDroid->iPowerUp)
			{
			case 0:
				mFont1.Print("Plasma.",120,270); break;
			case 1:
				mFont1.Print("Lightning.",120,270); break;
			case 2:
				mFont1.Print("Purple.",120,270); break;
			case 3:
				mFont1.Print("Red.",120,270); break;
			case 4:
				mFont1.Print("Speedup.",120,270); break;
			case 5:
				mFont1.Print("Homing Missile.",120,270); break;
			case 6:
				mFont1.Print("Bit Upgrade.",120,270); break;
			case 7:
				mFont1.Print("Cluster Missile.",120,270); break;
			}
			iTileFrame==5?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 7://Bug
		{
			Bug* pBug = (Bug*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Bug properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Fire every %i Seconds",pBug->RateofFire);
			mFont1.Print(sz,120,210);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pBug->bStartDirection?
				sprintf(sz,"Start From Bottom"):sprintf(sz,"Start From Top");
			mFont1.Print(sz,120,230);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Attack Pattern.",120,250);
			iTileFrame==5?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 8://breakscenery
		{
			BreakScenery* pBreak = (BreakScenery*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("BreakScenery properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("TILE:",120,270);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			switch(pBreak->iRotation)
			{
			case 0:	mFont1.Print("NORMAL:",120,290);break;
			case 1:	mFont1.Print("FLIPPED HORIZTONAL:",120,290);break;
			case 2:	mFont1.Print("FLIPPED VERTICAL:",120,290);break;
			case 3:	mFont1.Print("FLIPPED HORIZONTAL & VERTICAL:",120,290);break;
			}
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
			glColor4f(1,1,1,1);
			RenderSprite(sprBreak1,(int)pBreak->frame,300,270,(float)sprBreak1.GetWindowScale().first,(float)sprBreak1.GetWindowScale().second);
		}
		break;

	case 9://StarBurst
		{
			StarBurst* pStarBurst = (StarBurst*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("StarBurst properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Fire every %i Seconds",pStarBurst->RateofFire);
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pStarBurst->bStartMovement?
				sprintf(sz,"Move up Initially"):sprintf(sz,"Move Down Initially");
			mFont1.Print(sz,120,250);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 10://Fighter
		{
			Fighter* pFighter = (Fighter*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Fighter properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Fire every %i Seconds",pFighter->RateofFire);
			mFont1.Print(sz,120,210);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pFighter->bStartDirection?
				sprintf(sz,"Start From Right"):sprintf(sz,"Start From Left");
			mFont1.Print(sz,120,230);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Attack Pattern.",120,250);
			iTileFrame==5?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 11://checkpoint
		{
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("CheckPoint properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 13://Boss
		{
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Boss properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Serpent Attack Pattern1.",120,210);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Serpent Attack Pattern2.",120,230);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Serpent Attack Pattern3.",120,250);
			iTileFrame==5?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 14://MidBoss
		{
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("MidBoss properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 15://trigger speed up
		{
			TriggerSpeedUp* ptrigger = (TriggerSpeedUp*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Trigger SpeedUp properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Lasts for %i ticks",ptrigger->iMaxTicks);
			mFont1.Print(sz,120,210);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 16://trigger slow down
		{
			TriggerSlowDown* ptrigger = (TriggerSlowDown*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("Trigger SlowDown properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Lasts for %i ticks",ptrigger->iMaxTicks);
			mFont1.Print(sz,120,210);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 17://Fire
		{
			mFont1.SetColor(1,1,1,1);
			Fire* pfire = (Fire*)mpTempGenerator->mpEntity;
			mFont1.Print("Fire properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			!pfire->bTop?
			sprintf(sz,"Floor"):sprintf(sz,"Cieling");
			mFont1.Print(sz,120,250);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 18://solarflare
		{
			SolarFlare* pSolarFlare = (SolarFlare*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("SolarFlare properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pSolarFlare->bStartDirection?
				sprintf(sz,"Start From Bottom"):sprintf(sz,"Start From Top");
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Record Attack Pattern.",120,250);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 19://FireBug
		{
			FireBug* pFireBug = (FireBug*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("FireBug properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Fire every %i Seconds",pFireBug->RateofFire);
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			pFireBug->bStartMovement?
				sprintf(sz,"Start from right"):sprintf(sz,"start from left");
			mFont1.Print(sz,120,250);
			iTileFrame==4?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;

	case 20://SpaceTrash
		{
			SpaceTrash* pSpaceTrash = (SpaceTrash*)mpTempGenerator->mpEntity;
			mFont1.SetColor(1,1,1,1);
			mFont1.Print("SpaceTrash properties",250,190);
			iTileFrame==2?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			sprintf(sz,"Spread over %i Radius (x32)",pSpaceTrash->SpreadRadius);
			mFont1.Print(sz,120,230);
			iTileFrame==3?	mFont1.SetColor(1,1,0,1):mFont1.SetColor(1,1,1,1);
			mFont1.Print("Done..",440,400);
		}
		break;
	}

}