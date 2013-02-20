#include "Engine.h"
bool UpdateAndRemoveEntity(Entity* e)//not good, just remove not update
{
//	e->PreUpdate();
	if(e->IsActive)
		return false;
	delete e;
	e = NULL;
	return true;
}
//================================================================================================//
						/***************
						** Start Game **	
						****************/
//================================================================================================//
void Engine::StartGame()
{
	iStage = 1;
	//display loading message
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);
	glColor4f(1,1,1,1);
	mFont1.SetColor(0,1,1,1);
	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(0.5f,1.0f);
	mFont1.Print("LOADING...",550,430);
	SDL_GL_SwapBuffers();

	LoadMapData();
	mPlayer.StartGame();
	if(!bRecording || (bRecording && pRecordEnt == &mPlayer))
	{
		if(!bDemo)
		{
			mPlayer.RegisterControl(mButtons);
			bInGame = true;
		}
		else
		{
			demoSystem dem;
			//TODO: load demo here
			LoadDemo(dem,curDemo);
			curDemo++;
			if(curDemo>5)
				curDemo=1;
			mPlayer.mKeyBuffer = dem.mKeyBuffer;
			mPlayer.iSpawnOffset = dem.iScrollOffset;
		}
	}
	InitializeGame();
	if(mPlayer.iSpawnOffset == 0)
		DisplayStageTicks = 0;
	else
		DisplayStageTicks = 500;


	if(fGameDifficulty==2)
		fGameDifficulty=1.5f;

	bPauseScrolling = false;
}
//================================================================================================//
						/********************
						** Initialize Game **	
						*********************/
//================================================================================================//
void Engine::InitializeGame()
{
	fMapSpeed = 0;
	bGameCompleted = false;
	if(bBossFight && !bDemo)
		mPlayer.iSpawnOffset = 345*32;
	Scroll = (float)mPlayer.iSpawnOffset;
	mEffectsLayer1.clear();
	mEffectsLayer2.clear();
	mEffectsLayer3.clear();
	InitializeGenerators();
	mBullets.clear();
	mPlayerBullets.clear();
	mExplosions.clear();
	mPlayer.Spawn(Vec2(0,180));
	if(!bDemo)
	{
		stmStage.Load("Data/Sound/Stage1.ogg",true);
		//stmStage.Seek(0,30);
		stmStage.Play(30);
	}
	//stmStage.Play(30);
	mFade.StartFadeIn(mTimer.GetTime(),2,2);
	bPauseScrolling = false;


	if((mWeaponCheats && !bDemo) || (bBossFight && !bDemo))
	{
		PowerUp* p = new PowerUp;
		p->iType = 0;
		p->Spawn(Vec2(Scroll+550,130));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 1;
		p->Spawn(Vec2(Scroll+550,190));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 2;
		p->Spawn(Vec2(Scroll+550,250));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 3;
		p->Spawn(Vec2(Scroll+550,310));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 4;
		p->Spawn(Vec2(Scroll+550,70));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 4;
		p->Spawn(Vec2(Scroll+550,370));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 5;
		p->Spawn(Vec2(Scroll+500,190));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 7;
		p->Spawn(Vec2(Scroll+500,250));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 6;
		p->Spawn(Vec2(Scroll+500,130));
		gpEngine->GiveEntityToList(p);
		p = new PowerUp;
		p->iType = 6;
		p->Spawn(Vec2(Scroll+500,310));
		gpEngine->GiveEntityToList(p);
	}

}
//================================================================================================//
						/****************
						** Update Game **	
						****************/
