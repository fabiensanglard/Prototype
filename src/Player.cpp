#include "Player.h"
#include "Engine.h"
#include "RandExplosion.h"
bool bDemoIsFinished;
void Player::StartGame()
{
	iScore = 0;
	if(gpEngine->fGameDifficulty==3)//easy give more lives
		iLives = 3;
	else
		iLives = 2;
	iSpawnOffset = 0;
	iDelayBeforeRespawn = 0;
}
//================================================================================================//
						/******************
						** Player Spawn **	
						*******************/
//================================================================================================//
void Player::Spawn(Vec2 pos)
{
	bDemoIsFinished = false;
	IsActive = true;
	pos.x=(float)iSpawnOffset-750;
	pos.y= 260;
	oPos = Pos = pos;
	Vel = Vec2(0,0);
	Speed = 0.75f;
	frame = 9;
	fForceFrame = 0;
	fOrbFrame = 0;
	//spawn player with orb for testing
	bHasOrb = false;
	mOrb.Spawn(Pos);
	mOrb.GiveTarget(&Pos);
	fLastFireTime =gpEngine->mTimer.GetTime();
	fLastPlasmaTime =gpEngine->mTimer.GetTime();
	fLastMissileFired =gpEngine->mTimer.GetTime();
	iForceCharge = 0;
	fLife = 1;
	mSphere = Sphere(12,pos+Vec2(24,32));
	bJustSpawned = true;
	justSpawnedTime = 300;//gpEngine->mTimer.GetTime()+7;
	mSpawnBeamTicks = 0;
	bIsDead = false;
	bSpawnOrb = false;
	iSpawnOrbTicks = 0;
	mWaterticks = 0;
	iMissileType = 0;
	numBits = 0;
	bit1Ticks=0;
	bit2Ticks=0;
	bitFrame=0;
	iMaxPulseShots=0;
	bFullChargeFlash = false;
	iChargeFlash = 0;
	fArcCharge = 0;
	iDamagedControlTicks=0;
	bDamagedControls = false;
	if(gpEngine->fGameDifficulty!=1)
		numShields = 3;
	shieldTime = gpEngine->mTimer.GetTime();
	if(gpEngine->bDemo)
		SelfControl();

	iTicksToScreenCenter = 0;
}
//================================================================================================//
						/******************
						** Player Update **	
						*******************/
