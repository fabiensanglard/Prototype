#include "MidBoss.h"
#include "Engine.h"

MidBoss::MidBoss()
{
}
//================================================================================================//
						/*******************
						** MidBoss spawn **	
						********************/
//================================================================================================//
void MidBoss::Spawn(Vec2 pos)
{
	IsActive = true;
	pos.y-=16;
	oPos = Pos = pos;
	frame = 0;
	fStartLife = fLife = 300;
	fGunLife = 100;
	mSphere = Sphere(25,pos+Vec2(70,64));
	mBounds[0] = Sphere(13,Pos+Vec2(30,22));
	mBounds[1] = Sphere(13,Pos+Vec2(30,108));
	mBounds[2] = Sphere(9,Pos+Vec2(50,30));
	mBounds[3] = Sphere(9,Pos+Vec2(50,98));
	mBounds[4] = Sphere(9,Pos+Vec2(70,30));
	mBounds[5] = Sphere(9,Pos+Vec2(70,98));
	mSphereGun = Sphere(18,Pos+Vec2(32,64));
	iTakeDamageTicks = 0;
	iGunDamageTicks = 0;
	bJustSpawned = true;
	CoreColor[0]=CoreColor[2]=0;
	CoreColor[1]=1;

	iAttackPattern = 0;
	fAttackPatternTicks=500;
	iTicksAlongMotion = 0;
	mFoot[0] = mFootPatterns1[0] = pos-Vec2(32,48);//front legs
	mFoot[1] = mFootPatterns1[1] = pos-Vec2(32,-(128+48));//front legs
	mFoot[2] = mFootPatterns1[2] = pos-Vec2(-192,48);
	mFoot[3] = mFootPatterns1[3] = pos-Vec2(-192,-(128+48));
	mFootPatterns2[0] = pos-Vec2(192,144);
	mFootPatterns2[1] = pos-Vec2(192,-(128+144));
	mFootPatterns2[2] = pos-Vec2(32,48);
	mFootPatterns2[3] = pos-Vec2(32,-(128+48));
	mFootPatterns3[0] = pos-Vec2(420,176);
	mFootPatterns3[1] = pos-Vec2(420,-(128+176));
	mFootPatterns3[2] = pos-Vec2(192,144);
	mFootPatterns3[3] = pos-Vec2(192,-(128+144));

	mFootJoint[0] = Pos+Vec2(25,12);
	mFootJoint[1] = Pos+Vec2(25,128-12);
	mFootJoint[2] = Pos+Vec2(115,12);
	mFootJoint[3] = Pos+Vec2(115,128-12);
	mFootControl[0] = Pos+Vec2(-25,0);
	mFootControl[1] = Pos+Vec2(-25,128);
	mFootControl[2] = Pos+Vec2(145,0);
	mFootControl[3] = Pos+Vec2(145,128);

	for(int n=0;n<4;n++)
	{
		float a=1,b=0,step=0.1f;
		Vec2 point;
		for(int j=0;j<10;j++)
		{
			a-=step;
			b=1-a;
			point.x = mFoot[n].x*a*a + mFootControl[n].x*2*a*b + mFootJoint[n].x*b*b;
			point.y = mFoot[n].y*a*a + mFootControl[n].y*2*a*b + mFootJoint[n].y*b*b;
			mFootSphere[n][j] = Sphere(15,point);
		}
	}

	bIsDead = false;
	bBigGun = true;//until we get it drawn
	footRateOfFire = gpEngine->mTimer.GetTime();
	CoreRateOfFire = gpEngine->mTimer.GetTime();
	midActionFire = 0;
}
//================================================================================================//
						/*******************
						** MidBoss Clone **	
						********************/
//================================================================================================//
MidBoss* MidBoss::Clone()const
{
	return new MidBoss(*this);
}
//================================================================================================//
						/********************
						** MidBoss update **	
						*********************/
