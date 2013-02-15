#include "Boss.h"
#include "BrainBoss.h"
#include "Engine.h"

Boss::Boss()
{

}
//================================================================================================//
						/*******************
						** Boss spawn **	
						********************/
//================================================================================================//
void Boss::Spawn(Vec2 pos)
{
	IsActive = true;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 500;
	iTakeDamageTicks = 0;
	bJustSpawned = true;
	bComingToLife = false;
	Eye1Frame = 2;
	Eye2Frame = 5;
	Eye3Frame = 3;
	Eye4Frame = 5;
	Eye5Frame = 6;
	bEye1Blink = false;
	bEye2Blink = false;
	bEye3Blink = false;
	bEye4Blink = false;
	bEye5Blink = false;

	Tube[0][0]=Vec2(Pos.x-50,-150);
	Tube[0][1]=Vec2(Pos.x+100,0);
	Tube[0][2]=Vec2(Pos.x+140,180);
	Tube[1][0]=Vec2(Pos.x,-150);
	Tube[1][1]=Vec2(Pos.x+150,0);
	Tube[1][2]=Vec2(Pos.x+190,180);
	Tube[2][0]=Vec2(Pos.x+50,-150);
	Tube[2][1]=Vec2(Pos.x+200,0);
	Tube[2][2]=Vec2(Pos.x+240,180);
	Tube[3][0]=Vec2(Pos.x+120,-150);
	Tube[3][1]=Vec2(Pos.x+270,0);
	Tube[3][2]=Vec2(Pos.x+310,180);
	tubeSineWave=0;

	iAttackWaveTicks = 0;
	iAttackPattern = 0;
	fAttackTicks = 0;
	fAttackRatio =1;

	fChargeFrame =0;
	fTopCharge =0;
	fLowCharge =0;

	fTopGunFireTime = gpEngine->mTimer.GetTime()-1;
	fLowGunFireTime = gpEngine->mTimer.GetTime()-1;

	//collision boundaries
	mSphere = Sphere(30,Vec2(Pos.x+140,270));
	mBounds[0] = Sphere(35,Vec2((Pos.x-345)+23,85));
	mBounds[1] = Sphere(35,Vec2((Pos.x-345)+23,395));
	mBounds[2] = Sphere(55,Vec2((Pos.x-345)+33,30));
	mBounds[3] = Sphere(55,Vec2((Pos.x-345)+33,450));
	mBounds[4] = Sphere(178,Vec2(Pos.x+256,0));
	mBounds[5] = Sphere(128,Vec2(Pos.x+230,380));
	mBounds[6] = Sphere(50,Vec2(Pos.x+140,160));
	mBounds[7] = Sphere(55,Vec2(Pos.x+100,450));
	mBounds[8] = Sphere(40,Vec2(Pos.x+160,210));

	bIsDead = false;
}
//================================================================================================//
						/*******************
						** Boss Clone **	
						********************/
//================================================================================================//
Boss* Boss::Clone()const
{
	return new Boss(*this);
}
//================================================================================================//
						/********************
						** Boss update **	
						*********************/