//================================================================================================//
void Player::Update()
{
	if(!IsActive)
		return;
	oPos = Pos;

	if(gpEngine->bGameCompleted)
	{
		if(iTicksToScreenCenter==0)
		{
			StoredPos = Pos;
		}
		if(iTicksToScreenCenter<100)
		{
			Pos.x = UTIL_Misc::TimeRamp(StoredPos.x,gpEngine->Scroll+320,0,100,(float)iTicksToScreenCenter);
			Pos.y = UTIL_Misc::TimeRamp(StoredPos.y,240,0,100,(float)iTicksToScreenCenter);
		}	
		else if(iTicksToScreenCenter==100)
		{
			StoredPos = Pos;
		}
		else
		{
			Pos.x = UTIL_Misc::TimeRamp(StoredPos.x,gpEngine->Scroll+920,100,200,(float)iTicksToScreenCenter);
			mSpawnBeamTicks++;
			if(mSpawnBeamTicks>1)
			{
				mSpawnBeamTicks=0;
				vector<Vec2> points;
				points.push_back(Vec2(Pos.x-500,Pos.y+20));
				points.push_back(Vec2(Pos.x-250,Pos.y+20));
				points.push_back(Vec2(Pos.x+275,Pos.y+20));
				float col[4]={1,0.3f,0,1};
				gpEngine->SpawnBeam(points,3,1,col,0.085f,25);
			}
		}
		iTicksToScreenCenter++;
		return;
	}

	if(!gpEngine->bPauseScrolling)
		Pos.x+=SCROLL_SPEED + gpEngine->fMapSpeed;

	//im dead
	if((gpEngine->bDemo &&  bDemoIsFinished) || bIsDead)
	{
		iDelayBeforeRespawn--;
		if(iDelayBeforeRespawn==100)
		{
			gpEngine->mFade.StartFadeOut(gpEngine->mTimer.GetTime(),1,2.5f);
		}
		if(iDelayBeforeRespawn<0)
		{
			if(gpEngine->bDemo)
			{
				//printf("Demo ended\n");
				gpEngine->StartFrontEnd();
				return;
			}

			if(iLives>0)
			{
				iLives--;
				gpEngine->InitializeGame();
			}
			else
			{
				gpEngine->StartGameOver();
				IsActive = false;
			}
		}

		if(bIsDead)
		{
	//		printf("Died\n");
			return;
		}
	}

	if(!gpEngine->bRecording || (gpEngine->bRecording && gpEngine->pRecordEnt != this))
//	if(bPlayBack)
	if(gpEngine->bDemo)
		PlayBack();

	float waterMult = 1;
	if(InWater())
		waterMult = 0.75f;

	if(bSpawnOrb)
	{
		iSpawnOrbTicks<25?iSpawnOrbTicks++:bSpawnOrb = false;
		
	}	
	if(bJustSpawned)//spawn intro
	{
		justSpawnedTime--;
		float t = justSpawnedTime/50.0f;//-gpEngine->mTimer.GetTime();
		if(t<0)
			bJustSpawned = false;
		if(t>=2)
		{
			Pos.x+=5.5f;
			mSpawnBeamTicks++;
			if(mSpawnBeamTicks>1)
			{
				mSpawnBeamTicks=0;
				vector<Vec2> points;
				points.push_back(Vec2(Pos.x-500,Pos.y+40));
				points.push_back(Vec2(Pos.x-250,Pos.y+40));
				points.push_back(Vec2(Pos.x+275,Pos.y+40));
				float col[4]={1,0.3f,0,1};
				gpEngine->SpawnBeam(points,3,1,col,0.085f,25);
			}
		}
		else if(t>=1 && t<2)
			frame<15?frame+=0.5f:frame=15;
		else if(t<0.5f)
			frame>9?frame-=0.5f:frame=9;
		if(t>=0.5f && t<2)
			Pos.x-=2.0f;
		return;
	}

	if(bDamagedControls)
	{
		iDamagedControlTicks++;
		if(iDamagedControlTicks>150)
		{
			bDamagedControls = false;
			iDamagedControlTicks = 0;
		}
	}

	if(KEY_RIGHT.state == PRESSED)
	{
		if(bDamagedControls)
			Vel.x-=Speed;
		else
			Vel.x+=Speed;
	}
	if(KEY_LEFT.state == PRESSED)
	{
		if(bDamagedControls)
			Vel.x+=Speed;
		else
			Vel.x-=Speed;
	}
	if(KEY_DOWN.state == PRESSED)
	{
		if(bDamagedControls)
			Vel.y-=Speed;
		else
			Vel.y+=Speed;
		frame-=0.75f;
		if(frame<0)
			frame = 0;
	}
	if(KEY_UP.state == PRESSED)
	{
		if(bDamagedControls)
			Vel.y+=Speed;
		else
			Vel.y-=Speed;
		frame+=0.75f;
		if(frame>15)
			frame = 15;
	}

	//centralize animation
	if((KEY_UP.state == RELEASED && KEY_DOWN.state == RELEASED) || (KEY_UP.state == PRESSED && KEY_DOWN.state == PRESSED))
	{
		if(frame<8)
			frame+=0.75f;
		else if(frame>9)
			frame-=0.75f;
	}

	//clamp to screen borders
	if(Pos.x<gpEngine->Scroll)
		Pos.x = gpEngine->Scroll;
	if(Pos.x>gpEngine->Scroll+590)
		Pos.x = gpEngine->Scroll+590;
	if(Pos.y<10)
		Pos.y = 10;
	if(Pos.y>410)
		Pos.y = 410;
	if(KEY_ATTACK1.state == PRESSED)
	{
		bool charge = true;
		if(bHasOrb)
		if(mOrb.iWeaponType == 2)
		if(iMaxPulseShots>0)
			charge = false;

		if(charge)
		{
			if(iForceCharge<2)
				SND_FORCECHARGE;

			iForceCharge++;
  			fForceFrame+=0.5f;
			if(fForceFrame>31)
				fForceFrame-=31;
			if(iForceCharge == 10)
				gpEngine->fForceChargeStart = gpEngine->mTimer.GetTime();

			if(iForceCharge>=100)
			{
				!bFullChargeFlash?iChargeFlash++:iChargeFlash--;
				if(iChargeFlash<0)
				{
					bFullChargeFlash=!bFullChargeFlash;
					iChargeFlash=0;
				}
				else if(iChargeFlash>25)
				{
					bFullChargeFlash=!bFullChargeFlash;
					iChargeFlash=25;
				}
			}

			if(mOrb.iWeaponType == 1 && bHasOrb)
				fArcCharge++;
		}
	}
	if(KEY_ATTACK1.state == JUST_RELEASED)
	{
		bFullChargeFlash = false;
		iChargeFlash = 0;
		if(fArcCharge>200)
			bDamagedControls = true;
		fArcCharge=0;
		SND_STOPFORCECHARGE;
		if(iForceCharge>=10)
		{
			int f = iForceCharge>100? 100: iForceCharge;
			Bullet b;
			b.pSpawn = Spawn_ForceBullet;
			b.pUpdate = Update_ForceBullet;
			b.pRender = Render_ForceBullet;
			b.pCollide = Collide_ForceBullet;
			b.frame = (float)(f/25);
			if(b.frame<1)b.frame = 1;
			(*b.pSpawn)(b,Vec2(Pos.x+48,Pos.y),Vec2(13,0));
			gpEngine->SpawnBullet(b,true);
			gpEngine->smpForceShot.Play(2);

			if(bHasOrb)
			{
				if(mOrb.iWeaponType == 1)
					FireArc();
				else if(mOrb.iWeaponType == 2)
					if(iMaxPulseShots<=0)
						iMaxPulseShots = (int)((float)(iForceCharge>100? 100: iForceCharge)/1.55f);
			}
		}
		iForceCharge = 0;
	}
	Pos+=Vel;
	Vel*=0.85f;
	Vel*=waterMult;//if in water slow down a bit
	if(bHasOrb)
	{
		if(KEY_ATTACK2.state == PRESSED  && !bSpawnOrb)
			mOrb.bLockState = false;
		else if(KEY_ATTACK2.state == JUST_RELEASED)
		{
			mOrb.bLockState = true;
			Vec2 np;
			np=mOrb.Pos - Pos;
			mOrb.mLockOffset = np;
			for(int n=0;n<6;n++)
			{
				np=mOrb.mChain[n] - Pos;
				mOrb.mChainOffset[n] = np;
			}
		}
		mOrb.PreUpdate();
		fOrbFrame>7?fOrbFrame-=7:fOrbFrame+=0.5f;

		//rotate the orb
		if(!mOrb.bLockState)
		{
			Line l1(Pos,mOrb.Pos);
			Line l2;
			l2 =Line(mOrb.Pos,Vec2(mOrb.Pos.x,mOrb.Pos.y+100));

			mOrb.rot = RAD2DEG(l1.Angle(l2));
			if(mOrb.Pos.x>Pos.x)
				mOrb.rot=-mOrb.rot;
			mOrb.rot = UTIL_Misc::Wrap(mOrb.rot,360);
		}
	}

	if(KEY_ATTACK1.state == JUST_PRESSED)
	{
		//fire missiles
		if(iMissileType==1)
		{
			if(gpEngine->mTimer.GetTime()>fLastMissileFired+1.25f)
			{
				fLastMissileFired = gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_HomingMissile;
				b.pUpdate = Update_HomingMissile;
				b.pRender = Render_HomingMissile;
				b.pCollide = Collide_HomingMissile;
				(*b.pSpawn)(b,mSphere.p,Vec2(0,-5));
				gpEngine->SpawnBullet(b,true);
				(*b.pSpawn)(b,mSphere.p,Vec2(0,5));
				gpEngine->SpawnBullet(b,true);
				SND_LASER1;
			}
		}
		else if(iMissileType == 2)
		{
			if(gpEngine->mTimer.GetTime()>fLastMissileFired+0.5f)//slow rate of fire, but big damage
			{
				fLastMissileFired =gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_GravityBullet;
				b.pUpdate = Update_ClusterBullet;
				b.pRender = Render_ClusterBullet;
				b.pCollide = Collide_ClusterBullet;
				(*b.pSpawn)(b,mSphere.p+Vec2(15,-20),Vec2(5,0));
				b.rot = 180;
				gpEngine->SpawnBullet(b,true);
				SND_LASER7;
			}
		}
		//fire weak bullets from ship
		if(!bHasOrb || (bHasOrb && mOrb.iWeaponType !=3))
		if(gpEngine->mTimer.GetTime()>fLastFireTime+0.125f)
		{
			fLastFireTime =gpEngine->mTimer.GetTime();
			Bullet b;
			b.pSpawn = Spawn_BasicBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_PlayerBullet;
			b.pCollide = Collide_PlayerBullet;
			(*b.pSpawn)(b,Vec2(Pos.x+48,Pos.y+20),Vec2(18,0));
			gpEngine->SpawnBullet(b,true);
			SND_LASER1;
				if(numBits>=1)
				{
					(*b.pSpawn)(b,Vec2(Pos.x,mSphere.p.y-46),Vec2(18,0));
					gpEngine->SpawnBullet(b,true);
				}
				if(numBits>=2)
				{
					(*b.pSpawn)(b,Vec2(Pos.x,mSphere.p.y+20),Vec2(18,0));
					gpEngine->SpawnBullet(b,true);
				}
		}
		//fire plasma
		if(bHasOrb)
		if(mOrb.iWeaponType == 0)
		{
			if(gpEngine->mTimer.GetTime()>fLastPlasmaTime+0.115f)
			{
				fLastPlasmaTime =gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_PlasmaBullet;
				b.pUpdate = Update_PlasmaBullet;
				b.pRender = Render_PlasmaBullet;
				b.pCollide = Collide_PlasmaBullet;
				Vec2 u,r;
				UTIL_Misc::MakeVectors(mOrb.rot,u,r);
				b.rot = mOrb.rot;
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,32)),Vec2(r*15));
				gpEngine->SpawnBullet(b,true);
				b.rot = mOrb.rot-25;
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,32)),Vec2(r*15+u*5));
				gpEngine->SpawnBullet(b,true);
				b.rot = mOrb.rot+25;
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,32)),Vec2(r*15+u*-5));
				gpEngine->SpawnBullet(b,true);
				b.rot = mOrb.rot-15;
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,32)),Vec2(r*15+u*2.5f));
				gpEngine->SpawnBullet(b,true);
				b.rot = mOrb.rot+15;
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,32)),Vec2(r*15+u*-2.5f));
				gpEngine->SpawnBullet(b,true);
				SND_LASER4;
			}
		}
		else if(mOrb.iWeaponType == 3)
		{
			if(gpEngine->mTimer.GetTime()>fLastFireTime+0.125f)
			{
				fLastFireTime =gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_ForceBullet;
				b.pUpdate = Update_ForceBullet;
				b.pRender = Render_ForceBullet;
				b.pCollide = Collide_ForceBullet;
				b.frame = 5;
				b.rot = -90;
				(*b.pSpawn)(b,Vec2(Pos.x+48,Pos.y),Vec2(13,0));
				gpEngine->SpawnBullet(b,true);
				if(numBits>=1)
				{
					(*b.pSpawn)(b,Vec2(Pos.x+6,mSphere.p.y-60),Vec2(13,0));
					gpEngine->SpawnBullet(b,true);
				}
				if(numBits>=2)
				{
					(*b.pSpawn)(b,Vec2(Pos.x+6,mSphere.p.y+4),Vec2(13,0));
					gpEngine->SpawnBullet(b,true);
				}
				Vec2 u,r;
				UTIL_Misc::MakeVectors(mOrb.rot,u,r);
				b.rot = mOrb.rot;
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,0)+Vec2(u*10)),Vec2(r*13));
				gpEngine->SpawnBullet(b,true);
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,0)-Vec2(u*10)),Vec2(r*13));
				gpEngine->SpawnBullet(b,true);
				SND_LASER6;

			}
		}
	}

	if(bHasOrb)
	if(mOrb.iWeaponType == 2)//constantly fire the pulse charge
	{
		if(iMaxPulseShots>0)
		{
			if(gpEngine->mTimer.GetTime()>fLastPlasmaTime+0.05f)
			{
				fLastPlasmaTime =gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_PulseBullet;
				b.pUpdate = Update_PulseBullet;
				b.pRender = Render_PulseBullet;
				b.pCollide = Collide_PulseBullet;
				Vec2 u,r;
				UTIL_Misc::MakeVectors(mOrb.rot,u,r);
				b.rot = mOrb.rot;
				(*b.pSpawn)(b,Vec2(mOrb.Pos+Vec2(32,32)),Vec2(r*15));
				gpEngine->SpawnBullet(b,true);
				iMaxPulseShots--;
				SND_LASER2;
			}
		}
	}


	//bits
	if(numBits>=1)
		bit1Ticks++;
	if(numBits>=2)
		bit2Ticks++;
	bitFrame<6?bitFrame+=0.5f:bitFrame-=6;
	//woops I collided with the map, kill me
	if(CollidedWithMap())
		CheckCollided(mSphere,1);
}
//================================================================================================//
						/************************
						** virtual constructor **	
						*************************/