//================================================================================================//
void MidBoss::Update()
{
	iTakeDamageTicks--;
	iGunDamageTicks--;
	oPos = Pos;

	//death sequence
	if(bIsDead)
	{
		fAttackPatternTicks++;
		if(fAttackPatternTicks<100)
		{
			float a = (float)(fAttackPatternTicks/100.0f);
			float b = 1-a;
			mFootJoint[0] = mFoot[0]*a*a + mFootControl[0]*2*a*b + mFootJoint[0]*b*b;
			mFootJoint[1] = mFoot[1]*a*a + mFootControl[1]*2*a*b + mFootJoint[1]*b*b;
			mFootJoint[2] = mFoot[2]*a*a + mFootControl[2]*2*a*b + mFootJoint[2]*b*b;
			mFootJoint[3] = mFoot[3]*a*a + mFootControl[3]*2*a*b + mFootJoint[3]*b*b;
			if(gpEngine->mTimer.GetTime()>CoreRateOfFire+0.1f)
			{
				CoreRateOfFire=gpEngine->mTimer.GetTime();
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mFootJoint[0],120,0.75f, (float)(rand()%360),false);
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mFootJoint[1],120,0.75f, (float)(rand()%360),false);
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mFootJoint[2],120,0.75f, (float)(rand()%360),false);
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mFootJoint[3],120,0.75f, (float)(rand()%360),false);
				SND_MEDEXPLODE;
			}
			if(gpEngine->mTimer.GetTime()>footRateOfFire+0.4f)
			{
				footRateOfFire = gpEngine->mTimer.GetTime();
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,250,0.5f, (float)(rand()%360),false);
				SND_LARGEEXPLODE;
			}
		}
		else
			IsActive = false;
		return;
	}

	if(iAttackPattern==1 && fAttackPatternTicks>=150)
	{
		Pos.y+=sin((float)fAttackPatternTicks*0.25f)*5;
	}
	if(iAttackPattern==2 && fAttackPatternTicks>=150)
	{
		Pos.x += sin((float)fAttackPatternTicks*0.125f)*5;
		Pos.y += cos((float)fAttackPatternTicks*0.125f)*5;
	}
	else
		Pos.x = mFoot[0].x+((mFoot[0].x-mFoot[2].x)*0.15f);
	if(Pos!=oPos)
	{
		mFootJoint[0] = Pos+Vec2(25,12);
		mFootJoint[1] = Pos+Vec2(25,128-12);
		mFootJoint[2] = Pos+Vec2(115,12);
		mFootJoint[3] = Pos+Vec2(115,128-12);
		mFootControl[0] = Pos+Vec2(-25,0);
		mFootControl[1] = Pos+Vec2(-25,128);
		mFootControl[2] = Pos+Vec2(145,0);
		mFootControl[3] = Pos+Vec2(145,128);
		//update collision spheres
		mBounds[0].p = Pos+Vec2(30,22);
		mBounds[1].p = Pos+Vec2(30,108);
		mBounds[2].p = Pos+Vec2(50,30);
		mBounds[3].p = Pos+Vec2(50,98);
		mBounds[4].p = Pos+Vec2(70,30);
		mBounds[5].p = Pos+Vec2(70,98);

		if(bBigGun)
		{
			mSphereGun.p = Pos+Vec2(32,64);
		}
	}
	//update arm sphere positions
	for(int n=0;n<4;n++)
	{
		float a=1,b=0,step=0.1f;
		Vec2 point;
		for(int j=0;j<10;j++)
		{
			a-=step;
			b=1-a;
			point.x = mFoot[n].x*a*a + mFootControl[n].x*2*a*b + mFootJoint[n].x*b*b;
			point.y = mFoot[n].y*a*a + mFootControl[n].y*2*a*b + mFootJoint[n].y*b*b;
			mFootSphere[n][j].p = point;
		}
	}
	if(bJustSpawned)
	{
		if(gpEngine->Scroll > Pos.x-520)
		{
			gpEngine->Scroll = gpEngine->oScroll = Pos.x-520;
			gpEngine->bPauseScrolling = true;
			bJustSpawned = false;
		}
		return;
	}

	float speed = 1.5f-(fLife/200.0f);
	fAttackPatternTicks+=(1+speed);
	if(fAttackPatternTicks>300)
	{
		iTicksAlongMotion = 0;
		fAttackPatternTicks=0;
		iAttackPattern<2?iAttackPattern++:iAttackPattern=0;

		if(iAttackPattern==0)
		{
			mFootMotionSpline[0][0] = mFootPatterns1[0];
			mFootMotionSpline[0][1] = Pos+Vec2(-25,64);
			mFootMotionSpline[0][2] = mFootPatterns3[0];
			mFootMotionSpline[1][0] = mFootPatterns1[1];
			mFootMotionSpline[1][1] = Pos+Vec2(-25,64);
			mFootMotionSpline[1][2] = mFootPatterns3[1];
			mFootMotionSpline[2][0] = mFootPatterns1[2];
			mFootMotionSpline[2][1] = Pos+Vec2(145,64);
			mFootMotionSpline[2][2] = mFootPatterns3[2];
			mFootMotionSpline[3][0] = mFootPatterns1[3];
			mFootMotionSpline[3][1] = Pos+Vec2(145,64);
			mFootMotionSpline[3][2] = mFootPatterns3[3];
		}
		if(iAttackPattern==1)
		{
			mFootMotionSpline[0][0] = mFootPatterns2[0];
			mFootMotionSpline[0][1] = Pos+Vec2(-25,64);
			mFootMotionSpline[0][2] = mFootPatterns1[0];
			mFootMotionSpline[1][0] = mFootPatterns2[1];
			mFootMotionSpline[1][1] = Pos+Vec2(-25,64);
			mFootMotionSpline[1][2] = mFootPatterns1[1];
			mFootMotionSpline[2][0] = mFootPatterns2[2];
			mFootMotionSpline[2][1] = Pos+Vec2(145,64);
			mFootMotionSpline[2][2] = mFootPatterns1[2];
			mFootMotionSpline[3][0] = mFootPatterns2[3];
			mFootMotionSpline[3][1] = Pos+Vec2(145,64);
			mFootMotionSpline[3][2] = mFootPatterns1[3];
			midActionFire=0;
		}
		if(iAttackPattern==2)
		{
			mFootMotionSpline[0][0] = mFootPatterns3[0];
			mFootMotionSpline[0][1] = Pos+Vec2(-25,64);
			mFootMotionSpline[0][2] = mFootPatterns2[0];
			mFootMotionSpline[1][0] = mFootPatterns3[1];
			mFootMotionSpline[1][1] = Pos+Vec2(-25,64);
			mFootMotionSpline[1][2] = mFootPatterns2[1];
			mFootMotionSpline[2][0] = mFootPatterns3[2];
			mFootMotionSpline[2][1] = Pos+Vec2(145,64);
			mFootMotionSpline[2][2] = mFootPatterns2[2];
			mFootMotionSpline[3][0] = mFootPatterns3[3];
			mFootMotionSpline[3][1] = Pos+Vec2(145,64);
			mFootMotionSpline[3][2] = mFootPatterns2[3];
		}

	}
	speed = 3-(fLife/100.0f);
	iTicksAlongMotion+=(1+speed);
	if(iTicksAlongMotion<100)
	{
		float a = (float)(iTicksAlongMotion/100.0f);
		float b = 1-a;
		mFoot[0] = mFootMotionSpline[0][0]*a*a + mFootMotionSpline[0][1]*2*a*b + mFootMotionSpline[0][2]*b*b;
		mFoot[1] = mFootMotionSpline[1][0]*a*a + mFootMotionSpline[1][1]*2*a*b + mFootMotionSpline[1][2]*b*b;
	}
	if(iTicksAlongMotion<150 && iTicksAlongMotion>50)
	{
		float a = (float)((iTicksAlongMotion-50)/100.0f);
		float b = 1-a;
		mFoot[2] = mFootMotionSpline[2][0]*a*a + mFootMotionSpline[2][1]*2*a*b + mFootMotionSpline[2][2]*b*b;
		mFoot[3] = mFootMotionSpline[3][0]*a*a + mFootMotionSpline[3][1]*2*a*b + mFootMotionSpline[3][2]*b*b;
	}

	//feet shoot on way back to spawn position
	if(iAttackPattern==0 && fAttackPatternTicks<=100)
	{
		if(gpEngine->mTimer.GetTime()>footRateOfFire+0.1f)
		{
			if(mFoot[2]!=mFootPatterns1[2])
			if(mFoot[3]!=mFootPatterns1[3])
			{
				footRateOfFire = gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_BasicBullet;
				b.pUpdate = Update_BasicBullet;
				b.pRender = Render_BasicBullet;
				b.pCollide = Collide_BasicBullet;
				Vec2 v = FireAtTarget(mFoot[2]+Vec2(0,32),gpEngine->mPlayer.mSphere.p,4.5f);
				(*b.pSpawn)(b,mFoot[2]+Vec2(0,32),v);
				gpEngine->SpawnBullet(b,false);
				v = FireAtTarget(mFoot[3]-Vec2(0,48),gpEngine->mPlayer.mSphere.p,4.5f);
				(*b.pSpawn)(b,mFoot[3]-Vec2(0,48),v);
				gpEngine->SpawnBullet(b,false);
				SND_LASER2;
			}
		}
	}
	if(iAttackPattern==0 && fAttackPatternTicks>150)
	{
		if(!bBigGun)
		if(gpEngine->mTimer.GetTime()>footRateOfFire+0.1f)
		{
			footRateOfFire = gpEngine->mTimer.GetTime();
			Bullet b;
			b.pSpawn = Spawn_BasicBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_BasicBullet;
			b.pCollide = Collide_BasicBullet;
			Vec2 v = FireAtTarget(mSphere.p,gpEngine->mPlayer.mSphere.p,4.5f);
			(*b.pSpawn)(b,mSphere.p,v);
			gpEngine->SpawnBullet(b,false);
			SND_LASER2;
		}
		if(bBigGun)
		{
			if(gpEngine->mTimer.GetTime()>CoreRateOfFire+0.3f)
			{
				CoreRateOfFire=gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_PowerBullet;
				b.pUpdate = Update_BasicBullet;
				b.pRender = Render_PowerBullet;
				b.pCollide = Collide_PowerBullet;
				(*b.pSpawn)(b,mSphere.p,Vec2(-5,0));
				b.rot = 0;
				gpEngine->SpawnBullet(b,false);
				(*b.pSpawn)(b,mSphere.p,Vec2(-5,-2));
				b.rot = 20;
				gpEngine->SpawnBullet(b,false);
				(*b.pSpawn)(b,mSphere.p,Vec2(-5,2));
				b.rot = -20;
				gpEngine->SpawnBullet(b,false);
				SND_LASER4;
			}
			if(gpEngine->mTimer.GetTime()>footRateOfFire+0.15f)
			{
				footRateOfFire = gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_BasicBullet;
				b.pUpdate = Update_BasicBullet;
				b.pRender = Render_BasicBullet;
				b.pCollide = Collide_BasicBullet;
				Vec2 v = FireAtTarget(mSphere.p,gpEngine->mPlayer.mSphere.p,4.5f);
				(*b.pSpawn)(b,mSphere.p,v);
				gpEngine->SpawnBullet(b,false);
				(*b.pSpawn)(b,mSphere.p,Vec2(v.x,-v.y));
				gpEngine->SpawnBullet(b,false);
				SND_LASER2;
			}	

		}
	}
	if(iAttackPattern==1 && fAttackPatternTicks>=150)
	{
		if(!bBigGun)
		if(gpEngine->mTimer.GetTime()>CoreRateOfFire+0.075f)
		{
			CoreRateOfFire=gpEngine->mTimer.GetTime();
			Bullet b;
			b.pSpawn = Spawn_BasicBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_BasicBullet;
			b.pCollide = Collide_BasicBullet;
			(*b.pSpawn)(b,mSphere.p,Vec2(-7,0));
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,mSphere.p,Vec2(-6,-3));
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,mSphere.p,Vec2(-6,3));
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,mSphere.p,Vec2(-5,-4));
			gpEngine->SpawnBullet(b,false);
			(*b.pSpawn)(b,mSphere.p,Vec2(-5,4));
			gpEngine->SpawnBullet(b,false);
			SND_LASER2;
		}
		if(bBigGun)
		{
			if(gpEngine->mTimer.GetTime()>CoreRateOfFire+0.9f)
			{
				CoreRateOfFire=gpEngine->mTimer.GetTime();
				Bullet b;
				b.pSpawn = Spawn_BlastBullet;
				b.pUpdate = Update_BasicBullet;
				b.pRender = Render_BlastBullet;
				b.pCollide = Collide_PowerBullet;
				Vec2 v = FireAtTarget(mSphere.p,gpEngine->mPlayer.mSphere.p,6.5f);
				(*b.pSpawn)(b,mSphere.p,v);
				Line l1(gpEngine->mPlayer.mSphere.p,mSphere.p);
				Line l2(mSphere.p,Vec2(mSphere.p.x,mSphere.p.y+Distance(gpEngine->mPlayer.mSphere.p,mSphere.p)));
				if(mSphere.p.x>gpEngine->mPlayer.mSphere.p.x)
					b.rot = -(RAD2DEG(l1.Angle(l2))-90);
				else
					b.rot = (RAD2DEG(l1.Angle(l2))+90);
				gpEngine->SpawnBullet(b,false);
				SND_LASER4;
			}
			if(gpEngine->fGameDifficulty!=3)
			{
				if(gpEngine->mTimer.GetTime()>footRateOfFire+0.05f)
				{
					footRateOfFire = gpEngine->mTimer.GetTime();
					Bullet b;
					b.pSpawn = Spawn_BasicBullet;
					b.pUpdate = Update_BasicBullet;
					b.pRender = Render_BasicBullet;
					b.pCollide = Collide_BasicBullet;
					switch(midActionFire)
					{
					case 0:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,4));
						break;
					case 1:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,3));
						break;
					case 2:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,2));
						break;
					case 3:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,1));
						break;
					case 4:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,0));
						break;
					case 5:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,-1));
						break;
					case 6:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,-2));
						break;
					case 7:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,-3));
						break;
					case 8:
						(*b.pSpawn)(b,mSphere.p,Vec2(-4,-4));
						break;
					}
					gpEngine->SpawnBullet(b,false);
					SND_LASER2;
					midActionFire<8?midActionFire++:midActionFire=0;
				}
			}
		}
	}
	if(iAttackPattern==2 && fAttackPatternTicks>=150)
	{
		if(gpEngine->mTimer.GetTime()>CoreRateOfFire+(0.6f*gpEngine->fGameDifficulty))
		{
			CoreRateOfFire=gpEngine->mTimer.GetTime();
			Bullet b;
			b.pSpawn = Spawn_PowerBullet;
			b.pUpdate = Update_BasicBullet;
			b.pRender = Render_BossPowerBullet;
			b.pCollide = Collide_PowerBullet;
			Vec2 v = FireAtTarget(mSphere.p,gpEngine->mPlayer.mSphere.p,4.5f);
			(*b.pSpawn)(b,mSphere.p,v);
			Line l1(gpEngine->mPlayer.mSphere.p,mSphere.p);
			Line l2(mSphere.p,Vec2(mSphere.p.x,mSphere.p.y+Distance(gpEngine->mPlayer.mSphere.p,mSphere.p)));
			if(mSphere.p.x>gpEngine->mPlayer.mSphere.p.x)
				b.rot = -(RAD2DEG(l1.Angle(l2))-90);
			else
				b.rot = (RAD2DEG(l1.Angle(l2))+90);
			gpEngine->SpawnBullet(b,false);
			SND_LASER4;
		}

	}
}
//================================================================================================//
						/******************
						** MidBoss draw **	
						*******************/
