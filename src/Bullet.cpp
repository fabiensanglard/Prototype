#include "Engine.h"
#include "Debris.h"
#include "RandExplosion.h"
//================================================================================================//
						/**********************************************************************
						** return a velocity that will make a bullet travel towards a target **	
						***********************************************************************/
//================================================================================================//
Vec2 FireAtTarget(Vec2 curPos, Vec2 tarPos, float speed)
{
	Vec2 vel;
	vel = Normalize(tarPos-curPos)*speed;
	if(!gpEngine->bPauseScrolling)
		vel.x += SCROLL_SPEED + gpEngine->fMapSpeed;
	return vel;
}
bool FireAtDistantTarget(Vec2 curPos, Vec2 tarPos, float speed, Vec2 &out)
{
	if(Distance(curPos,tarPos)<250)
		return false;
	if(curPos.x<gpEngine->Scroll)
		return false;
	if(curPos.y<0)
		return false;
	if(curPos.y>480)
		return false;

	out = Normalize(tarPos-curPos)*speed;
	if(!gpEngine->bPauseScrolling)
		out.x += SCROLL_SPEED + gpEngine->fMapSpeed;
	return true;
}
bool CollidedWithMap(Bullet b)
{
	int x,y;//,mx,my;
/*	x = (int)((float)((b.Pos.x-b.sphere.r)/32.0f));
	mx = (int)((float)((b.Pos.x+b.sphere.r)/32.0f));
	y = (int)((float)((b.Pos.y-b.sphere.r)/32.0f));
	my = (int)((float)((b.Pos.y+b.sphere.r)/32.0f));
	if(x<0)x=0;
	if(y<0)y=0;
	*/
	x=(int)((float)(b.sphere.p.x/32.0f));
	y=(int)((float)(b.sphere.p.y/32.0f));
//	Box box;
//	for(x; x<mx; x++)
	{
//		for(y=0; y<mx; y++)
		{
			if(gpEngine->mTilesLayer1[x][y].ID>=0 && gpEngine->mTilesLayer1[x][y].ID<512)
			{
//				box= Box(32,32,Vec2((float)(x*32)+16, (float)(y*32)+16));
//				if(Collision::SphereBoxOverlap(b.sphere,box))
					return true;
			}
		}
	}
	return false;
}
//================================================================================================//
						/*****************************
						** basic player bullet type **	
						******************************/
//================================================================================================//
void Render_PlayerBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderSprite(gpEngine->sprBulletSet1,1,p.x,p.y,24,24);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 5);*/
}
void Collide_PlayerBullet(Bullet& b)
{
	if(!b.IsActive)
		return;
	gpEngine->SpawnSpriteEffect(1,b.sphere.p,Vec2(32,32),gpEngine->sprBulletHit3,0.25f,false,NULL,true);
	b.IsActive = false;
	SND_LASER3;
}
//================================================================================================//
						/*****************************
						** Force player bullet type **	
						******************************/