//================================================================================================//
Player* Player::Clone()const
{
	return new Player(*this);
}
//================================================================================================//
						/******************
						** Render Player **	
						*******************/
//================================================================================================//
void Player::Draw(const float interp)
{
	if(!IsActive || bIsDead)
		return;

	float x=UTIL_Misc::Interpolate(Pos.x,oPos.x,interp);
	float y=UTIL_Misc::Interpolate(Pos.y,oPos.y,interp);

	if((!bJustSpawned && gpEngine->bWimpMode) || (!bJustSpawned && numShields>0))
	{
		UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossPush);
		UTIL_GL::SetBlend(GL_ONE, GL_ONE);
		float sine = gpEngine->mTimer.GetTime()*0.5f;
		if(gpEngine->bWimpMode || numShields==3)
			glColor4f(0.3f,1,0.3f,1);
		else if(numShields==2)
			glColor4f(1,1,0,1);
		else if(numShields==1)
			glColor4f(1,0,0,1);
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5f,0-sine);
		glVertex2f(x+32, y+32);
		float degree = (float)(360/15);
		float d=0;
		bool flipTex=false;

		glColor4f(0,0,0,1);
		for(int n=0; n<16; n++)
		{
			if(!flipTex)
				glTexCoord2f(0,0.25f-sine);
			else
				glTexCoord2f(1,0.25f-sine);
			flipTex=!flipTex;
			glVertex2f((x+32) + sin(d*PI/180) * 50, (y+32) + cos(d*PI/180) * 35);
			d+=degree;
		}
		
		glEnd();
	}
	

	if(!gpEngine->bGameCompleted)
	{	//render chain
		if(bHasOrb)
		{
			glColor4f(0,1,1,1);
			if(bSpawnOrb)
				glColor4f(0,1,1,(float)iSpawnOrbTicks/25.0f);
		
	//		UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE);
			UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgChain);
			float s = gpEngine->mTimer.GetTime()*5;
			Vec2 norm,p1,p2, oldn;
			glBegin(GL_QUADS);
			p1 = Vec2(x,y);
			p1 += Vec2(32,32);
			p2 = UTIL_Misc::Interpolate(mOrb.mChain[0],mOrb.mOChain[0],interp);
			norm = MakeNormalForPoints(p1,p2)*5;
			glTexCoord2f(0,0-s);
			glVertex2fv(p1.v);
			glTexCoord2f(1,0-s);
			glVertex2fv((p1+norm).v);
			glTexCoord2f(1,1-s);
			glVertex2fv((p2+norm).v);
			glTexCoord2f(0,1-s);
			glVertex2fv(p2.v);
			oldn = norm;
			for(int n=0;n<5;n++)
			{
				p1 = UTIL_Misc::Interpolate(mOrb.mChain[n],mOrb.mOChain[n],interp);
				p2 = UTIL_Misc::Interpolate(mOrb.mChain[n+1],mOrb.mOChain[n+1],interp);
				norm = MakeNormalForPoints(p1,p2)*5;
				glTexCoord2f(0,0-s);
				glVertex2fv(p1.v);
				glTexCoord2f(1,0-s);
				glVertex2fv((p1+oldn).v);
				glTexCoord2f(1,1-s);
				glVertex2fv((p2+norm).v);
				glTexCoord2f(0,1-s);
				glVertex2fv(p2.v);
				oldn = norm;
			}
			glEnd();
		}
	}

	//render ship
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
	if(bJustSpawned)
	{
		float t = justSpawnedTime/50.0f;//-gpEngine->mTimer.GetTime();
		if(t<2)
		t>0.5f? t=((t+0.5f)/2)-0.5f: t=0;
		if(t>1)t=1;
		RenderSprite(gpEngine->sprPlr,(int)frame,x,y,64+(32*t),64+(32*t));
	}
	else
		RenderSprite(gpEngine->sprPlr,(int)frame,x,y,64,64);

	//render damaged controls
	if(bDamagedControls)
	{
		glColor4f(1,1,1,0.5f);
		UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE );
		RenderSprite(gpEngine->sprFireBall,rand()%15,x-50,y-20,170,120);
	}

	if(!gpEngine->bGameCompleted)
	{
		//render bits
		if(numBits>0)
		{
			float c = bit1Ticks/100.0f;
			if(c>1)c=1;
			glColor4f(1,1,1,c);
			RenderSprite(gpEngine->sprBit,(int)bitFrame,x-16,y-16,32,32);
			if(numBits>1)
			{
				c = bit2Ticks/100.0f;
				if(c>1)c=1;
				glColor4f(1,1,1,c);
				RenderSprite(gpEngine->sprBit,(int)bitFrame,x-16,y+48,32,32);
			}

		}

		if(bHasOrb)
		{
			mOrb.Draw(interp);
			if(mOrb.iWeaponType == 1 && fArcCharge>50)
			{
				float size = (float)fArcCharge-50;
				if(size>64)size=64;
				Vec2 right,up;
				UTIL_Misc::MakeVectors(mOrb.rot,up,right);
				Vec2 np = UTIL_Misc::Interpolate(mOrb.Pos,mOrb.oPos,interp)+Vec2(32,32)+up*7;
				glColor4f(1,1,1,0.5f);
				UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE );
				RenderRotatedSprite(gpEngine->sprFireBall,rand()%15,np.x,np.y,size,size,mOrb.rot);
				glColor4f(1,1,1,1);
			}
		}

		//render force charging
		if(iForceCharge>10)
		{
			UTIL_GL::SetBlend(GL_ONE, GL_ONE);
			glColor4f(0,1,1,1);
			x+=48;
			y-=4;
			for(int n=0;n<2;n++)
			{
				int f;
				switch(n)
				{
				case 0:
					RenderSprite(gpEngine->sprForceCharge,(int)fForceFrame,x,y,96,64);
					break;
				case 1:
	//				f= (int)fForceFrame+13;
	//				if(f>26)f-=26;
					f= (int)fForceFrame+15;
					if(f>31)f-=31;
					RenderSprite(gpEngine->sprForceCharge,f,x,y,96,64);
					break;
				}
			}
			UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1,1,1,1);
		}
	}