//================================================================================================//
void MidBoss::Draw(const float interp)
{
	Vec2 p = UTIL_Misc::Interpolate(Pos,oPos,interp);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if(bBigGun)
	{
		if(iGunDamageTicks>0)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
			glColor4f(1,1,0,1);
		}
		else
			glColor4f(1,1,1,1);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgMidBossGun);
		UTIL_GL::GL2D::DrawTextureQuad(p.x,p.y+32,p.x+64,p.y+96);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	if(iTakeDamageTicks>0)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glColor4f(1,1,0,1);
	}
	else
		glColor4f(1,1,1,1);
	//draw legs
//	glDisable(GL_TEXTURE_2D);
	for(int leg=0; leg<4; leg++)
	{
		float a=1,b=0,step=0.1f;
		float a2,b2;
		Vec2 point,point2,normal,p1,p2,n2;
		bool texes=false;

		UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgMidBossSeg);
		glBegin(GL_TRIANGLE_STRIP);
		for(int c=0;c<11;c++)
		{
			a-=step;
			b=1-a;
			point.x = mFoot[leg].x*a*a + mFootControl[leg].x*2*a*b + mFootJoint[leg].x*b*b;
			point.y = mFoot[leg].y*a*a + mFootControl[leg].y*2*a*b + mFootJoint[leg].y*b*b;
			a2=a;
			a2-=step;
			b2=1-a2;
			point2.x = mFoot[leg].x*a2*a2 + mFootControl[leg].x*2*a2*b2 + mFootJoint[leg].x*b2*b2;
			point2.y = mFoot[leg].y*a2*a2 + mFootControl[leg].y*2*a2*b2 + mFootJoint[leg].y*b2*b2;
			normal = MakeNormalForPoints(point,point2);
			if(!texes)
			{
				glTexCoord2f(0,0);
				glVertex2fv((point-normal*15).v);
				glTexCoord2f(1,0);
				glVertex2fv((point+normal*15).v);
			}
			else
			{
				glTexCoord2f(0,1);
				glVertex2fv((point-normal*15).v);
				glTexCoord2f(1,1);
				glVertex2fv((point+normal*15).v);
			}
			texes = !texes;
			if(c==0)
			{
				p1= point;
				p2= point2;
				n2= normal;
			}
		}
		glEnd();
		//draw feet
		UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgMidBossFoot);
		Vec2 ray(Normalize(p1-p2));
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2fv((p1+ray*18-n2*18).v);
		glTexCoord2f(1,0);
		glVertex2fv((p1+ray*18+n2*18).v);
		glTexCoord2f(1,1);
		glVertex2fv((p1-ray*18+n2*18).v);
		glTexCoord2f(0,1);
		glVertex2fv((p1-ray*18-n2*18).v);
		glEnd();
	}

	//draw body
	if(!bIsDead)
	{
		UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgMidBoss);
		UTIL_GL::GL2D::DrawTextureQuad(p.x,p.y,p.x+128,p.y+128);
		if(iTakeDamageTicks<=0)
		{
			glColor4f(CoreColor[0],CoreColor[1],CoreColor[2],1);
		}
		UTIL_GL::BindTexture(GL_TEXTURE_2D,gpEngine->imgMidBossCore);
		UTIL_GL::GL2D::DrawTextureQuad(p.x+44,p.y+33,p.x+108,p.y+97);
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(mSphere, 15);
	GeoDraw2D::DrawSphere(mSphereGun, 15);
	int n;
	for(n=0;n<6;n++)
		GeoDraw2D::DrawSphere(mBounds[n], 15);
	for(n=0;n<4;n++)
		for(int j=0;j<10;j++)
			GeoDraw2D::DrawSphere(mFootSphere[n][j], 15);
