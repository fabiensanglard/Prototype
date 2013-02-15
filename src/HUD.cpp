#include "Engine.h"
#include "UTIL_Misc.h"
//================================================================================================//
						/***************
						** Render HUD **	
						****************/
//================================================================================================//
void Engine::RenderHud(float interp)
{
	glDisable(GL_TEXTURE_2D);
	UTIL_GL::GL2D::SetOrtho(640,480);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0,0,0,1);
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(640,0);
	glColor4f(0,0,0,0);
	glVertex2f(640,64);
	glVertex2f(0,64);
	glVertex2f(0,416);
	glVertex2f(640,416);
	glColor4f(0,0,0,1);
	glVertex2f(640,480);
	glVertex2f(0,480);
	glEnd();
	glDisable(GL_BLEND);

	//force charger
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float force = (float)mPlayer.iForceCharge*2;
	if(force>200) force = 200;
	glBegin(GL_QUADS);
	if(mPlayer.iForceCharge>=100)
		glColor4f(UTIL_Misc::TimeRamp(0,1,(float)0,25,(float)mPlayer.iChargeFlash),
				UTIL_Misc::TimeRamp(0.25f,1,(float)0,25,(float)mPlayer.iChargeFlash),
				UTIL_Misc::TimeRamp(0.75f,0,(float)0,25,(float)mPlayer.iChargeFlash),0.7f);
	else
		glColor4f(0,0.25f,0.75f,0.7f);
	glVertex2f(220,430);
	if(mPlayer.iForceCharge>=100)
		glColor4f(UTIL_Misc::TimeRamp(0,1,(float)0,25,(float)mPlayer.iChargeFlash),
				UTIL_Misc::TimeRamp(0.5f,1,(float)0,25,(float)mPlayer.iChargeFlash),
				UTIL_Misc::TimeRamp(1,0,(float)0,50,(float)mPlayer.iChargeFlash),0.7f);
	else
		glColor4f(0,0.5f,1,0.7f);
	glVertex2f(220+force,430);
	glVertex2f(220+force,450);
	if(mPlayer.iForceCharge>=100)
		glColor4f(UTIL_Misc::TimeRamp(0,1,(float)0,25,(float)mPlayer.iChargeFlash),
				UTIL_Misc::TimeRamp(0.25f,1,(float)0,25,(float)mPlayer.iChargeFlash),
				UTIL_Misc::TimeRamp(0.75f,0,(float)0,25,(float)mPlayer.iChargeFlash),0.7f);
	else
		glColor4f(0,0.25f,0.75f,0.7f);
	glVertex2f(220,450);
	glColor4f(0,0,0,0.7f);
	glVertex2f(220+force,430);
	glVertex2f(420,430);
	glVertex2f(420,450);
	glVertex2f(220+force,450);
	glEnd();

	glColor4f(0,0,0,0.7f);
	glLineWidth(5);
	glBegin(GL_LINE_LOOP);
	glVertex2f(220,430);
	glVertex2f(420,430);
	glVertex2f(420,450);
	glVertex2f(220,450);
	glEnd();
	glLineWidth(1);

	glColor4f(1,1,1,0.75f);
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D, imgHudBeam);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(200,424);
	glTexCoord2f(1,0);
	glVertex2f(232,424);
	glTexCoord2f(1,1);
	glVertex2f(232,454);
	glTexCoord2f(0,1);
	glVertex2f(200,454);

	glTexCoord2f(1,0);
	glVertex2f(402,424);
	glTexCoord2f(0,0);
	glVertex2f(440,424);
	glTexCoord2f(0,1);
	glVertex2f(440,454);
	glTexCoord2f(1,1);
	glVertex2f(402,454);

	glTexCoord2f(1,0);
	glVertex2f(232,424);
	glTexCoord2f(1,0);
	glVertex2f(402,424);
	glTexCoord2f(1,1);
	glVertex2f(402,454);
	glTexCoord2f(1,1);
	glVertex2f(232,454);

	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1,1,1,1);
	//weapon name
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(0.5f,0.5f);
	mFont1.SetAlignment(ALIGN_CENTER);
	mFont1.SetColor(1,1,0,0.75f);
	mFont1.Print("FORCE METER:",320,430);
	mFont1.SetScale(1,0.5f);
	mFont1.SetColor(1,1,1,1);
	if(mPlayer.bHasOrb)
	{
		switch(mPlayer.mOrb.iWeaponType)
		{
		case 0:	mFont1.Print("PLASMA",320,452); break;
		case 1:	mFont1.Print("RAIL",320,452); break;
		case 2:	mFont1.Print("PULSE",320,452); break;
		case 3:	mFont1.Print("MAGMA",320,452); break;
		}
	}
	char sz[32];
	mFont1.SetScale(0.5f,0.5f);
	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.Print("SCORE:",100, 16);
	sprintf(sz,"TOP:");
	mFont1.Print(sz,320,16);
	sprintf(sz,"LIVES");
	mFont1.Print(sz,10,16);

	mFont2.SetScale(0.5f,0.5f);
	mFont2.SetColor(1,1,1,1);
	mFont2.SetAlignment(ALIGN_LEFT);
	sprintf(sz,"%i",mPlayer.iLives);
	mFont2.Print(sz,60,16);
	sprintf(sz,"%.8i",mPlayer.iScore);
	mFont2.Print(sz,155,16);
	sprintf(sz,"%.8i %s",iHiScore[0],szHiScore[0]);
	mFont2.Print(sz,355,16);

}