/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void Player::NeedsToBeRemoved()
{
	if(gpEngine->bDemo)
	{
		if((int)mKeyBuffer.size()<=20 && (int)mKeyBuffer.size()>=10)
		{
//			printf("Demo is nearing end\n");
			CheckCollided(mSphere,0);
		}
	}
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Player::CheckCollided(Sphere s, float damage)
{
	if(!IsActive || bIsDead || bDemoIsFinished)
		return false;

	if(gpEngine->bDemo)
		bDemoIsFinished =true;

	if(gpEngine->mTimer.GetTime()<shieldTime+1.5f)
		return false;

	if(damage<10)//if got shields
	{
		if(numShields>0)
		{
			gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),1.5f);
			switch(numShields)
			{
			case 3:
				gpEngine->mFade.SetSrcColor(0,1,0,0.5f);
				gpEngine->mFade.SetDstColor(0,1,0,0);
				break;
			case 2:
				gpEngine->mFade.SetSrcColor(1,1,0,0.5f);
				gpEngine->mFade.SetDstColor(1,1,0,0);
				break;
			case 1:
				gpEngine->mFade.SetSrcColor(1,0,0,0.5f);
				gpEngine->mFade.SetDstColor(1,0,0,0);
				break;
			}
			rand()%2?(frame=0,Vel.y-=5):(frame=15,Vel.y+=5);
			shieldTime = gpEngine->mTimer.GetTime();
			numShields--;
			return true;
		}
	}
	if(damage>0)
	{
		SND_STOPFORCECHARGE;//make sure we arent charging
		iForceCharge = 0;
		bIsDead = true;
		gpEngine->stmStage.Stop(FSOUND_ALL);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p+Vec2(-64,24),100,0.75f, 90,false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p+Vec2(-64,24),150,0.25f, 90,true);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p+Vec2(+64,48),100,0.75f, 270,false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p+Vec2(+64,48),150,0.25f, 270,true);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p+Vec2(+32,-8),100,0.75f, 130,false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p+Vec2(+32,-8),150,0.25f, 130,true);
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,150,0.5f, 0,false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p,200,0.25f, 0,true);
		
		RandExplosion* rE = new RandExplosion;
		rE->Spawn(mSphere.p);
		gpEngine->GiveEntityToList(rE);	

		if(!gpEngine->bDemo)
		{
			SND_LARGEEXPLODE;
			SND_LARGEEXPLODE2;
			KEY_UP.state = RELEASED;
			KEY_DOWN.state = RELEASED;
			KEY_RIGHT.state = RELEASED;
			KEY_LEFT.state = RELEASED;
			KEY_ATTACK1.state = RELEASED;
			KEY_ATTACK2.state = RELEASED;
			iDelayBeforeRespawn = 300;
		}
		else
			iDelayBeforeRespawn = 110;
	}
	else
		iDelayBeforeRespawn = 110;


	return true;
}
//================================================================================================//
						/*********************
						** Fire the Arc gun **	
						**********************/
