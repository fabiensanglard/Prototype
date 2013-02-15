#include "Engine.h"

SpriteEffect Engine::GetAnimForEditor(Vec2 pos)
{
	SpriteEffect s;
	switch(iTileFrame)
	{
	case 0://test
		s.Spawn(pos,Vec2(64,128),gpEngine->sprAnimBack1,0.3f,false,NULL,false);
		s.bAlignCenter = false;
		break;
	case 1://fore tile anim1
		s.Spawn(pos,Vec2(64,64),gpEngine->sprAnimFore1,0.5f,false,NULL,false);
		s.bAlignCenter = false;
		break;
	case 2://fore tile anim2
		s.Spawn(pos,Vec2(32,32),gpEngine->sprAnimFore2,0.5f,false,NULL,false);
		s.bAlignCenter = false;
		break;
	case 3://fore tile anim3
		s.Spawn(pos,Vec2(64,32),gpEngine->sprAnimFore3,0.5f,false,NULL,false);
		s.bAlignCenter = false;
		break;
	case 4://back tile anim2
		s.Spawn(pos,Vec2(32,32),gpEngine->sprAnimBack2,0.15f,false,NULL,false);
		s.bAlignCenter = false;
		break;
	case 5://back tile anim2 (larger)
		s.Spawn(pos,Vec2(64,64),gpEngine->sprAnimBack2,0.15f,false,NULL,false);
		s.bAlignCenter = false;
		break;
	}
	return s;
}