//================================================================================================//
void Spawn_ForceBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.Vel = vel;
	switch((int)b.frame)
	{
	case 1:
		pos.y+=28;
		b.fLife = 3 *gpEngine->fGameDifficulty;
		b.sphere = Sphere(7,pos+Vec2(16,8));
		break;
	case 2:
		pos.y+=20;
		b.fLife = 7 *gpEngine->fGameDifficulty;
		b.sphere = Sphere(12,pos+Vec2(24,12));
		break;
	case 3:
		pos.y+=16;
		b.fLife = 14 *gpEngine->fGameDifficulty;
		b.sphere = Sphere(16,pos+Vec2(32,16));
		break;
	case 4:
		pos.y+=8;
		b.fLife = 30 *gpEngine->fGameDifficulty;
		b.sphere = Sphere(25,pos+Vec2(48,24));
		break;
	case 5:
		pos.y+=28;
		b.fLife = 3 *gpEngine->fGameDifficulty;
		b.sphere = Sphere(7,pos+Vec2(16,8));
		break;

	}
	b.oPos = b.Pos = pos;
}
void Update_ForceBullet(Bullet& b)
{
	b.oPos = b.Pos;
	b.Pos += b.Vel;

	if(b.Pos.x<gpEngine->Scroll
		|| b.Pos.x>gpEngine->Scroll+640
		|| b.Pos.y<0
		|| b.Pos.y>480
		|| b.fLife<=0)
		b.IsActive = false;

	if(CollidedWithMap(b))
	{
		b.fLife = 0;
		b.pCollide(b);
		b.IsActive = false;
	}
	b.sphere.p += (b.Pos-b.oPos);
}
void Render_ForceBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
	glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	switch((int)b.frame)
	{
	case 1:
		RenderSprite(gpEngine->sprBulletForce,0,p.x,p.y,32,16);
		break;
	case 2:
		RenderSprite(gpEngine->sprBulletForce,0,p.x,p.y,48,24);
		break;
	case 3:
		RenderSprite(gpEngine->sprBulletForce,0,p.x,p.y,64,32);
		break;
	case 4:
		RenderSprite(gpEngine->sprBulletForce,0,p.x,p.y,96,48);
		break;
	case 5:
		RenderRotatedSprite(gpEngine->sprBulletSet1,3,p.x,p.y,16,8,b.rot);
		break;
	}

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 10);*/
}
void Collide_ForceBullet(Bullet& b)
{
	if(!b.IsActive)
		return;
	if(b.fLife<=0)
	{
		if(b.frame<5)
			gpEngine->SpawnExplosion(gpEngine->sprBlueExplosion,b.sphere.p,50*b.frame,0.75f,0,true);
		else
			gpEngine->SpawnSpriteEffect(1,b.sphere.p,Vec2(32,32),gpEngine->sprBulletHit,0.5f,false,NULL,true);
		SND_LASER3;
	}
}
//================================================================================================//
						/******************
						** Plasma bullet **	
						*******************/
//================================================================================================//
void Spawn_PlasmaBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 0.75f *gpEngine->fGameDifficulty;
	b.sphere = Sphere(8,pos);
}
void Update_PlasmaBullet(Bullet& b)
{
	b.oPos = b.Pos;
	b.Pos += b.Vel;

	if(b.Pos.x<gpEngine->Scroll-100
		|| b.Pos.x>gpEngine->Scroll+640
		|| b.Pos.y<0
		|| b.Pos.y>480
		|| b.fLife<=0)
		b.IsActive = false;

	if(CollidedWithMap(b))
	{
		b.pCollide(b);
	}

	b.frame<2? b.frame+=0.5f: b.frame=0;
	b.sphere.p += (b.Pos-b.oPos);
}
void Render_PlasmaBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_ONE,GL_ONE);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	Vec2 o = p-b.Pos;
	Vec2 r = (b.oPos+o)-p;
	for(int n=0;n<5;n++)
	{
		float s = (float)(n/5.0f);
//		glColor4f(0,1,0,1);
		glColor4f(0,1*s,0,1*s);
//		RenderRotatedSprite(gpEngine->sprGlowBullets,0,p.x,p.y,15,15,b.rot);
		RenderRotatedSprite(gpEngine->sprGlowBullets,0,p.x-r.x*(s*2),p.y-r.y*(s*2),15,15,b.rot);
	}
/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 5);//*/
}
void Collide_PlasmaBullet(Bullet& b)
{
	if(!b.IsActive)
		return;
	gpEngine->SpawnSpriteEffect(1,b.sphere.p,Vec2(32,32),gpEngine->sprBulletHit2,0.25f,false,NULL,true);
	b.IsActive = false;
	SND_LASER3;
}
//================================================================================================//
						/******************
						** Pulse bullet **	
						*******************/
//================================================================================================//
void Spawn_PulseBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 2.75f *gpEngine->fGameDifficulty;
	b.sphere = Sphere(8,pos);
}
void Update_PulseBullet(Bullet& b)
{
	b.oPos = b.Pos;
	b.Pos += b.Vel;

	if(b.Pos.x<gpEngine->Scroll-100
		|| b.Pos.x>gpEngine->Scroll+640
		|| b.Pos.y<0
		|| b.Pos.y>480
		|| b.fLife<=0)
		b.IsActive = false;

	if(CollidedWithMap(b))
	{
		b.pCollide(b);
	}

	b.frame<2? b.frame+=0.5f: b.frame=0;
	b.sphere.p += (b.Pos-b.oPos);
}
void Render_PulseBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	glColor4f(1,1,1,1);
	RenderRotatedSprite(gpEngine->sprBulletSet1,4,p.x,p.y,15,15,b.rot);