//================================================================================================//
bool SortPointsByDistance(Vec2 v1, Vec2 v2)
{
	if(Distance(gpEngine->mPlayer.Pos, v1) < Distance(gpEngine->mPlayer.Pos, v2))
		return true;
	return false;
}
void Player::FireArc()
{
	SND_BIGLASER2;

	if(iForceCharge>=100)//super charged - nice blue flashy
	{
		gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),1.0f);
		gpEngine->mFade.SetSrcColor(0,0.75f,1,0.25f);
		gpEngine->mFade.SetDstColor(0,0,1,0);
	}

	bool bFoundEnts = false;

	float col[4];
	vector<Vec2> points;
	Vec2 u,r;
	UTIL_Misc::MakeVectors(mOrb.rot,u,r);

	//check see if any ents are in fov of the orb
	Line li(mOrb.mSphere.p,mOrb.mSphere.p+r*700 + Vec2(32,32));
	list<Entity*>::iterator iter;
	for(iter = gpEngine->mGameEnts.begin(); iter != gpEngine->mGameEnts.end(); iter++)
	{
//		Line li(mOrb.Pos+r + Vec2(32,32),mOrb.Pos+r*700 + Vec2(32,32));
		if(Collision::PointDistanceToLine((*iter)->mSphere.p,li)<50)
		{
			Sphere sp(1,(*iter)->mSphere.p);
			(*iter)->CheckCollided(sp,(float)iForceCharge/2);
			bFoundEnts = true;
		}
	}
	for(iter = gpEngine->mTopGameEnts.begin(); iter != gpEngine->mTopGameEnts.end(); iter++)
	{
	//	Line li(mOrb.Pos+r + Vec2(32,32),mOrb.Pos+r*700 + Vec2(32,32));
		if(Collision::PointDistanceToLine((*iter)->mSphere.p,li)<50)
		{
			Sphere sp(1,(*iter)->mSphere.p);
			(*iter)->CheckCollided(sp,(float)iForceCharge/2);
			bFoundEnts = true;
		}
	}
	col[0]=0.7f;col[1]=0.95f;col[2]=1;col[3]=1;
	for(int i=0;i<4;i++)
	{
		int numPoints = (rand()%3)+3;
		points.push_back(mOrb.mSphere.p);
		for(int n=0; n<numPoints; n++)
		{
			float bp = (700.0f*(1.0f/(float)numPoints))*n+1;
			rand()%2?
				points.push_back(mOrb.mSphere.p+r*(75+bp)):
				points.push_back(mOrb.mSphere.p+r*(75+bp));
		}
		points.push_back(mOrb.mSphere.p+r*700);
		col[0]=col[1]=(float)(int)(rand()%100)/100.0f;
		if(i<3)
			gpEngine->SpawnBeam(points,5,35,col,0.075f,5+(float)((iForceCharge>100?100:iForceCharge)/20)*0.5f);
		else if(i==3)
		{
			col[0]=0.7f;col[1]=0.95f;col[2]=1;col[3]=1;
			points.clear();
			points.push_back(mOrb.mSphere.p);
			points.push_back(mOrb.mSphere.p+r*150);
			points.push_back(mOrb.mSphere.p+r*250);
			points.push_back(mOrb.mSphere.p+r*350);
			points.push_back(mOrb.mSphere.p+r*450);
			points.push_back(mOrb.mSphere.p+r*550);
			points.push_back(mOrb.mSphere.p+r*700);
			gpEngine->SpawnBeam(points,5,5,col,0.075f,25+(float)((iForceCharge>100?100:iForceCharge)/15));
			gpEngine->SpawnBeam(points,5,5,col,0.115f,15+(float)((iForceCharge>100?100:iForceCharge)/15));
		}
		//	gpEngine->SpawnBeam(points,5,5,col,0.075f,5+(float)((iForceCharge>100?100:iForceCharge)/20));
		points.clear();
	}
}

void Player::LoadFromFile(CFileIO &fIO)
{
}
void Player::WriteToFile(CFileIO &fIO)
{
}
int Player::InWater()
{
	mWaterticks++;
	if(mWaterticks<5)
		return 0;
	mWaterticks=0;
	int x = (int)(mSphere.p.x/32);
	int y = (int)(mSphere.p.y/32);
	if(x<0 || x>399)
		return 0;
	if(y<0 || y>15)
		return 0;
	if(gpEngine->mTilesLayer1[x][y].ID==1018)//water surface
		gpEngine->SpawnSpriteEffect(1,Vec2(mSphere.p.x,(float)(y+1)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);
	if(y>0)
	if(gpEngine->mTilesLayer1[x][y-1].ID==1018)//water surface
		gpEngine->SpawnSpriteEffect(1,Vec2(mSphere.p.x,(float)(y)*32),Vec2(64,32),gpEngine->sprWaterSplash,0.3f,true,NULL,true);

	if(gpEngine->mTilesLayer1[x][y].ID>1018)//in water
		return 1;
	return 0;
}