//*/
}
//================================================================================================//
						/*********************************
						** if it goes offscreen kill it **	
						**********************************/
//================================================================================================//
void MidBoss::NeedsToBeRemoved()
{

}
//================================================================================================//
						/****************
						** Take damage **	
						*****************/
//================================================================================================//
bool MidBoss::CheckCollided(Sphere s, float damage)
{
	if(!IsActive || bIsDead)
		return false;

	bool bHit = false;
	if(bBigGun)
	{
		if(Collision::SphereSphereOverlap(s,mSphereGun) || Collision::SphereSphereOverlap(s,mSphere))
		{
			if(bJustSpawned)
				return true;

			fGunLife -= damage;
			iGunDamageTicks=3;
			if(fGunLife<=0)
			{
				bBigGun = false;
				int RotAngle = rand()%360;
				gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphereGun.p,180,0.75f, (float)(RotAngle),false);
				gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphereGun.p,220,0.5f, (float)(RotAngle),true);
				SND_LARGEEXPLODE;
				gpEngine->mPlayer.iScore+=150;
				gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),1.0f);
				gpEngine->mFade.SetSrcColor(1,0,0,1);
				gpEngine->mFade.SetDstColor(1,0,0,0);
			}
			bHit = true;
		}
	}
	int n;
	for(n=0;n<6;n++)
	{
		if(Collision::SphereSphereOverlap(s,mBounds[n]))
			bHit= true;
	}
	for(n=0;n<4;n++)
	{
		for(int j=0;j<10;j++)
			if(Collision::SphereSphereOverlap(s,mFootSphere[n][j]))
				bHit = true;
	}

	if(Collision::SphereSphereOverlap(s,mSphere))
	{
		if(bJustSpawned)
			return true;

		fLife -= damage;
		iTakeDamageTicks = 3;
		bHit = true;
	}