//================================================================================================//
void Boss::Update()
{
	iTakeDamageTicks--;
	oPos = Pos;
	tubeSineWave+=0.01f;
	
	//death sequence
	if(bIsDead)
	{
		nextExplosionTick++;
		iDeathTicks++;
		if(iDeathTicks==800)
		{
			gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),2.5f);
			gpEngine->mFade.SetSrcColor(1,1,1,1);
			gpEngine->mFade.SetDstColor(1,1,1,0);
			IsActive = false;
			BrainBoss* brain = new BrainBoss;
			brain->Spawn(Vec2(Pos.x+60,240-128));
			gpEngine->GiveEntityToList((Entity*)brain);
			return;
		}
		if(nextExplosionTick==5 || nextExplosionTick==15 || nextExplosionTick==25)
		{
			int RotAngle = rand()%360;
			float Y = (float)(rand()%480);
			float X = (float)(rand()%200);
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Vec2(Pos.x+100+X,Y),90,0.75f, (float)(RotAngle),false);
			SND_SMALLEXPLODE;
		}
		if(nextExplosionTick==10)
		{
			int RotAngle = rand()%360;
			float Y = (float)(rand()%480);
			float X = (float)(rand()%200);
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Vec2(Pos.x+100+X,Y),120,0.75f, (float)(RotAngle),false);
			gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Vec2(Pos.x+100+X,Y),120,0.5f, (float)(RotAngle),true);
			SND_MEDEXPLODE;
		}
		if(nextExplosionTick==30)
		{
			int RotAngle = rand()%360;
			float Y = (float)(rand()%480);
			float X = (float)(rand()%200);
			gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Vec2(Pos.x+100+X,Y),180,0.75f, (float)(RotAngle),false);
			gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Vec2(Pos.x+100+X,Y),180,0.5f, (float)(RotAngle),true);
			SND_LARGEEXPLODE;
		}
		if(nextExplosionTick>30)
			nextExplosionTick = 0;
		return;
	}

	//makes sure player gets pushed by air
	if(Collision::SphereSphereOverlap(Sphere(30,Vec2((Pos.x-345)+23,150)),gpEngine->mPlayer.mSphere))
	{
		gpEngine->mPlayer.Vel.y+=0.1f;
	}
	if(Collision::SphereSphereOverlap(Sphere(30,Vec2((Pos.x-345)+23,330)),gpEngine->mPlayer.mSphere))
	{
		gpEngine->mPlayer.Vel.y-=0.1f;
	}
	if(Collision::SphereSphereOverlap(Sphere(30,Vec2(Pos.x+30,385)),gpEngine->mPlayer.mSphere))
	{
		gpEngine->mPlayer.Vel-=Vec2(0.1f,0.1f);
	}

	//spawn sequence
	if(bJustSpawned)
	{
		if(gpEngine->Scroll > Pos.x-384)
		{
			gpEngine->Scroll = gpEngine->oScroll = Pos.x-384;
			gpEngine->bPauseScrolling = true;
			bJustSpawned = false;
			bComingToLife = true;
			if(!gpEngine->bDemo)
			{
				gpEngine->stmStage.Load("Data/Sound/Boss.ogg",true);
				//gpEngine->stmStage.Seek(0,30);
				gpEngine->stmStage.Play(30);
			}
		}
		return;
	}

	if(gpEngine->bRecording)
		return;


	iAttackWaveTicks++;
	//coming to life
	if(bComingToLife)
	{
		if(iAttackWaveTicks>250)
		{
			Eye2Frame>0?	Eye2Frame-=0.1f:	Eye2Frame=0;
			Eye3Frame>0?	Eye3Frame-=0.1f:	Eye3Frame=0;
			Eye4Frame>0?	Eye4Frame-=0.1f:	Eye4Frame=0;
			Eye5Frame>0?	Eye5Frame-=0.1f:	Eye5Frame=0;
			if(Eye2Frame==0 && Eye3Frame==0 && Eye4Frame==0 && Eye5Frame==0)
			{
				Eye1BlinkTime = gpEngine->mTimer.GetTime();
				Eye2BlinkTime = gpEngine->mTimer.GetTime();
				Eye3BlinkTime = gpEngine->mTimer.GetTime();
				Eye4BlinkTime = gpEngine->mTimer.GetTime();
				Eye5BlinkTime = gpEngine->mTimer.GetTime();

				iAttackWaveTicks=0;
				bComingToLife = false;
				fTopGunRateOfFire = gpEngine->mTimer.GetTime();
				fLowGunRateOfFire = gpEngine->mTimer.GetTime();
			}
		}
		return;
	}

	//animate eyes
	if(gpEngine->mTimer.GetTime()>Eye2BlinkTime+3)
	{
		bEye2Blink = true;
		Eye2BlinkTime = gpEngine->mTimer.GetTime()+4;
	}
	if(gpEngine->mTimer.GetTime()>Eye3BlinkTime+4)
	{
		bEye3Blink = true;
		Eye3BlinkTime = gpEngine->mTimer.GetTime()+3;
	}
	if(gpEngine->mTimer.GetTime()>Eye4BlinkTime+2)
	{
		bEye4Blink = true;
		Eye4BlinkTime = gpEngine->mTimer.GetTime()+5;
	}
	if(gpEngine->mTimer.GetTime()>Eye5BlinkTime+2.5f)
	{
		bEye5Blink = true;
		Eye5BlinkTime = gpEngine->mTimer.GetTime()+3;
	}


	bEye2Blink?
		Eye2Frame<5? Eye2Frame+=0.3f: (Eye2Frame=5,bEye2Blink=false):
		Eye2Frame>0? Eye2Frame-=0.3f: (Eye2Frame=0);
	bEye3Blink?
		Eye3Frame<3? Eye3Frame+=0.1f: (Eye3Frame=3,bEye3Blink=false):
		Eye3Frame>0? Eye3Frame-=0.3f: (Eye3Frame=0);
	bEye4Blink?
		Eye4Frame<5? Eye4Frame+=0.5f: (Eye4Frame=5,bEye4Blink=false):
		Eye4Frame>0? Eye4Frame-=0.3f: (Eye4Frame=0);
	bEye5Blink?
		Eye5Frame<6? Eye5Frame+=0.4f: (Eye5Frame=6,bEye5Blink=false):
		Eye5Frame>0? Eye5Frame-=0.6f: (Eye5Frame=0);

		//update tubes
	Tube[0][1]+=Vec2(sin(tubeSineWave*5),cos(tubeSineWave*3));
	Tube[1][1]+=Vec2(sin(tubeSineWave*2),cos(tubeSineWave*4));
	Tube[2][1]+=Vec2(sin(tubeSineWave*4),cos(tubeSineWave*1));
	Tube[3][1]+=Vec2(sin(tubeSineWave*3),cos(tubeSineWave*2));

	//loop around the entity spawning waves
	if(iAttackWaveTicks>550)
	{

		fChargeFrame = 0;
		iAttackWaveTicks = 0;
		iAttackPattern<3?iAttackPattern++:iAttackPattern=0;
		if(iAttackPattern==1)
		{
			Serpent* pSerpent = mSerpentAttackPattern1->Clone();
			pSerpent->RateofFire = 0;
			pSerpent->Spawn(Pos-Vec2(400,0));
			gpEngine->GiveEntityToList(pSerpent);
		}
		if(iAttackPattern==2)
		{
			Serpent* pSerpent = mSerpentAttackPattern2->Clone();
			pSerpent->RateofFire = 0;
			pSerpent->Spawn(Vec2(Pos.x-400,480));
			gpEngine->GiveEntityToList(pSerpent);
		}
		if(iAttackPattern==3)
		{
			Serpent* pSerpent = mSerpentAttackPattern3->Clone();
			pSerpent->Spawn(Vec2(Pos.x+70,480));
			pSerpent->RateofFire = 0;
			pSerpent->fStartRotation = pSerpent->fRotation = -135;
			vector<Serpent>::iterator n;
			for(n=pSerpent->mSections.begin(); n<pSerpent->mSections.end(); n++)
			{
				(*n).fStartRotation = -135;
			}
			gpEngine->GiveEntityToList(pSerpent);
		}
	}


	if(fAttackTicks>410)
	{
		fChargeFrame<31? fChargeFrame+=0.5f: fChargeFrame-=31;
	}

	//open the main hit spot
	if(fAttackTicks>400)
		Eye1Frame>0?Eye1Frame-=0.1f:Eye1Frame=0;
	if(fAttackTicks>50 && fAttackTicks<400)
		Eye1Frame<2?Eye1Frame+=0.1f:Eye1Frame=2;

	//fire top gun
	if(fAttackTicks>=90 && fAttackTicks<=100)
	{
		if(fTopGunFireTime<gpEngine->mTimer.GetTime())
		{
			targetPos = gpEngine->mPlayer.mSphere.p;
			fTopGunFireTime = gpEngine->mTimer.GetTime()+1;
		}
	}
	//fire low gun
	if(fAttackTicks>=390 && fAttackTicks<=400)
	{
		fLowGunFireTime = gpEngine->mTimer.GetTime()+1;
	}

	if(gpEngine->mTimer.GetTime()<fTopGunFireTime)
	{
		if(gpEngine->mTimer.GetTime()>fTopGunRateOfFire)
		{
			fTopGunRateOfFire = gpEngine->mTimer.GetTime()+0.1f;
			Bullet b;
			b.pSpawn = Spawn_PowerBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_BossPowerBullet;
			b.pCollide = Collide_PowerBullet;
			Vec2 p(Pos.x+50,140);
			Vec2 v = FireAtTarget(p,targetPos,7.5f);
			(*b.pSpawn)(b,p,v);
			Line l1(targetPos,p);
			Line l2(p,Vec2(p.x,p.y+Distance(targetPos,p)));
			if(p.x>targetPos.x)
				b.rot = -(RAD2DEG(l1.Angle(l2))-90);
			else
				b.rot = (RAD2DEG(l1.Angle(l2))+90);
			gpEngine->SpawnBullet(b,false);
			SND_LASER4;
		}
	}

	if(gpEngine->mTimer.GetTime()<fLowGunFireTime)
	{
		if(gpEngine->mTimer.GetTime()>fLowGunRateOfFire)
		{
			fLowGunRateOfFire = gpEngine->mTimer.GetTime()+((float)0.15f*gpEngine->fGameDifficulty);
			Bullet b;
			b.pSpawn = Spawn_BasicBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_BasicBullet;
			b.pCollide = Collide_BasicBullet;
			Vec2 v = FireAtTarget(Vec2(Pos.x+50,340),gpEngine->mPlayer.mSphere.p,1.5f);
			(*b.pSpawn)(b,Vec2(Pos.x+50,340),v*4);
			gpEngine->SpawnBullet(b,false);
			SND_LASER2;
		}
	}

	//update chargeup animations
	if(gpEngine->mTimer.GetTime()<fTopGunFireTime || fAttackTicks>50 && fAttackTicks<150)
		fTopCharge<31?fTopCharge+=0.5f:fTopCharge-=31;
	if(gpEngine->mTimer.GetTime()<fLowGunFireTime || fAttackTicks>300 && fAttackTicks<400)
		fLowCharge<31?fLowCharge+=0.5f:fLowCharge-=31;

	//loop the attack patterns around
	if(fAttackTicks>500)
	{
		fAttackRatio+=0.25f;
		if(gpEngine->fGameDifficulty==1)//hard
		{
			if(fAttackRatio>6)//TODO base on difficulty settings
				fAttackRatio=6;
		}
		else if(gpEngine->fGameDifficulty==3)//easy
		{
			if(fAttackRatio>2)
				fAttackRatio=2;
		}
		else
		{
			if(fAttackRatio>4)
				fAttackRatio=4;
		}
		fAttackTicks=0;
		fTopCharge=0;
		fLowCharge=0;
		Bullet b;
		b.pSpawn = Spawn_BossBullet;
		b.pUpdate = Update_BossBullet;
		b.pRender = Render_BossBullet;
		b.pCollide = Collide_PowerBullet;
		(*b.pSpawn)(b,Vec2(Pos.x+100,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+150,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+200,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+250,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+300,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+350,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+400,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+450,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+500,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		(*b.pSpawn)(b,Vec2(Pos.x+550,208),Vec2(-18,0));
		b.frame = (float)(rand()%15);
		gpEngine->SpawnBullet(b,false);
		SND_BIGLASER1;
	}

	fAttackTicks += fAttackRatio;
}
//================================================================================================//
						/******************
						** Boss draw **	
						*******************/
//================================================================================================//
void Boss::Draw(const float interp)
{
	float deathFade;
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
	{
		if(!bComingToLife && !bJustSpawned)
			glColor4f(1,1,1,1);
		else
		{
			if(iAttackWaveTicks<250)
			{
				float color = (float)(iAttackWaveTicks/250.0f);
				glColor4f(color+0.3f,color,color,1);
			}
		}

	}
	if(bIsDead)
	{
		if(iDeathTicks>400)
		{
			deathFade = 1.0f-(float)((iDeathTicks-400)/400.0f);
			if(deathFade<0)
				deathFade=0;
			glColor4f(deathFade,deathFade,deathFade,1);
		}
	}
	//Render Tubes
	for(int t=0;t<4;t++)
	{
		Vec2 point,point2;

		for(int pass=0;pass<3;pass++)
		{
			switch(pass)
			{
			case 0:
			UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossTubeFluid);
				break;
			case 1:
			UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossTube);
				break;
			case 2:
			UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE);
			UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossTubeSpec);
				break;
			}
			float a=0;
			float b=1;
			float step=0.1f;

			bool texes = false;
			glBegin(GL_TRIANGLE_STRIP);
			for(int c=0;c<10;c++)
			{
				a+=step;
				b=1-a;
				point = Tube[t][0] *a*a + Tube[t][1] *2*a*b + Tube[t][2] *b*b;
				float a2=a-step;
				float b2=1-a2;
				point = Tube[t][0] *a2*a2 + Tube[t][1] *2*a2*b2 + Tube[t][2] *b2*b2;
				Line l(point,point2);
				float sc = gpEngine->mTimer.GetTime()*0.25f;
				if(!texes)
				{
					glTexCoord2f(0,(pass==0?sc:0));
					glVertex2fv((point-l.n*55).v);
					glTexCoord2f(1,(pass==0?sc:0));
					glVertex2fv((point+l.n*55).v);
				}
				else
				{
					glTexCoord2f(0,1+(pass==0?sc:0));
					glVertex2fv((point-l.n*55).v);
					glTexCoord2f(1,1+(pass==0?sc:0));
					glVertex2fv((point+l.n*55).v);
				}
				texes = !texes;
			}
			glEnd();
		}
	}

	//Render Eyes
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderSprite(gpEngine->sprBossEye1,(int)Eye1Frame,Pos.x+92,235,64,64);
	RenderSprite(gpEngine->sprBossEye2,(int)Eye2Frame,Pos.x+151,162,64,64);
	RenderSprite(gpEngine->sprBossEye3,(int)Eye3Frame,Pos.x+136,154,32,32);
	RenderSprite(gpEngine->sprBossEye4,(int)Eye4Frame,Pos.x+192,227,64,64);
	RenderSprite(gpEngine->sprBossEye5,(int)Eye5Frame,Pos.x+176,281,32,32);
	//Render Boss
	UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBoss);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(Pos.x,0);
	glTexCoord2f(1,0);
	glVertex2f(Pos.x+256,0);
	glTexCoord2f(1,1);
	glVertex2f(Pos.x+256,512);
	glTexCoord2f(0,1);
	glVertex2f(Pos.x,512);
	glEnd();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//render boss pushing out air
	float rot = -135;
	Vec2 up,right,p;
	if(!bIsDead)
	{
		UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossPush);
		UTIL_GL::SetBlend(GL_ONE, GL_ONE);
		glColor4f(1,0,0,1);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0-tubeSineWave);
		glVertex2f(Pos.x-345,120);
		glTexCoord2f(1,0-tubeSineWave);
		glVertex2f((Pos.x-345)+46,120);
		glColor4f(0,0,0,1);
		glTexCoord2f(1,1-tubeSineWave);
		glVertex2f((Pos.x-345)+46,182);
		glTexCoord2f(0,1-tubeSineWave);
		glVertex2f((Pos.x-345),182);
		glEnd();

		glColor4f(0,0,0,1);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0+tubeSineWave);
		glVertex2f(Pos.x-345,298);
		glTexCoord2f(1,0+tubeSineWave);
		glVertex2f((Pos.x-345)+46,298);
		glColor4f(1,0,0,1);
		glTexCoord2f(1,1+tubeSineWave);
		glVertex2f((Pos.x-345)+46,360);
		glTexCoord2f(0,1+tubeSineWave);
		glVertex2f((Pos.x-345),360);
		glEnd();

		p=Vec2(Pos.x+30,385);
		UTIL_Misc::MakeVectors(rot,up,right);
		glBegin(GL_QUADS);
			glColor4f(1,0,0,1);
			glTexCoord2f(0,1+tubeSineWave);
			glVertex2fv((p - right * 23 - up * 32).v);
			glTexCoord2f(1,1+tubeSineWave);
			glVertex2fv((p + right * 23 - up * 32).v);
			glColor4f(0,0,0,1);
			glTexCoord2f(1,0+tubeSineWave);
			glVertex2fv((p + right * 23 + up * 32).v);
			glTexCoord2f(0,0+tubeSineWave);
			glVertex2fv((p - right * 23 + up * 32).v);
		glEnd();
	}
	glColor4f(1,1,1,1);

	
	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
	{
		if(!bComingToLife && !bJustSpawned)
			glColor4f(1,1,1,1);
		else
		{
			if(iAttackWaveTicks<250)
			{
				float color = (float)(iAttackWaveTicks/250.0f);
				glColor4f(color+0.3f,color,color,1);
			}
		}
	}
	if(bIsDead)
		if(iDeathTicks>400)
			glColor4f(deathFade,deathFade,deathFade,1);

	//arms
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgBossArm);
	UTIL_GL::GL2D::DrawTextureQuad(Pos.x-440,0,(Pos.x-440)+256,128);
	UTIL_GL::GL2D::DrawTextureQuad(Pos.x-440,480,(Pos.x-440)+256,480-128);
	p=Vec2(Pos.x+90,440);
	UTIL_Misc::MakeVectors(rot,up,right);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2fv((p - right * 128 - up * 128).v);
		glTexCoord2f(1,1);
		glVertex2fv((p + right * 128 - up * 128).v);
		glTexCoord2f(1,0);
		glVertex2fv((p + right * 128 + up * 64).v);
		glTexCoord2f(0,0);
		glVertex2fv((p - right * 128 + up * 64).v);
	glEnd();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//render charge
	if(!bIsDead)
	{
		if(!bJustSpawned && !bComingToLife)
		{
			if(fAttackTicks>450)
			{
				UTIL_GL::SetBlend(GL_ONE, GL_ONE);
				float overlap = fChargeFrame+16;
				if(overlap>31)overlap-=31;
				glColor4f(1,1,0,1);
				RenderSprite(gpEngine->sprForceCharge,(int)fChargeFrame,Pos.x+160,204,-256,128);
				RenderSprite(gpEngine->sprForceCharge,(int)overlap,Pos.x+160,204,-256,128);
				glColor4f(1,1,1,1);
			}
			if(gpEngine->mTimer.GetTime()<fTopGunFireTime || fAttackTicks>50 && fAttackTicks<150)
			{
				UTIL_GL::SetBlend(GL_ONE, GL_ONE);
				float overlap = fTopCharge+16;
				if(overlap>31)overlap-=31;
				glColor4f(0.1f,1,1,1);
				RenderSprite(gpEngine->sprBossCharge,(int)fTopCharge,Pos.x,90,128,128);
				RenderSprite(gpEngine->sprBossCharge,(int)overlap,Pos.x,90,128,128);
				glColor4f(1,1,1,1);
			}
			if(gpEngine->mTimer.GetTime()<fLowGunFireTime || fAttackTicks>300 && fAttackTicks<400)
			{
				UTIL_GL::SetBlend(GL_ONE, GL_ONE);
				float overlap = fLowCharge+16;
				if(overlap>31)overlap-=31;
				glColor4f(1,0,0,1);
				RenderSprite(gpEngine->sprBossCharge,(int)fLowCharge,Pos.x+15,290,128,128);
				RenderSprite(gpEngine->sprBossCharge,(int)overlap,Pos.x+15,290,128,128);
				glColor4f(1,1,1,1);
			}
		}
	}