//================================================================================================//
void Engine::UpdateGame(float delta)
{
	if(!bDemo && !bRecording)
	{
		if(!mPlayer.bJustSpawned && !mPlayer.bIsDead)
			if(KEY_PAUSE.state == JUST_PRESSED)
				bPaused = !bPaused;
	}
	if(bPaused)
		return;

	if(iDelayGameTicks >0)
	{
		iDelayGameTicks--;
		return;
	}

	UpdateMap();
	UpdateGenerators();
	UpdateEntities();
	if(!bRecording || (bRecording && pRecordEnt != &mPlayer))
		mPlayer.PreUpdate();
//	UpdateBullets();
	UpdateExplosions();
	UpdateSpriteEffects();
	CheckCollisions();

	mFade.UpdateFade(mTimer.GetTime());

	DisplayStageTicks++;

	if(bDemo)
	{
		if(mTimer.GetTime()>fUIDelay)
		{
			bGameOverFrame = !bGameOverFrame;
			if(!bGameOverFrame)
				fUIDelay = mTimer.GetTime()+0.5f;
			else
				fUIDelay = mTimer.GetTime()+1;
		}
		if(KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
		{
			SetState(&Engine::UpdateSplash,&Engine::RenderSplash);
			SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0);
			mFade.StartFadeIn(mTimer.GetTime(),2,0.5f);
			fUIDelay = mTimer.GetTime();
			bDemo = false;
			fGameDifficulty=3;
			iOption = 0;
			return;
		}
	}
}
//================================================================================================//
						/****************
						** Update Ents **	
						****************/
//================================================================================================//
void Engine::UpdateEntities()
{
	list<Entity*>::iterator e;
	for(e=mGameEnts.begin(); e!=mGameEnts.end(); e++)
	{
		(*e)->PreUpdate();
	}
	for(e=mTopGameEnts.begin(); e!=mTopGameEnts.end(); e++)
	{
		(*e)->PreUpdate();
	}
	UpdateBullets();

	mGameEnts.remove_if(UpdateAndRemoveEntity);//This is LEAKING!!
	mTopGameEnts.remove_if(UpdateAndRemoveEntity);//This is LEAKING!!
	//play record entity
	if(bRecording)
		pRecordEnt->PreUpdate();

}
//================================================================================================//
						/****************
						** Render Game **	
						****************/
//================================================================================================//
void Engine::RenderGame(float interp)
{
	BindMainContext();
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float s = UTIL_Misc::Interpolate(Scroll,oScroll,interp);
	glTranslatef(-s*0.5f,0,0);
		RenderMapLayer(3,interp,false);
		RenderSpriteEffects(interp,3);
	glTranslatef(s*0.5f,0,0);
	glTranslatef(-s*0.75f,0,0);
		RenderMapLayer(2,interp,false);
		RenderSpriteEffects(interp,2);
	glTranslatef(s*0.75f,0,0);
	//draw the warp post effect before player
	if(mPlayer.iForceCharge>10)
	{
		PostCharge(interp);
	}
	glTranslatef(-s,0,0);

	//beams
	list<Beam>::iterator b;
	for(b = mBeams.begin(); b != mBeams.end(); b++)
	{
		(*b).Render(interp);
	}
	//render player
	if(!bRecording || (bRecording && pRecordEnt == &mPlayer))
		mPlayer.Draw(interp);

	RenderEntities(interp);
	RenderBullets(interp);
	RenderMapLayer(1,interp,false);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	list<Entity*>::iterator iter;
	for(iter = mTopGameEnts.begin(); iter != mTopGameEnts.end(); iter++)//these ents must be drawn after the map
	{
		(*iter)->Draw(interp);
	}
	RenderSpriteEffects(interp,1);
	RenderExplosions(interp);
	glTranslatef(s,0,0);

	FinalizeMainContext();

	DoPostEffects(interp);
	glTranslatef(-s,0,0);
	RenderWaterTransparent(interp);//final pass on water
	glTranslatef(s,0,0);

	

	RenderHud(interp);

	mFade.RenderFade(mTimer.GetTime());

	if(DisplayStageTicks<300)
	{
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,imgStage1);
		if(DisplayStageTicks<100)
		{
			float a = (float)(DisplayStageTicks)/100.0f;
			glColor4f(1,1,1,a);
			mFont1.SetColor(1,1,0,a);
		}
		else if(DisplayStageTicks>250)
		{
			float a = 1.0f - ((float)(DisplayStageTicks-250)/50.0f);
			glColor4f(1,1,1,a);
			mFont1.SetColor(1,1,0,a);
		}
		else
		{
			glColor4f(1,1,1,1);
			mFont1.SetColor(1,1,0,1);
		}
		if(iStage == 1)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0,0); glVertex2f(64,100);
			glTexCoord2f(1,0); glVertex2f(596,100);
			glTexCoord2f(1,1); glVertex2f(596,228);
			glTexCoord2f(0,1); glVertex2f(64,228);
			glEnd();
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			mFont1.SetScale(1,1.0f);
			mFont1.SetAlignment(ALIGN_CENTER);
			mFont1.Print("MARS COLONY UNDER ATTACK",320,240);
		}
	}

	if(bDemo)
	{
		mFont1.SetAlignment(ALIGN_CENTER);
		mFont1.SetScale(1,0.5f);
		mFont1.SetColor(1,1,1,1);
		if(bGameOverFrame)
			mFont1.Print("PRESS START",320,210);

		glColor4f(1,1,1,1);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplash);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0); glVertex2f(440,400);
		glTexCoord2f(1,0); glVertex2f(636,400);
		glTexCoord2f(1,1); glVertex2f(636,450);
		glTexCoord2f(0,1); glVertex2f(440,450);
		glEnd();
	}
	if(bPaused)
	{
		mFont1.SetAlignment(ALIGN_CENTER);
		mFont1.SetScale(1,0.5f);
		mFont1.SetColor(1,1,1,1);
		mFont1.Print("PAUSED",320,210);
	}
}
//================================================================================================//
						/****************
						** Render Ents **	
						****************/
