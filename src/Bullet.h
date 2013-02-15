#pragma once
#include "Vector.h"
#include "Geo2D.h"
#include "Geo2D_Draw.h"
#include "Entity.h"
class Bullet
{
public:
	bool IsActive;
	Vec2 oPos,Pos,Vel;
	float frame;
	float fLife;//how much damage it does
	float rot;
	float user1,user2;//just incase
	void (*pSpawn)(Bullet& b, Vec2 pos, Vec2 vel);
	void (*pUpdate)(Bullet& b);
	void (*pRender)(Bullet& b, const float interp);
	void (*pCollide)(Bullet& b);

	Sphere sphere;
	Entity* pTarget;//for bullets that can chase.
};
Vec2 FireAtTarget(Vec2 curPos, Vec2 tarPos, float speed);
bool FireAtDistantTarget(Vec2 curPos, Vec2 tarPos, float speed, Vec2 &out);
bool CollidedWithMap(Bullet b);
void PickTarget(Bullet& b);
//player bullets
void Render_PlayerBullet(Bullet& b, const float interp);
void Collide_PlayerBullet(Bullet& b);
void Spawn_ForceBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Update_ForceBullet(Bullet& b);
void Render_ForceBullet(Bullet& b, const float interp);
void Collide_ForceBullet(Bullet& b);
void Spawn_PlasmaBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Update_PlasmaBullet(Bullet& b);
void Render_PlasmaBullet(Bullet& b, const float interp);
void Collide_PlasmaBullet(Bullet& b);
void Update_ClusterBullet(Bullet& b);
void Render_ClusterBullet(Bullet& b, const float interp);
void Collide_ClusterBullet(Bullet& b);
void Spawn_LifeBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Update_LifeBullet(Bullet& b);
void Render_LifeBullet(Bullet& b, const float interp);
void Collide_LifeBullet(Bullet& b);
void Spawn_HomingMissile(Bullet& b, Vec2 pos, Vec2 vel);
void Update_HomingMissile(Bullet& b);
void Render_HomingMissile(Bullet& b, const float interp);
void Collide_HomingMissile(Bullet& b);
void Spawn_PulseBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Update_PulseBullet(Bullet& b);
void Render_PulseBullet(Bullet& b, const float interp);
void Collide_PulseBullet(Bullet& b);
//Enemy bullets
void Spawn_BasicBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Update_BasicBullet(Bullet& b);
void Render_BasicBullet(Bullet& b, const float interp);
void Collide_BasicBullet(Bullet& b);

void Spawn_GravityBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Update_GravityBullet(Bullet& b);
void Render_GravityBullet(Bullet& b, const float interp);
void Collide_GravityBullet(Bullet& b);
void Spawn_PowerBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Render_PowerBullet(Bullet& b, const float interp);
void Collide_PowerBullet(Bullet& b);

//midboss bullets
void Spawn_BlastBullet(Bullet& b, Vec2 pos, Vec2 vel);
void Render_BlastBullet(Bullet& b, const float interp);
//bigboss bullets
void Spawn_BossBullet(Bullet& b, Vec2 pos, Vec2 vel);//same properties as power bullet
void Update_BossBullet(Bullet& b);
void Render_BossBullet(Bullet& b, const float interp);
void Render_BossPowerBullet(Bullet& b, const float interp);