/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 5);//*/
}
void Collide_PulseBullet(Bullet& b)
{
	if(!b.IsActive)
		return;
	gpEngine->SpawnSpriteEffect(1,b.sphere.p,Vec2(32,32),gpEngine->sprBulletHit,0.25f,false,NULL,true);
	b.IsActive = false;
	SND_LASER3;
}
//================================================================================================//
						/******************
						** Cluster bullet **	
						*******************/
//================================================================================================//
void Update_ClusterBullet(Bullet& b)
{
	b.oPos = b.Pos;
	b.user1+=0.097f;
	b.Pos += b.Vel;
	b.Pos.y += b.user1;
	
	Line ray(b.oPos,b.Pos);
	Line ray2(b.Pos,Vec2(b.Pos.x , b.Pos.y+100));
//	b.user2?
	if(ray.r.x<0)
		b.rot = RAD2DEG(ray.Angle(ray2))-90;
	else
		b.rot = -(RAD2DEG(ray.Angle(ray2))+90);
	b.rot = UTIL_Misc::Wrap(b.rot,360);
	if(b.Pos.x<gpEngine->Scroll-100
		|| b.Pos.x>gpEngine->Scroll+640
		|| b.Pos.y>480
		|| b.fLife<=0)
		b.IsActive = false;
	if(CollidedWithMap(b))
	{
		Collide_ClusterBullet(b);
	}
	b.sphere.p += (b.Pos-b.oPos);

	b.frame<7?b.frame+=0.75f:b.frame=0;
}
void Render_ClusterBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderRotatedSprite(gpEngine->sprCluster,(int)b.frame,p.x+8,p.y+8,-16,16,b.rot);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 15);//*/
}
void Collide_ClusterBullet(Bullet& b)
{
	if(!b.IsActive)
		return;
	b.IsActive = false;
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(50,0),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(-50,0),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(0,50),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(0,-50),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(35,35),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(-35,35),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(35,-35),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(-35,-35),70,0.75f, (float)(rand()%360),false);
	gpEngine->SpawnExplosion(gpEngine->sprImplosion,b.Pos,230,0.95f, (float)(rand()%360),true);
	SND_LARGEEXPLODE;
	Bullet bl;
	bl.pSpawn = Spawn_LifeBullet;
	bl.pUpdate = Update_LifeBullet;
	bl.pRender = Render_LifeBullet;
	bl.pCollide = Collide_LifeBullet;
	(*bl.pSpawn)(bl,b.Pos,Vec2(0,0));
	gpEngine->SpawnBullet(bl,true);

}
//================================================================================================//
						/*******************
						** homing missile **	
						********************/
//================================================================================================//
void PickTarget(Bullet& b)
{
	list<Entity*>::iterator i;
	float heuristic=19999;
	float d;
	Vec2 u,r;
	UTIL_Misc::MakeVectors(b.rot,u,r);
	for(i = gpEngine->mGameEnts.begin(); i!=gpEngine->mGameEnts.end(); i++)
	{
		if(dynamic_cast<Debris*>((*i)) || dynamic_cast<PowerUp*>((*i)) || dynamic_cast<RandExplosion*>((*i)))//dont target debris
			continue;
		d = Distance(b.sphere.p,(*i)->mSphere.p);
		d += 1 + (1.0f-Dot(r, Normalize(b.sphere.p - (*i)->mSphere.p)))*400;
		if(d<heuristic)
		{
			heuristic = d;
			b.pTarget = (*i);
		}
	}
	for(i = gpEngine->mTopGameEnts.begin(); i!=gpEngine->mTopGameEnts.end(); i++)
	{
		if(dynamic_cast<BreakScenery*>((*i)))//dont target breakable scenery
			continue;
		d = Distance(b.sphere.p,(*i)->mSphere.p);
		d += 1 + (1.0f-Dot(r, Normalize(b.sphere.p - (*i)->mSphere.p)))*400;
		if(d<heuristic)
		{
			if(dynamic_cast<PowerUp*>((*i)))
				continue;
			heuristic = d;
			b.pTarget = (*i);
		}
	}
}
void Spawn_HomingMissile(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 3;
	b.sphere = Sphere(5,pos+Vec2(14,8));
	b.user1 = 0;//denots just spawned
	b.user2 = 0;
	b.rot = -90;
	b.pTarget = NULL;
}
void Update_HomingMissile(Bullet& b)
{
	if(!b.IsActive)
		return;

	if(b.user1==0)
	{
		b.user2++;
		if(b.user2>20)
		{
			b.user1=1;
			b.user2=0;
		}
		b.Vel*=0.9f;
	}
	else
	{
		Vec2 u,r;
		UTIL_Misc::MakeVectors(b.rot,u,r);
		b.Vel = r * 5;
		if(b.pTarget == NULL || !b.pTarget->IsActive)
			PickTarget(b);
		else
		{
			Vec2 toTarget;
			toTarget = Normalize(b.sphere.p-b.pTarget->mSphere.p);
			if(Dot(u,toTarget)<0)
				b.rot -= (1.0f - Dot(r,toTarget))*2.5f;
			else
				b.rot += (1.0f - Dot(r,toTarget))*2.5f;
		}
		b.user2++;
		if(b.user2>3)
		{
			b.user2 = 0;
			gpEngine->SpawnExplosion(gpEngine->sprSmokePuff,b.sphere.p,50,0.45f, (float)(rand()%360),true);
		}
	}
	b.oPos = b.Pos;
	b.Pos += b.Vel;

	if(b.Pos.x<gpEngine->Scroll-100
		|| b.Pos.x>gpEngine->Scroll+640
		|| b.Pos.y<-100
		|| b.Pos.y>580
		|| b.fLife<=0)
		b.IsActive = false;
	if(CollidedWithMap(b))
	{
		b.pCollide(b);
	}
	b.frame<15? b.frame+=0.5f: b.frame-=15;
	b.sphere.p += (b.Pos-b.oPos);
}
void Render_HomingMissile(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderRotatedSprite(gpEngine->sprHomingMissile,(int)b.frame,p.x+8,p.y+8,16,16,b.rot);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 15);//*/
}
void Collide_HomingMissile(Bullet& b)
{
	b.IsActive = false;
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(0,0),130,0.75f, (float)(rand()%360),false);
	SND_MEDEXPLODE;
}
//================================================================================================//
						/******************
						** life bullet **	
						*******************/