/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
//	GeoDraw2D::DrawSphere(Sphere(30,Vec2((Pos.x-345)+23,150)),15);
//	GeoDraw2D::DrawSphere(Sphere(30,Vec2((Pos.x-345)+23,330)),15);
//	GeoDraw2D::DrawSphere(Sphere(30,Vec2(Pos.x+30,385)),15);
	for(int n=0;n<9;n++)
		GeoDraw2D::DrawSphere(mBounds[n], 15);
	GeoDraw2D::DrawSphere(mSphere, 15);//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void Boss::NeedsToBeRemoved()
{
}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool Boss::CheckCollided(Sphere s, float damage)
{
	if(!IsActive || bIsDead)
		return false;


	if(Collision::SphereSphereOverlap(s,mSphere))
	{
		if(Eye1Frame<2 && !bJustSpawned && !bComingToLife)
		{
			fLife -= damage;
			if(fLife<=0)
			{
				bIsDead = true;
				int RotAngle = rand()%360;
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,Vec2(Pos.x+100,240),220,0.75f, (float)(RotAngle),false);
				gpEngine->SpawnExplosion(gpEngine->sprExplosion2,Vec2(Pos.x+100,240),220,0.5f, (float)(RotAngle),true);
				SND_LARGEEXPLODE2;
				gpEngine->mPlayer.iScore+=1500;
				gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),1.5f);
				gpEngine->mFade.SetSrcColor(1,0,0,1);
				gpEngine->mFade.SetDstColor(1,0,0,0);
				nextExplosionTick = 0;
				iDeathTicks = 0;
				list<Entity*>::iterator i;
				for(i=gpEngine->mGameEnts.begin();i!=gpEngine->mGameEnts.end();i++)
				{
					(*i)->CheckCollided((*i)->mSphere,100000);//kill all ents
				}
			}
			iTakeDamageTicks = 3;
		}
		return true;
	}

	for(int n=0;n<9;n++)
	{
		if(Collision::SphereSphereOverlap(s,mBounds[n]))
			return true;
	}

	return false;
}

void Boss::LoadFromFile(CFileIO &fIO)
{
	mSerpentAttackPattern1 = new Serpent;
	mSerpentAttackPattern2 = new Serpent;
	mSerpentAttackPattern3 = new Serpent;
	mSerpentAttackPattern1->LoadFromFile(fIO);
	mSerpentAttackPattern2->LoadFromFile(fIO);
	mSerpentAttackPattern3->LoadFromFile(fIO);
}
void Boss::WriteToFile(CFileIO &fIO)
{
	mSerpentAttackPattern1->WriteToFile(fIO);
	mSerpentAttackPattern2->WriteToFile(fIO);
	mSerpentAttackPattern3->WriteToFile(fIO);
}
int Boss::InWater()
{
	return 0;
}