//	else
//		return false;

	if(fLife>=150)
	{
		float interp = (fLife-150)/150.0f;
		CoreColor[0] = UTIL_Misc::TimeRamp(1,0,0,1,interp);
	}
	else
	{
		float interp = fLife/150.0f;
		CoreColor[1] = UTIL_Misc::TimeRamp(0,1,0,1,interp);
	}
	if(fLife<=0)
	{
		fAttackPatternTicks = 0;
		bIsDead = true;
		int RotAngle = rand()%360;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,mSphere.p,220,0.75f, (float)(RotAngle),false);
		gpEngine->SpawnExplosion(gpEngine->sprExplosion2,mSphere.p,220,0.5f, (float)(RotAngle),true);
		SND_LARGEEXPLODE2;
		gpEngine->mPlayer.iScore+=500;
		gpEngine->mFade.StartFade(gpEngine->mTimer.GetTime(),1.5f);
		gpEngine->mFade.SetSrcColor(1,0,0,1);
		gpEngine->mFade.SetDstColor(1,0,0,0);
		gpEngine->bPauseScrolling = false;
	}

	return bHit;
}

void MidBoss::LoadFromFile(CFileIO &fIO)
{
}
void MidBoss::WriteToFile(CFileIO &fIO)
{
}
int MidBoss::InWater()
{
	return 0;
}