//================================================================================================//
void Spawn_LifeBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 3;//was 15
	b.sphere = Sphere(65,pos);
}
void Update_LifeBullet(Bullet& b)
{
	if(!b.IsActive)
		return;

	if(b.fLife<=0)
		b.IsActive = false;
	b.fLife -= 1.0f;
}
void Render_LifeBullet(Bullet& b, const float interp)
{
/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 15);//*/
}
void Collide_LifeBullet(Bullet& b)
{
}
//================================================================================================//
						/**********************
						** basic bullet type **	
						***********************/
//================================================================================================//
void Spawn_BasicBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife =  1 *gpEngine->fGameDifficulty;
	b.sphere = Sphere(5,pos+Vec2(14,14));
}
void Update_BasicBullet(Bullet& b)
{
	if(!b.IsActive)
		return;

	b.oPos = b.Pos;
	b.Pos += b.Vel;

	if(b.Pos.x<gpEngine->Scroll-100
		|| b.Pos.x>gpEngine->Scroll+640
		|| b.Pos.y<0
		|| b.Pos.y>480
		|| b.fLife<=0)
		b.IsActive = false;
	if(CollidedWithMap(b))
	{
		b.pCollide(b);
	}
	b.frame<2? b.frame+=0.5f: b.frame=0;
	b.sphere.p += (b.Pos-b.oPos);
}
void Render_BasicBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	switch((int)b.frame)
	{
	case 0:		glColor4f(1,0,0,1); break;
	case 1:		glColor4f(1,1,0,1); break;
	case 2:		glColor4f(0.5f,0,0.5f,1); break;
	}
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderSprite(gpEngine->sprBulletSet1,0,p.x,p.y,24,24);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 5);*/
}
void Collide_BasicBullet(Bullet& b)
{
	if(!b.IsActive)
		return;
	gpEngine->SpawnSpriteEffect(1,b.sphere.p,Vec2(32,32),gpEngine->sprBulletHit,0.5f,false,NULL,true);
	b.IsActive = false;
}
//================================================================================================//
						/************************
						** Gravity bullet type **	
						*************************/