//================================================================================================//
void Engine::RenderEntities(float interp)
{
//	float s = UTIL_Misc::Interpolate(Scroll,oScroll,interp);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//this should be done on a per layer basis really
	list<Entity*>::iterator iter;
	for(iter = mGameEnts.begin(); iter != mGameEnts.end(); iter++)
	{
		(*iter)->Draw(interp);
	}


/*	//render player
	if(!bRecording || bRecording && pRecordEnt == &mPlayer)
		mPlayer.Draw(interp);
*/
	//play record entity
	if(bRecording)
		pRecordEnt->Draw(interp);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

}
//================================================================================================//
						/*******************
						** Update Bullets **	
						********************/
//================================================================================================//
bool UpdateBullet(Bullet& b)
{
	(*b.pUpdate)(b);
	if(b.IsActive)
		return false;
	return true;
}
bool UpdateBeam(Beam &b)
{
	b.Update();
	if(b.IsActive)
		return false;
	return true;
}
void Engine::UpdateBullets()
{
	mBullets.remove_if(UpdateBullet);
	mPlayerBullets.remove_if(UpdateBullet);
	//beams
	mBeams.remove_if(UpdateBeam);
}
//================================================================================================//
						/*******************
						** Render Bullets **	
						********************/
//================================================================================================//
void Engine::RenderBullets(const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	list<Bullet>::iterator iter;
	for(iter = mBullets.begin(); iter != mBullets.end(); iter++)
	{
		(*iter).pRender((*iter),interp);
	}
	for(iter = mPlayerBullets.begin(); iter != mPlayerBullets.end(); iter++)
	{
		(*iter).pRender((*iter),interp);
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

}
//================================================================================================//
						/*******************
						** spawn Bullets **	
						********************/
//================================================================================================//
void Engine::SpawnBullet(Bullet b,	bool IsPlayers)
{
	IsPlayers? 
		mPlayerBullets.push_back(b):
		mBullets.push_back(b);
}
void Engine::SpawnBeam(vector<Vec2> p, int segs, int random, float color[4], float duration, float thickness)
{
	Beam b;
	b.Spawn(p,segs,random,color,duration,thickness);
	mBeams.push_back(b);
}

//================================================================================================//
						/*********************
						** check collisions **	
						**********************/
//================================================================================================//
void Engine::CheckCollisions()
{
	list<Bullet>::iterator b;
	list<Entity*>::iterator e;
	//test player bullets against enemies
	for(b=mPlayerBullets.begin(); b!=mPlayerBullets.end(); b++)
	{
		for(e=mGameEnts.begin(); e!=mGameEnts.end(); e++)
		{
			float entlife = (*e)->fStartLife;
			if((*e)->CheckCollided((*b).sphere,(*b).fLife))
			{
				if((*b).pCollide != Collide_LifeBullet)
					(*b).fLife -= entlife;
				(*b).pCollide((*b));
			}
		} 
		for(e=mTopGameEnts.begin(); e!=mTopGameEnts.end(); e++)
		{
			float entlife = (*e)->fStartLife;
			if((*e)->CheckCollided((*b).sphere,(*b).fLife))
			{
				if((*b).pCollide != Collide_LifeBullet)
					(*b).fLife -= entlife;
				(*b).pCollide((*b));
			}
		} 
	}
	//enemy bullets
	if(!mPlayer.bIsDead)
	for(b=mBullets.begin(); b!=mBullets.end(); b++)
	{
		//test against orb
		if(mPlayer.bHasOrb)
			if((*b).pCollide != Collide_PowerBullet)
				if(Collision::SphereSphereOverlap(mPlayer.mOrb.mSphere,(*b).sphere))
				{
					(*b).fLife-=0.5f;
					(*b).pCollide((*b));
					SND_DEFLECT;
					mPlayer.iScore+=10;
				}
		//test player against bullets
		if(!bWimpMode)
		if(Collision::SphereSphereOverlap(mPlayer.mSphere,(*b).sphere))
		{
			if(mPlayer.CheckCollided(mPlayer.mSphere,(*b).fLife))//kills the player
				iDelayGameTicks = 10;
			(*b).fLife-=0.5f;
			(*b).pCollide((*b));
			break;
		}
	}
	//test enemies
	if(!mPlayer.bIsDead)
	{
		for(e=mGameEnts.begin(); e!=mGameEnts.end(); e++)
		{
			//test against orb
			if(mPlayer.bHasOrb)
				(*e)->CheckCollided(mPlayer.mOrb.mSphere,0.5f);
			//test player against enemies
				if(!bWimpMode)
			if((*e)->CheckCollided(mPlayer.mSphere,0.5f))
			{
				if(mPlayer.CheckCollided(mPlayer.mSphere,(*e)->fLife))//kills the player
					iDelayGameTicks = 10;
				break;
			}
		}
		for(e=mTopGameEnts.begin(); e!=mTopGameEnts.end(); e++)
		{
			//test against orb
			if(mPlayer.bHasOrb)
				(*e)->CheckCollided(mPlayer.mOrb.mSphere,0.5f);
			//test player against enemies
			if(!bWimpMode)
			if((*e)->CheckCollided(mPlayer.mSphere,0.5f))
			{
				if(mPlayer.CheckCollided(mPlayer.mSphere,(*e)->fLife))//kills the player
					iDelayGameTicks = 10;
				break;
			}

		}
	}
}
//================================================================================================//
						/***************
						** explosions **	
						****************/
//================================================================================================//
bool UpdateAndRemoveExplosions(Explosion &e)
{
	if(!e.IsActive)
		return true;
	e.Update();
	return false;
}
void Engine::SpawnExplosion(SpriteSheet spr, Vec2 pos, float size, float duration, float rotation, bool glow)
{
	
	mExplosions.push_back(Explosion(spr,pos,size,duration,rotation,glow));
}
void Engine::UpdateExplosions()
{
	mExplosions.remove_if(UpdateAndRemoveExplosions);
}
void Engine::RenderExplosions(const float interp)
{
	list<Explosion>::iterator iter;
	for(iter = mExplosions.begin(); iter != mExplosions.end(); iter++)
	{
		(*iter).Render(interp);
	}
}
//================================================================================================//
						/*****************
						** SpriteEffect **	
						******************/
//================================================================================================//
void Engine::SpawnSpriteEffect(int layer, Vec2 pos, Vec2 size, SpriteSheet sprite, float duration, bool glow, Vec2* target, bool playOnce)
{
	SpriteEffect s;
	s.Spawn(pos,size,sprite,duration,glow,target,playOnce);
	switch(layer)
	{
	case 1:
		mEffectsLayer1.push_back(s);
		break;
	case 2:
		mEffectsLayer2.push_back(s);
		break;
	case 3:
		mEffectsLayer3.push_back(s);
		break;
	}
}
float gLayer;//getting hacky now.
bool UpdateAndRemoveEffect(SpriteEffect& s)
{
	if((s.oPos.x+s.mSize.x)<(float)(gpEngine->Scroll*gLayer))
		s.bIsActive = false;
	if(!s.bIsActive)
		return true;
	s.Update();


	return false;
}
void Engine::UpdateSpriteEffects()
{
	gLayer = 0.5f;
	mEffectsLayer3.remove_if(UpdateAndRemoveEffect);
	gLayer = 0.75f;
	mEffectsLayer2.remove_if(UpdateAndRemoveEffect);
	gLayer = 1;
	mEffectsLayer1.remove_if(UpdateAndRemoveEffect);
}
void Engine::RenderSpriteEffects(const float interp, int layer)
{
	float railMulti=1;
	if(layer>1 && mPlayer.bHasOrb && mPlayer.mOrb.iWeaponType == 1)
	{
			if(mPlayer.iForceCharge>100)
			{
				float tick = (float)(mPlayer.iForceCharge-100);
				if(tick>50)tick=50;
				railMulti=1.0f - (tick/65.0f);
			}
	}
	glColor4f((1.0f/(float)layer)*railMulti,(1.0f/(float)layer)*railMulti,(1.0f/(float)layer)*railMulti,1);
	list<SpriteEffect>::iterator iter;
	switch(layer)
	{
	case 1:
		for(iter = mEffectsLayer1.begin(); iter != mEffectsLayer1.end(); iter++)
			(*iter).Draw(interp);
		break;
	case 2:
		for(iter = mEffectsLayer2.begin(); iter != mEffectsLayer2.end(); iter++)
			(*iter).Draw(interp);
		break;
	case 3:
		for(iter = mEffectsLayer3.begin(); iter != mEffectsLayer3.end(); iter++)
			(*iter).Draw(interp);
		break;
	}
}
//================================================================================================//
						/*********************
						** start completion **	
						**********************/
//================================================================================================//

void Engine::StartCompleted()
{
	bGameCompleted = true;
	SetStateTimed(&Engine::UpdateCongratulations,&Engine::RenderCongratulations,5,&Engine::FadeInEndCompleted);
	mFade.StartFadeOut(mTimer.GetTime(),2,5.1f);
	fUIDelay = mTimer.GetTime()+5;
	stmStage.Stop(FSOUND_ALL);
	gpEngine->smpForceCharge.Stop(FSOUND_ALL);
	SND_LARGEEXPLODE2;
	gpEngine->stmSplash.Load("Data/Sound/HiScores.ogg",false);
	gpEngine->stmSplash.Play(30);

}
void Engine::FadeInEndCompleted()
{
	StartFadeIn();
	SetStateTimed(&Engine::UpdateCongratulations,&Engine::RenderCongratulations,20,&Engine::EndCompleted);
}
void Engine::EndCompleted()
{
	SetStateTimed(NULL,&Engine::RenderIntro,3,&Engine::StartGameOver);
	mFade.StartFadeOut(mTimer.GetTime(),2,3.1f);
	IntroTimer = mTimer.GetTime()+1;
	fUIDelay = mTimer.GetTime()+1;
}