//================================================================================================//
void Spawn_GravityBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 5;
	b.sphere = Sphere(7,pos+Vec2(8,8));
	b.user1 =0;
}
void Update_GravityBullet(Bullet& b)
{
	b.oPos = b.Pos;
	b.user1+=0.097f;
	b.Pos += b.Vel;
	b.Pos.y += b.user1;
	
	Line ray(b.oPos,b.Pos);
	Line ray2(b.Pos,Vec2(b.Pos.x , b.Pos.y+100));
	b.user2?
		b.rot = RAD2DEG(ray.Angle(ray2))-90:
		b.rot = -(RAD2DEG(ray.Angle(ray2))+90);
	b.rot = UTIL_Misc::Wrap(b.rot,360);
	if(b.Pos.x<gpEngine->Scroll-100
		|| b.Pos.x>gpEngine->Scroll+640
		|| b.Pos.y>480
		|| b.fLife<=0)
		b.IsActive = false;
	if(CollidedWithMap(b))
	{
		b.IsActive = false;
		gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(0,0),130,0.75f, (float)(rand()%360),false);
		SND_MEDEXPLODE;
	}
	b.sphere.p += (b.Pos-b.oPos);
	b.frame<7?b.frame+=0.5f:b.frame=0;
}
void Render_GravityBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderRotatedSprite(gpEngine->sprMissile,(int)b.frame,p.x+8,p.y+8,16,16,b.rot);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 15);//*/
}
void Collide_GravityBullet(Bullet& b)
{
	b.IsActive = false;
	gpEngine->SpawnExplosion(gpEngine->sprExplosionSmall,b.Pos+Vec2(0,0),130,0.75f, (float)(rand()%360),false);
	SND_MEDEXPLODE;
}
//================================================================================================//
						/**********************
						** Power bullet type **	
						***********************/
//================================================================================================//
void Spawn_PowerBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 10;
	b.sphere = Sphere(5,pos+Vec2(8,8));
}
void Render_PowerBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0,1,0,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderRotatedSprite(gpEngine->sprBulletSet1,2,p.x+8,p.y+8,16,16,b.rot);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 5);//*/
}
void Collide_PowerBullet(Bullet& b)
{
	if(!b.IsActive)
		return;
	gpEngine->SpawnSpriteEffect(1,b.sphere.p,Vec2(32,32),gpEngine->sprBulletHit2,0.35f,false,NULL,true);
	b.IsActive = false;
}
//================================================================================================//
						/******************
						** mid boss type **	
						*******************/
//================================================================================================//

void Spawn_BlastBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 1;
	b.sphere = Sphere(8,pos+Vec2(10,10));
}
void Render_BlastBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	Vec2 r,u;
	UTIL_Misc::MakeVectors(b.rot,r,u);
	float rot = b.rot-90;
	glColor4f(1,0,0,1);
	RenderRotatedSprite(gpEngine->sprGlowBullets,2,p.x+8+r.x*10,p.y+8+r.y*10,16,16,rot);
	glColor4f(1,0.5f,0,1);
	RenderRotatedSprite(gpEngine->sprGlowBullets,2,p.x+8+r.x*5,p.y+8+r.y*5,16,16,rot);
	glColor4f(1,1,0,1);
	RenderRotatedSprite(gpEngine->sprGlowBullets,2,p.x+8,p.y+8,16,16,rot);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 15);//*/
}
//================================================================================================//
						/******************
						** big boss type **	
						*******************/
//================================================================================================//

void Spawn_BossBullet(Bullet& b, Vec2 pos, Vec2 vel)
{
	b.IsActive = true;
	b.oPos = b.Pos = pos;
	b.Vel = vel;
	b.frame = 0;
	b.fLife = 1000;
	b.sphere = Sphere(32,pos+Vec2(32,48));
}
void Update_BossBullet(Bullet& b)
{
	if(!b.IsActive)
		return;

	b.oPos = b.Pos;
	b.Pos += b.Vel;

	if(b.Pos.x<gpEngine->Scroll-100
		|| b.Pos.y<0
		|| b.Pos.y>480
		|| b.fLife<=0)
		b.IsActive = false;

	b.frame<15? b.frame+=0.75f: b.frame-=15;
	b.sphere.p += (b.Pos-b.oPos);
}
void Render_BossBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);

	glColor4f(1,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderSprite(gpEngine->sprFireBall,(int)b.frame,p.x-96,p.y-64,256,256);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 15);//*/
}
void Render_BossPowerBullet(Bullet& b, const float interp)
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
	glColor4f(0.3f,1,1,1);
	Vec2 p = UTIL_Misc::Interpolate(b.Pos,b.oPos,interp);
	RenderRotatedSprite(gpEngine->sprGlowBullets,0,p.x+8,p.y+8,16,16,b.rot+90);

/*	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,0,1);
	GeoDraw2D::DrawSphere(b.sphere, 5);//*/
}

