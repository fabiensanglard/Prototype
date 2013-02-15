#include "Engine.h"
void Engine::StartFadeIn()
{
//	gLog.OutPut("StartFadeIn Called\n");
	mFade.StartFadeIn(mTimer.GetTime(),2,1);
}
void Engine::StartFadeOut()
{
//	gLog.OutPut("StartFadeOut Called\n");
	mFade.StartFadeOut(mTimer.GetTime(),0,1);
}
void Engine::StartFrontEnd()
{
//	gLog.OutPut("StartFrontEnd START\n");
	bInGame = false;
	if(!bDemo)
	{
		stmSplash.Load("Data/Sound/AttractMode.ogg",true);
		stmSplash.Play(30);
	}
	bDemo = false;
	fUIDelay=mTimer.GetTime();
	IntroTimer= mTimer.GetTime();

	bRecording = false;
	pRecordEnt = NULL;
	bUseEditor = false;
	iEditMap = 1;
	iStage = 1;
	Scroll = 0;
	bConfigureKey = false;
	iOption = 0;
	mbTimedState = false;
	fGameOverFrame = 0;
	StartFadeIn();
	SetState(&Engine::UpdateIntro,&Engine::RenderIntro);
//	gLog.OutPut("StartFrontEnd END\n");
}
//================================================================================================//
						/**********************
						** Render a UI panel **	
						***********************/
//================================================================================================//
#define UIMAP(x) x/32.0f //map position into texture coordinate
void Engine::RenderUIPanel(float x, float y, float w, float h)
{
//	gLog.OutPut("RenderUI START\n");

	UTIL_GL::BindTexture(GL_TEXTURE_2D, imgUI);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glTexCoord2f(UIMAP(0),UIMAP(0));
	glVertex2f(x,y);
	glTexCoord2f(UIMAP(16),UIMAP(0));
	glVertex2f(x+16,y);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+16);
	glTexCoord2f(UIMAP(0),UIMAP(16));
	glVertex2f(x,y+16);
	//topright
	glTexCoord2f(UIMAP(16),UIMAP(0));
	glVertex2f(x+w-16,y);
	glTexCoord2f(UIMAP(32),UIMAP(0));
	glVertex2f(x+w,y);
	glTexCoord2f(UIMAP(32),UIMAP(16));
	glVertex2f(x+w,y+16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+16);
	//bottomright
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+h-16);
	glTexCoord2f(UIMAP(32),UIMAP(16));
	glVertex2f(x+w,y+h-16);
	glTexCoord2f(UIMAP(32),UIMAP(32));
	glVertex2f(x+w,y+h);
	glTexCoord2f(UIMAP(16),UIMAP(32));
	glVertex2f(x+w-16,y+h);
	//bottomleft
	glTexCoord2f(UIMAP(0),UIMAP(16));
	glVertex2f(x,y+h-16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+h-16);
	glTexCoord2f(UIMAP(16),UIMAP(32));
	glVertex2f(x+16,y+h);
	glTexCoord2f(UIMAP(0),UIMAP(32));
	glVertex2f(x,y+h);
	//topmiddle
	glTexCoord2f(UIMAP(16),UIMAP(0));
	glVertex2f(x+16,y);
	glTexCoord2f(UIMAP(16),UIMAP(0));
	glVertex2f(x+w-16,y);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+16);
	//bottom middle
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+h-16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+h-16);
	glTexCoord2f(UIMAP(16),UIMAP(32));
	glVertex2f(x+w-16,y+h);
	glTexCoord2f(UIMAP(16),UIMAP(32));
	glVertex2f(x+16,y+h);
	//middle
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+h-16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+h-16);
	//left middle
	glTexCoord2f(UIMAP(0),UIMAP(16));
	glVertex2f(x,y+16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+16,y+h-16);
	glTexCoord2f(UIMAP(0),UIMAP(16));
	glVertex2f(x,y+h-16);
	//right middle
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+16);
	glTexCoord2f(UIMAP(32),UIMAP(16));
	glVertex2f(x+w,y+16);
	glTexCoord2f(UIMAP(32),UIMAP(16));
	glVertex2f(x+w,y+h-16);
	glTexCoord2f(UIMAP(16),UIMAP(16));
	glVertex2f(x+w-16,y+h-16);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
//	gLog.OutPut("RenderUI END\n");

}
//================================================================================================//
						/******************
						** Splash Update **	
						*******************/
//================================================================================================//
float msgScroll = 640,oldMsg;
void Engine::UpdateSplash(float delta)
{
	oldMsg=msgScroll;
	if(msgScroll<-mFont2.GetStringLength("GR33TZ TO EVERYONE THAT ENTERED THE SHMUP-DEV.com CONTEST AND THOSE SPECIAL FEW THAT IDLED ON THE IRC CHANNEL!    BIG THANKYOU TO MATT FOR PULLING IT ALLTOGETHER!   IF YOU HAVE A GAMEPAD PLUG IT IN - KEYBOARDS ARENT MEANT FOR THIS KIND OF BASHING!!!     TIPS: LIKE TO CHEAT?    READ THE HI-SCORE BOARD FOR WEAPON CHEATS...   SUCK TOO MUCH?   ENTER YOUR NAME AS WIMPS ON THE HI-SCORE BOARD...   LIKE BONUS FEATURES?   ENTER YOUR NAME AS HYPER FOR AN INTENSE GAMING EXPERIENCE!    TOO IMPATIENT?   ENTER YOUR NAME AS BOSSY TO SKIP TO THE BIG FINALE."))
	{
		oldMsg=msgScroll=640;
	}
	msgScroll-=2.0f;

	if(fGameDifficulty!=1 && fGameDifficulty!=2 && fGameDifficulty!=3)
		fGameDifficulty = 3;

//	gLog.OutPut("UpdateSplash START\n");
	if(mTimer.GetTime()<fUIDelay)
		return;

	if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
	{
		iOption>0?
			iOption--:
			iOption=2;
		smpUImove.Play(1);
		fUIDelay = mTimer.GetTime()+0.15f;
	}
	if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
	{
		iOption<2?
			iOption++:
			iOption=0;
		smpUImove.Play(1);
		fUIDelay = mTimer.GetTime()+0.15f;
	}
	if(bCompileWithEditor)
	if(iOption == 2)
	{
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			iEditMap>1?	iEditMap--: iEditMap=5;
			smpUImove.Play(1);
			fUIDelay = mTimer.GetTime()+0.15f;
		}
		else if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			iEditMap<5?	iEditMap++: iEditMap=1;
			smpUImove.Play(1);
			fUIDelay = mTimer.GetTime()+0.15f;
		}
	}
	if(iOption == 0)
	{
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			smpUImove.Play(1);
			fGameDifficulty<3? fGameDifficulty++: fGameDifficulty = 1;
			fUIDelay = mTimer.GetTime()+0.15f;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			smpUImove.Play(1);
			fGameDifficulty>1? fGameDifficulty--: fGameDifficulty = 3;
			fUIDelay = mTimer.GetTime()+0.15f;
		}
	}
	if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		fUIDelay = mTimer.GetTime()+0.15f;
		oldMsg=msgScroll;
		switch(iOption)
		{
		case 0://start game
			stmSplash.Stop(30);
			smpUIselect.Play(1);
			SetState(&Engine::UpdateSplash2,&Engine::RenderSplash);
			SetStateTimed(&Engine::UpdateGame,&Engine::RenderGame,2,&Engine::StartGame);
			mFade.StartFadeOut(mTimer.GetTime(),0,2.15f);
			KEY_ATTACK1.state = RELEASED;
			break;
		case 1://configure
			smpUImove.Play(1);
			SetState(&Engine::UpdateSplash2,&Engine::RenderSplash);
			SetStateTimed(&Engine::UpdateConfig,&Engine::RenderConfig,0.25f,&Engine::StartFadeIn);
			mFade.StartFadeOut(mTimer.GetTime(),2,0.3f);
			break;
		case 2://editor
			if(bCompileWithEditor)
			{
				stmSplash.Stop(30);
				smpUImove.Play(1);
				if(!LoadMapData())
					return;
				if(!LoadEditorData())
					return;
				bUseEditor = true;
				SetState(NULL,&Engine::RenderSplash);
				SetStateTimed(&Engine::UpdateEditor,&Engine::RenderEditor,0.25f);
				StartEditor();
			}
			else
			{
				smpUImove.Play(1);
				SetState(&Engine::UpdateSplash2,&Engine::RenderSplash);
				SetStateTimed(&Engine::UpdateHowToPlay,&Engine::RenderHowToPlay,0.25f,&Engine::StartFadeIn);
				mFade.StartFadeOut(mTimer.GetTime(),2,0.3f);
			}
			break;
		}
	}

	if(!mWeaponCheats)
	if(KEY_M.state == PRESSED && KEY_A.state == PRESSED && KEY_R.state == PRESSED && KEY_Y.state == PRESSED)
	{
		mWeaponCheats = true;
		SND_LARGEEXPLODE2;
	}
	mFade.UpdateFade(mTimer.GetTime());

//	gLog.OutPut("UpdateSplash END\n");
}
void Engine::UpdateSplash2(float delta)
{
	mFade.UpdateFade(mTimer.GetTime());
}
//================================================================================================//
						/******************
						** Splash Render **	
						*******************/
//================================================================================================//
void Engine::RenderSplash(const float interp)
{

//	gLog.OutPut("RenderSplash START\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplashBack);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(64,0);
	glTexCoord2f(1,0); glVertex2f(576,0);
	glTexCoord2f(1,1); glVertex2f(576,512);
	glTexCoord2f(0,1); glVertex2f(64,512);
	glEnd();
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplashFire);
	for(int f=0;f<2;f++)
	{
		float sinus=0.3f;
		float fire;
		float t=gpEngine->mTimer.GetTime();
		if(f>0)
			fire = sin(t*=1.5f)*10;
		else
			fire = sin(t*5)*10;
		glBegin(GL_QUADS);
		glTexCoord2f((float)(f?1:0),0); glVertex2f(32+fire,			40);
		glTexCoord2f((float)(f?0:1),0); glVertex2f(608+fire,			40);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608+fire*0.75f,	82);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32+fire*0.75f,		82);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32+fire*0.75f,		82);
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608+fire*0.75f,	82);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.5f,			124);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.5f,			124);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.5f,			124);
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.5f,			124);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608+fire*0.25f,	166);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32+fire*0.25f,		166);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32+fire*0.25f,		166);
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608+fire*0.25f,	166);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),1); glVertex2f(608,			208);
		glTexCoord2f((float)(f?1:0),1); glVertex2f(32,			208);
		glEnd();
	}
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplash);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(64,50);
	glTexCoord2f(1,0); glVertex2f(576,50);
	glTexCoord2f(1,1); glVertex2f(576,178);
	glTexCoord2f(0,1); glVertex2f(64,178);
	glEnd();

	glColor4f(0.3f,0.6f,0.8f,0.8f);
	RenderUIPanel(150,300,340,32);
	RenderUIPanel(150,340,340,32);
	RenderUIPanel(150,380,340,32);
	mFont1.SetAlignment(ALIGN_CENTER);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(0.5f,0.5f);
	iOption==0?
		mFont1.SetColor(1,1,0,1):
		mFont1.SetColor(1,1,1,1);
	if(fGameDifficulty==1)
		mFont1.Print("START GAME <HARD>",320,308);
	if(fGameDifficulty==2)
		mFont1.Print("START GAME <MEDIUM>",320,308);
	if(fGameDifficulty==3)
		mFont1.Print("START GAME <EASY>",320,308);
	iOption==1?
		mFont1.SetColor(1,1,0,1):
		mFont1.SetColor(1,1,1,1);
	mFont1.Print("CONFIGURE",320,348);
	iOption==2?
		mFont1.SetColor(1,1,0,1):
		mFont1.SetColor(1,1,1,1);

	if(bCompileWithEditor)
	{
		char sz[32];
		sprintf(sz,"EDIT MAP: %i",iEditMap);
		mFont1.Print(sz,320,388);
	}
	else
	{
		mFont1.Print("HOW TO PLAY",320,388);
	}
	mFont2.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont2.EnableSineWave(0.03f,10);
	mFont2.SetScale(0.5f,0.5f);
	mFont2.SetColor(1,1,0,1);
	mFont2.SetAlignment(ALIGN_LEFT);
	float msg = UTIL_Misc::Interpolate(msgScroll,oldMsg,interp);
	mFont2.Print("GR33TZ TO EVERYONE THAT ENTERED THE SHMUP-DEV.com CONTEST AND THOSE SPECIAL FEW THAT IDLED ON THE IRC CHANNEL!    BIG THANKYOU TO MATT FOR PULLING IT ALLTOGETHER!   IF YOU HAVE A GAMEPAD PLUG IT IN - KEYBOARDS ARENT MEANT FOR THIS KIND OF BASHING!!!     TIPS: LIKE TO CHEAT?    READ THE HI-SCORE BOARD FOR WEAPON CHEATS...   SUCK TOO MUCH?   ENTER YOUR NAME AS WIMPS ON THE HI-SCORE BOARD...   LIKE BONUS FEATURES?   ENTER YOUR NAME AS HYPER FOR AN INTENSE GAMING EXPERIENCE!    TOO IMPATIENT?   ENTER YOUR NAME AS BOSSY TO SKIP TO THE BIG FINALE.",msg,450);
	mFade.RenderFade(mTimer.GetTime());
	mFont2.DisableSineWave();
//	gLog.OutPut("RenderSplash END\n");
}

//================================================================================================//
						/******************
						** Config Update **	
						*******************/
//================================================================================================//
void Engine::UpdateConfig(float delta)
{
	if(mTimer.GetTime()<fUIDelay)
		return;
	if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
	{
		iOption>0?
			(iOption==6? iOption=13: iOption--):
			iOption=13;
		smpUImove.Play(1);
		fUIDelay = mTimer.GetTime()+0.15f;
	}
	if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
	{
		iOption<13?
			(iOption==5? iOption = 12: iOption++):
			iOption=0;
		smpUImove.Play(1);
		fUIDelay = mTimer.GetTime()+0.15f;
	}
	if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
	{
		iOption<6? iOption+=6: iOption;
		smpUImove.Play(1);
		fUIDelay = mTimer.GetTime()+0.15f;
	}
	if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
	{
		iOption>5? iOption-=6: iOption;
		smpUImove.Play(1);
		fUIDelay = mTimer.GetTime()+0.15f;
	}
	if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		fUIDelay = mTimer.GetTime()+0.15f;
		smpUImove.Play(1);
		switch(iOption)
		{
		case 12://restore default
			InitializeKeys();
			break;

		case 13://back to main
			//write out new cfg
			DeleteFile("Controls.cfg");
			gSerializer.PutComment("Controls.cfg","[Control Configuration]");
			for(int n=0; n<6;n++)
			{
				char sz[256];
				sprintf(sz,"%s_Primary",mKeys[n].KeyName);
				gSerializer.ReadVariable("Controls.cfg",sz,mKeys[n].PrimaryKey);
				sprintf(sz,"%s_Secondary",mKeys[n].KeyName);
				gSerializer.ReadVariable("Controls.cfg",sz,mKeys[n].SecondaryKey);
			}
			//back to main
			SetState(&Engine::UpdateSplash2,&Engine::RenderConfig);
			SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0.25f,&Engine::StartFadeIn);
			mFade.StartFadeOut(mTimer.GetTime(),2,0.3f);
			break;

		default:
			bConfigureKey = true;
			iOption<6? iConfigKey = iOption: iConfigKey = iOption-6;
			bConfigPrimary = (iOption<6)? true: false; 
			break;
		}
	}
}
//================================================================================================//
						/******************
						** Config Render **	
						*******************/
//================================================================================================//
void Engine::RenderConfig(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);
	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplashBack);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(64,0);
	glTexCoord2f(1,0); glVertex2f(576,0);
	glTexCoord2f(1,1); glVertex2f(576,512);
	glTexCoord2f(0,1); glVertex2f(64,512);
	glEnd();
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplashFire);
	for(int f=0;f<2;f++)
	{
		float fire;
		if(f>0)
			fire = -sin(gpEngine->mTimer.GetTime()*5)*3;
		else
			fire = sin(gpEngine->mTimer.GetTime()*5)*10;
		fire = -fire*0.75f;
		glBegin(GL_QUADS);
		glTexCoord2f((float)(f?1:0),0); glVertex2f(32+fire*2,			15);
		glTexCoord2f((float)(f?0:1),0); glVertex2f(608+fire*2,			15);
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608-fire*1.25f,	57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32-fire*1.25f,		57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32-fire*1.25f,		57);
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608-fire*1.25f,	57);
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.75f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.75f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.75f,			99);
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.75f,			99);
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608-fire*0.25f,	141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32-fire*0.25f,		141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32-fire*0.25f,		141);
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608-fire*0.25f,	141);
		glTexCoord2f((float)(f?0:1),1); glVertex2f(608,			183);
		glTexCoord2f((float)(f?1:0),1); glVertex2f(32,			183);
		glEnd();
	}	for(int f=0;f<2;f++)
	{
		float sinus=0.3f;
		float fire;
		float t=gpEngine->mTimer.GetTime();
		if(f>0)
			fire = sin(t*=1.5f)*10;
		else
			fire = sin(t*5)*10;
		glBegin(GL_QUADS);
		glTexCoord2f((float)(f?1:0),0); glVertex2f(32+fire,			15);
		glTexCoord2f((float)(f?0:1),0); glVertex2f(608+fire,			15);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608+fire*0.75f,	57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32+fire*0.75f,		57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32+fire*0.75f,		57);
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608+fire*0.75f,	57);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.5f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.5f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.5f,			99);
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.5f,			99);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608+fire*0.25f,	141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32+fire*0.25f,		141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32+fire*0.25f,		141);
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608+fire*0.25f,	141);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),1); glVertex2f(608,			183);
		glTexCoord2f((float)(f?1:0),1); glVertex2f(32,			183);
		glEnd();
	}
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplash);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(64,25);
	glTexCoord2f(1,0); glVertex2f(576,25);
	glTexCoord2f(1,1); glVertex2f(576,153);
	glTexCoord2f(0,1); glVertex2f(64,153);
	glEnd();

	glColor4f(0.3f,0.6f,0.8f,0.8f);
	int n;
	for(n=0; n<8;n++)
	{
		RenderUIPanel(100,160+(40*(float)n),440,32);
	}
	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(0.5f,0.5f);
	for(n=0; n<6;n++)
	{
		mFont1.SetColor(1,1,1,1);
		mFont1.Print(mKeys[n].KeyName,110,168 + (40*(float)n));
		iOption==n?
			(bConfigureKey? mFont1.SetColor(0,1,1,1): mFont1.SetColor(1,1,0,1)): mFont1.SetColor(1,1,1,1);
		mFont1.Print(SDLKeyStrings[mKeys[n].PrimaryKey],280,168 + (40*(float)n));
		iOption==(n+6)?
			(bConfigureKey? mFont1.SetColor(0,1,1,1): mFont1.SetColor(1,1,0,1)): mFont1.SetColor(1,1,1,1);
		mFont1.Print(SDLKeyStrings[mKeys[n].SecondaryKey],420,168 + (40*(float)n));
	}

	mFont1.SetAlignment(ALIGN_CENTER);
	iOption==12?
		mFont1.SetColor(1,1,0,1):
		mFont1.SetColor(1,1,1,1);
	mFont1.Print("RESTORE DEFAULTS",320,168 + (40*(float)6));
	iOption==13?
		mFont1.SetColor(1,1,0,1):
		mFont1.SetColor(1,1,1,1);
	mFont1.Print("BACK",320,168 + (40*(float)7));
	mFade.RenderFade(mTimer.GetTime());

}
//================================================================================================//
						/**************
						** Game Over **	
						***************/
//================================================================================================//
void Engine::StartGameOver()
{
	bInGame = false;
	bHyperMode = false;
	bBossFight = false;
	bWimpMode = false;
	SetState(&Engine::UpdateGameOver,&Engine::RenderGameOver);
	stmSplash.Load("Data/Sound/GameOver.ogg",false);
	stmSplash.Play(30);
	StartFadeIn();
	fGameOverFrame = 0;
	bGameOverFrame = false;
	SetStateTimed(&Engine::UpdateGameOver,&Engine::RenderGameOver,10,&Engine::EndGameOver);
}
void Engine::EndGameOver()
{
	for(int n=0;n<10;n++)
	{
		if(mPlayer.iScore>iHiScore[n])
		{
			SetStateTimed(&Engine::UpdateEnterHiScore,&Engine::RenderEnterHiScore,3,&Engine::StartEnterHiScore);
			mFade.StartFadeOut(mTimer.GetTime(),2,3.5f);
			return;
		}
	}
	SetStateTimed(&Engine::UpdateIntro,&Engine::RenderIntro,3,&Engine::StartFrontEnd);
	mFade.StartFadeOut(mTimer.GetTime(),2,3.5f);
}
void Engine::UpdateGameOver(float delta)
{
	if(!bGameOverFrame)
		fGameOverFrame<15? fGameOverFrame += 0.25f:(fGameOverFrame=15,bGameOverFrame=true);
	else
		fGameOverFrame>0? fGameOverFrame -= 0.25f:(fGameOverFrame=1,bGameOverFrame=false);
	if(fGameOverFrame>15)
		fGameOverFrame=15;
	if(fGameOverFrame<0)
		fGameOverFrame=0;
	mFade.UpdateFade(mTimer.GetTime());
}
void Engine::RenderGameOver(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);
	
	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	RenderSprite(sprGameOver,(int)fGameOverFrame,0,-50,600,600);

	UTIL_GL::BindTexture(GL_TEXTURE_2D, imgGameOver);
	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(64,50);
	glTexCoord2f(1,0); glVertex2f(596,50);
	glTexCoord2f(1,1); glVertex2f(596,178);
	glTexCoord2f(0,1); glVertex2f(64,178);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	mFade.RenderFade(mTimer.GetTime());

}
//================================================================================================//
						/**********
						** Intro **	
						***********/
//================================================================================================//
void Engine::UpdateIntro(float delta)
{
	if(mTimer.GetTime()>IntroTimer+20)
	{
		SetStateTimed(&Engine::UpdateIntro2,&Engine::RenderIntro2,1,&Engine::StartFadeIn);
		mFade.StartFadeOut(mTimer.GetTime(),2,1.1f);
		IntroTimer= mTimer.GetTime()+1;
		fUIDelay = mTimer.GetTime();
		bGameOverFrame = true;
	}
	if(KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		SetState(&Engine::UpdateSplash,&Engine::RenderSplash);
		SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0);
		mFade.StartFadeIn(mTimer.GetTime(),2,0.5f);
		fUIDelay = mTimer.GetTime();
		iOption = 0;
		return;
	}
	mFade.UpdateFade(mTimer.GetTime());

}
void Engine::RenderIntro(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	float s = (mTimer.GetTime()-fUIDelay)*25;
	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(1,0.5f);
	mFont1.SetColor(1,1,1,1);
	mFont1.Print("\"From the dark regions of space they",10,480-s);
	mFont1.Print(" came...Waging war upon us. ",10,500-s);
	mFont1.Print(" One saviour stood his ground while all",10,520-s);
	mFont1.Print(" others were crushed under the alien",10,540-s);
	mFont1.Print(" assault...",10,560-s);
	mFont1.Print(" The Prototype MarkI.\"",10,600-s);
	glDisable(GL_TEXTURE_2D);
	UTIL_GL::GL2D::SetOrtho(640,480);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_COLOR);
	glColor4f(0,0,0,1);
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(640,0);
	glVertex2f(640,100);
	glVertex2f(0,100);
	glVertex2f(0,100);
	glVertex2f(640,100);
	glColor4f(0,0,0,0);
	glVertex2f(640,150);
	glVertex2f(0,150);
	glVertex2f(0,330);
	glVertex2f(640,330);
	glColor4f(0,0,0,1);
	glVertex2f(640,430);
	glVertex2f(0,430);
	glVertex2f(0,430);
	glVertex2f(640,430);
	glVertex2f(640,480);
	glVertex2f(0,480);
	glEnd();
	glDisable(GL_BLEND);
	mFade.RenderFade(mTimer.GetTime());
}
void Engine::UpdateIntro2(float delta)
{
	if(mTimer.GetTime()>IntroTimer+5)
	{
		SetStateTimed(&Engine::UpdateIntro3,&Engine::RenderIntro3,1,&Engine::StartFadeIn);
		mFade.StartFadeOut(mTimer.GetTime(),2,1.1f);
		IntroTimer= mTimer.GetTime()+1;
		fUIDelay = mTimer.GetTime();
		bGameOverFrame = true;
	}
	if(KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		SetState(&Engine::UpdateSplash,&Engine::RenderSplash);
		SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0);
		mFade.StartFadeIn(mTimer.GetTime(),2,0.5f);
		fUIDelay = mTimer.GetTime();
		iOption = 0;
		return;
	}
	mFade.UpdateFade(mTimer.GetTime());
}
void Engine::RenderIntro2(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	UTIL_GL::BindTexture(GL_TEXTURE_2D, imgProtoTypeMk1);
	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0,0);
	glTexCoord2f(1,0); glVertex2f(640,0);
	glTexCoord2f(1,1); glVertex2f(640,480);
	glTexCoord2f(0,1); glVertex2f(0,480);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	mFade.RenderFade(mTimer.GetTime());
}
void Engine::UpdateIntro3(float delta)
{
	if(mTimer.GetTime()>fUIDelay)
	{
		bGameOverFrame = !bGameOverFrame;
		if(!bGameOverFrame)
			fUIDelay = mTimer.GetTime()+0.5f;
		else
			fUIDelay = mTimer.GetTime()+1;
	}
	if(mTimer.GetTime()>IntroTimer+6)
	{
		SetStateTimed(&Engine::UpdateIntro4,&Engine::RenderIntro4,1,&Engine::StartFadeIn);
		IntroTimer = mTimer.GetTime();
		mFade.StartFadeOut(mTimer.GetTime(),2,1.1f);
		return;
	}
	fGameOverFrame>15? fGameOverFrame-=15:fGameOverFrame += 0.25f;
	if(KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		SetState(&Engine::UpdateSplash,&Engine::RenderSplash);
		SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0);
		mFade.StartFadeIn(mTimer.GetTime(),2,0.5f);
		fUIDelay = mTimer.GetTime();
		iOption = 0;
		return;
	}
	mFade.UpdateFade(mTimer.GetTime());
}
void Engine::RenderIntro3(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//	RenderSprite(sprIntro,(int)fGameOverFrame,120,40,400,400);
	RenderSprite(sprIntro,(int)fGameOverFrame,0,-80,640,640);

	glDisable(GL_TEXTURE_2D);
	UTIL_GL::GL2D::SetOrtho(640,480);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_COLOR);
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

	mFade.RenderFade(mTimer.GetTime());

	mFont1.SetAlignment(ALIGN_CENTER);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(1,0.5f);
	mFont1.SetColor(1,1,1,1);
	if(bGameOverFrame)
		mFont1.Print("PRESS START",320,240);


}

void Engine::UpdateIntro4(float delta)
{
	if(mTimer.GetTime()>fUIDelay)
	{
		bGameOverFrame = !bGameOverFrame;
		if(!bGameOverFrame)
			fUIDelay = mTimer.GetTime()+0.5f;
		else
			fUIDelay = mTimer.GetTime()+1;
	}
	fGameOverFrame>6? fGameOverFrame-=6:fGameOverFrame += 0.25f;
	if(mTimer.GetTime()>IntroTimer+6)
	{
		SetStateTimed(&Engine::UpdateHiScores,&Engine::RenderHiScores,1,&Engine::StartFadeIn);
		mFade.StartFadeOut(mTimer.GetTime(),2,1.1f);
		IntroTimer = mTimer.GetTime()+1;
		fUIDelay = mTimer.GetTime()+1;
		iOption = 0;
		return;
	}
	if(KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		SetState(&Engine::UpdateSplash,&Engine::RenderSplash);
		SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0);
		mFade.StartFadeIn(mTimer.GetTime(),2,0.5f);
		fUIDelay = mTimer.GetTime();
		return;
	}
	mFade.UpdateFade(mTimer.GetTime());
}
void Engine::RenderIntro4(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//	RenderSprite(sprIntro,(int)fGameOverFrame,120,40,400,400);
	RenderSprite(sprIntro2,(int)fGameOverFrame,0,-80,640,640);

	glDisable(GL_TEXTURE_2D);
	UTIL_GL::GL2D::SetOrtho(640,480);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_COLOR);
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

	mFade.RenderFade(mTimer.GetTime());

	mFont1.SetAlignment(ALIGN_CENTER);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(1,0.5f);
	mFont1.SetColor(1,1,1,1);
	if(bGameOverFrame)
		mFont1.Print("PRESS START",320,240);
}
//================================================================================================//
						/*************
						** hiscores **	
						**************/
//================================================================================================//
void Engine::UpdateHiScores(float delta)
{
	if(mTimer.GetTime()>fUIDelay)
	{
		bGameOverFrame = !bGameOverFrame;
		if(!bGameOverFrame)
			fUIDelay = mTimer.GetTime()+0.5f;
		else
			fUIDelay = mTimer.GetTime()+1;
	}
	fGameOverFrame>15? fGameOverFrame-=15:fGameOverFrame += 0.25f;
	if(mTimer.GetTime()>IntroTimer+6)
	{
/*		SetStateTimed(UpdateIntro,RenderIntro,1,StartFadeIn);
		mFade.StartFadeOut(mTimer.GetTime(),2,1.1f);
*/
		IntroTimer = mTimer.GetTime()+2;
		fUIDelay = mTimer.GetTime()+2;
		bDemo = true;
		SetState(&Engine::UpdateSplash2,&Engine::RenderHiScores);
		SetStateTimed(&Engine::UpdateGame,&Engine::RenderGame,2,&Engine::StartGame);
		mFade.StartFadeOut(mTimer.GetTime(),0,2.1f);
		fGameDifficulty = 2;
		return;
	}
	if(KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		SetState(&Engine::UpdateSplash,&Engine::RenderSplash);
		SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0);
		mFade.StartFadeIn(mTimer.GetTime(),2,0.5f);
		fUIDelay = mTimer.GetTime();
		iOption = 0;
		return;
	}
	mFade.UpdateFade(mTimer.GetTime());
}
void Engine::RenderHiScores(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	RenderSprite(sprHiScore,(int)fGameOverFrame,40,-40,560,560);

	mFont2.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont2.SetScale(2,2);
	mFont2.SetAlignment(ALIGN_CENTER);
	mFont2.SetColor(1,1,0,1);
	mFont2.Print("ALL TIME GREATEST",320,20);
	mFont2.SetScale(2,1);
	mFont2.SetColor(1,1,1,1);
	for(int n=0; n<10; n++)
	{
		mFont2.SetAlignment(ALIGN_LEFT);
		mFont2.Print(szHiScore[n],30,80+(35*(float)n));
		mFont2.SetAlignment(ALIGN_RIGHT);
		char sz[32];
		sprintf(sz,"%i",iHiScore[n]);
		mFont2.Print(sz,600,80+(35*(float)n));
	}
	mFade.RenderFade(mTimer.GetTime());
	mFont1.SetAlignment(ALIGN_CENTER);
	mFont1.SetScale(1,0.5f);
	mFont1.SetColor(1,1,1,1);
	if(bGameOverFrame)
		mFont1.Print("PRESS START",320,240);
}
//================================================================================================//
						/*********************
						** entering hiscore **	
						**********************/
//================================================================================================//
void Engine::StartEnterHiScore()
{
	SetState(&Engine::UpdateEnterHiScore,&Engine::RenderEnterHiScore);
	stmSplash.Load("Data/Sound/HiScores.ogg",false);
	stmSplash.Play(30);
	StartFadeIn();
	fGameOverFrame = 0;
	bGameOverFrame = false;
	iScoreEntryDigit = 0;
	iScoreEntryCharacter = 65;
	fUIDelay =mTimer.GetTime();
	for(int n=9; n>-1; n--)
	{
		if(n>0)
		{
			if(mPlayer.iScore>iHiScore[n])
			{
				iHiScore[n]=iHiScore[n-1];
				strcpy(szHiScore[n],szHiScore[n-1]);
			}
			else
			{
				iHiScore[n+1] =mPlayer.iScore;
				strcpy(szHiScore[n+1],"_____");
				iHiScoreOverWrite=n+1;
				break;
			}
		}
		else
		{
			if(mPlayer.iScore>iHiScore[n])
			{
				iHiScore[n] =mPlayer.iScore;
				strcpy(szHiScore[n],"_____");
				iHiScoreOverWrite=n;
				break;
			}
			else
			{
				iHiScore[n+1] =mPlayer.iScore;
				strcpy(szHiScore[n+1],"_____");
				iHiScoreOverWrite=n+1;
				break;
			}
		}
	}
	szHiScore[iHiScoreOverWrite][iScoreEntryDigit] = iScoreEntryCharacter;
}
void Engine::UpdateEnterHiScore(float delta)
{
	if(mTimer.GetTime()>fUIDelay)
	{
		if(KEY_UI_UP.state == PRESSED || KEY_UP.state == PRESSED)
		{
			iScoreEntryCharacter>33?iScoreEntryCharacter--:iScoreEntryCharacter=126;
			szHiScore[iHiScoreOverWrite][iScoreEntryDigit] = iScoreEntryCharacter;
			fUIDelay = mTimer.GetTime()+0.1f;
		}
		if(KEY_UI_DOWN.state == PRESSED || KEY_DOWN.state == PRESSED)
		{
			iScoreEntryCharacter<126?iScoreEntryCharacter++:iScoreEntryCharacter=33;
			szHiScore[iHiScoreOverWrite][iScoreEntryDigit] = iScoreEntryCharacter;
			fUIDelay = mTimer.GetTime()+0.1f;
		}
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
		{
			iScoreEntryDigit++;
			if(iScoreEntryDigit>4)
			{
				SetState(&Engine::UpdateSplash2,&Engine::RenderEnterHiScore);
				SetStateTimed(&Engine::UpdateIntro,&Engine::RenderIntro,3,&Engine::StartFrontEnd);
				mFade.StartFadeOut(mTimer.GetTime(),2,3.1f);
				IntroTimer = mTimer.GetTime()+1;
				fUIDelay = mTimer.GetTime()+1;
				if(stricmp(szHiScore[iHiScoreOverWrite],"AAAAA")==0)
				{
					strcpy(szHiScore[iHiScoreOverWrite],"SHMUP");
				}
				if(stricmp(szHiScore[iHiScoreOverWrite],"hyper")==0)
				{
					bHyperMode = true;
					SND_LARGEEXPLODE2;
				}
				if(stricmp(szHiScore[iHiScoreOverWrite],"wimps")==0)
				{
					bWimpMode = !bWimpMode;
					SND_LARGEEXPLODE2;
				}
				if(stricmp(szHiScore[iHiScoreOverWrite],"bossy")==0)
				{
					bBossFight = !bBossFight;
					SND_LARGEEXPLODE2;
				}
				return;
			}
			szHiScore[iHiScoreOverWrite][iScoreEntryDigit] = iScoreEntryCharacter;
			fUIDelay = mTimer.GetTime()+0.1f;
		}
	}
	mFade.UpdateFade(mTimer.GetTime());
	fGameOverFrame>15? fGameOverFrame-=15:fGameOverFrame += 0.25f;
}
void Engine::RenderEnterHiScore(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	RenderSprite(sprHiScore,(int)fGameOverFrame,40,-40,560,560);

	mFont2.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont2.SetScale(2,2);
	mFont2.SetAlignment(ALIGN_CENTER);
	mFont2.SetColor(1,1,0,1);
	mFont2.Print("A NEW HI-SCORE!",320,20);
	mFont2.SetScale(2,1);
	for(int n=0; n<10; n++)
	{
		if(iHiScoreOverWrite == n && iScoreEntryDigit<5)
		{
			float c = (cos(mTimer.GetTime()*5)*0.5f)+0.5f;
			if(c<0.25f)c=0.25f;
			if(c>1)c=1;
			mFont2.SetColor(c,c,0,1);
		}
		else
			mFont2.SetColor(1,1,1,1);
		mFont2.SetAlignment(ALIGN_LEFT);
		mFont2.Print(szHiScore[n],30,80+(35*(float)n));
		mFont2.SetAlignment(ALIGN_RIGHT);
		char sz[32];
		mFont2.SetColor(1,1,1,1);
		sprintf(sz,"%i",iHiScore[n]);
		mFont2.Print(sz,600,80+(35*(float)n));
	}
	mFade.RenderFade(mTimer.GetTime());
}
//================================================================================================//
						/*********************
						** congratulations **	
						**********************/
//================================================================================================//

void Engine::UpdateCongratulations(float delta)
{
}
void Engine::RenderCongratulations(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	glDisable(GL_BLEND);
	UTIL_GL::BindTexture(GL_TEXTURE_2D, imgCongrats);
	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0,0);
	glTexCoord2f(1,0); glVertex2f(640,0);
	glTexCoord2f(1,1); glVertex2f(640,480);
	glTexCoord2f(0,1); glVertex2f(0,480);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	float s = (mTimer.GetTime()-fUIDelay)*25;
	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(1,0.5f);
	mFont1.SetColor(1,1,1,1);
	mFont1.Print("\"...and so the evil tyrants where",40,480-s);
	mFont1.Print(" forced out of our solar system",40,500-s);
	mFont1.Print(" and back to the dark corners of",40,520-s);
	mFont1.Print(" space.",40,540-s);
	mFont1.Print(" Let it be told to each generation,",40,560-s);
	mFont1.Print(" 'The Prototype MarkI' .\"",40,600-s);
	glDisable(GL_TEXTURE_2D);
	UTIL_GL::GL2D::SetOrtho(640,480);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_COLOR);
	glColor4f(0,0,0,1);
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(640,0);
	glVertex2f(640,100);
	glVertex2f(0,100);
	glVertex2f(0,100);
	glVertex2f(640,100);
	glColor4f(0,0,0,0);
	glVertex2f(640,150);
	glVertex2f(0,150);
	glVertex2f(0,330);
	glVertex2f(640,330);
	glColor4f(0,0,0,1);
	glVertex2f(640,430);
	glVertex2f(0,430);
	glVertex2f(0,430);
	glVertex2f(640,430);
	glVertex2f(640,480);
	glVertex2f(0,480);
	glEnd();
	glDisable(GL_BLEND);
	mFade.RenderFade(mTimer.GetTime());
}

//================================================================================================//
						/****************
						** How To Play **	
						*****************/
//================================================================================================//
void Engine::UpdateHowToPlay(float delta)
{
	if(mTimer.GetTime()<fUIDelay)
		return;
//	float time = (mTimer.GetTime()-fUIDelay);
	if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED || KEY_UI_SELECT2.state == JUST_PRESSED)
	{
		smpUImove.Play(1);
		SetState(&Engine::UpdateSplash2,&Engine::RenderHowToPlay);
		SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0.25f,&Engine::StartFadeIn);
		mFade.StartFadeOut(mTimer.GetTime(),2,0.3f);
	}
}
void Engine::RenderHowToPlay(const float interp)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplashBack);
	glColor4f(0.3f,0.6f,0.8f,0.8f);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(64,0);
	glTexCoord2f(1,0); glVertex2f(576,0);
	glTexCoord2f(1,1); glVertex2f(576,512);
	glTexCoord2f(0,1); glVertex2f(64,512);
	glEnd();
	glColor4f(1,1,1,1);
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplashFire);
	for(int f=0;f<2;f++)
	{
		float fire;
		if(f>0)
			fire = -sin(gpEngine->mTimer.GetTime()*5)*3;
		else
			fire = sin(gpEngine->mTimer.GetTime()*5)*10;
		fire = -fire*0.75f;
		glBegin(GL_QUADS);
		glTexCoord2f((float)(f?1:0),0); glVertex2f(32+fire*2,			15);
		glTexCoord2f((float)(f?0:1),0); glVertex2f(608+fire*2,			15);
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608-fire*1.25f,	57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32-fire*1.25f,		57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32-fire*1.25f,		57);
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608-fire*1.25f,	57);
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.75f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.75f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.75f,			99);
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.75f,			99);
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608-fire*0.25f,	141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32-fire*0.25f,		141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32-fire*0.25f,		141);
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608-fire*0.25f,	141);
		glTexCoord2f((float)(f?0:1),1); glVertex2f(608,			183);
		glTexCoord2f((float)(f?1:0),1); glVertex2f(32,			183);
		glEnd();
	}
	for(int f=0;f<2;f++)
	{
		float sinus=0.3f;
		float fire;
		float t=gpEngine->mTimer.GetTime();
		if(f>0)
			fire = sin(t*=1.5f)*10;
		else
			fire = sin(t*5)*10;
		glBegin(GL_QUADS);
		glTexCoord2f((float)(f?1:0),0); glVertex2f(32+fire,			15);
		glTexCoord2f((float)(f?0:1),0); glVertex2f(608+fire,			15);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608+fire*0.75f,	57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32+fire*0.75f,		57);
		glTexCoord2f((float)(f?1:0),0.25f); glVertex2f(32+fire*0.75f,		57);
		glTexCoord2f((float)(f?0:1),0.25f); glVertex2f(608+fire*0.75f,	57);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.5f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.5f,			99);
		glTexCoord2f((float)(f?1:0),0.5f); glVertex2f(32+fire*0.5f,			99);
		glTexCoord2f((float)(f?0:1),0.5f); glVertex2f(608+fire*0.5f,			99);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608+fire*0.25f,	141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32+fire*0.25f,		141);
		glTexCoord2f((float)(f?1:0),0.75f); glVertex2f(32+fire*0.25f,		141);
		glTexCoord2f((float)(f?0:1),0.75f); glVertex2f(608+fire*0.25f,	141);
		fire = sin((t+=sinus)*5)*10;
		glTexCoord2f((float)(f?0:1),1); glVertex2f(608,			183);
		glTexCoord2f((float)(f?1:0),1); glVertex2f(32,			183);
		glEnd();
	}
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D,imgSplash);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(64,25);
	glTexCoord2f(1,0); glVertex2f(576,25);
	glTexCoord2f(1,1); glVertex2f(576,153);
	glTexCoord2f(0,1); glVertex2f(64,153);
	glEnd();


	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(0.5f,1);
	mFont1.SetColor(1,1,1,1);

	float frame = (mTimer.GetTime()-fUIDelay)*15;
	float time = (mTimer.GetTime()-fUIDelay);
	if(time>14)
		time -= (float)((int)(time/14)*14);
	if(time<=5)
	{
		RenderSprite(sprPlr,8,100,190,64,64);
		if(frame>16)
			frame -= (float)((int)(frame/16)*16);
		RenderSprite(sprPowerDroid,(int)frame,90,240,64,64);
		if(frame>8)
			frame -= (float)((int)(frame/8)*8);
		RenderSprite(sprPowerUp,(int)frame+(8*4),90,310,42,42);
		RenderSprite(sprPowerUp,(int)frame+(8*5),95,360,32,32);
		RenderSprite(sprPowerUp,(int)frame+(8*7),95,400,32,32);
		RenderSprite(sprPowerUp,(int)frame+(8*6),95,440,32,32);

		mFont1.Print("PLAYER SHIP: DUAL MODES OF FIRE",160,190);
		mFont1.Print("TAP PRIMARY FOR PULSE, HOLD FOR CHARGED",160,210);
		mFont1.Print("KILLED POWERDROIDS RELEASE POWERUPS",160,250);
		mFont1.Print("SPEED UP - TOO MANY IS NOT A GOOD THING",160,310);
		mFont1.Print("HOMING MISSILE",160,360);
		mFont1.Print("CLUSTER BOMB",160,400);
		mFont1.Print("BIT UPGRADE - MAXIMUM 2 PER SHIP",160,440);
	}
	else if(time>5)
	{
		if(frame>16)
			frame -= (float)((int)(frame/16)*16);
		RenderRotatedSprite(sprOrb1,(int)frame+(16),110,190,20,20,-90);
		if(frame>8)
			frame -= (float)((int)(frame/8)*8);
		RenderSprite(sprPowerUp,(int)frame+(8*0),90,220,32,32);
		RenderSprite(sprPowerUp,(int)frame+(8*1),90,260,32,32);
		RenderSprite(sprPowerUp,(int)frame+(8*2),90,300,32,32);
		RenderSprite(sprPowerUp,(int)frame+(8*3),90,340,32,32);

		mFont1.Print("SOME POWERUPS GRANT AN ORB WHICH IS ATTACHED",150,170);
		mFont1.Print("TO THE PLAYER SHIP:",160,190);
		mFont1.Print("PLASMA - WEAK BUT GOOD SPREAD",160,220);
		mFont1.Print("RAIL - STRONG BUT NEEDS TO BE CHARGED",160,260);
		mFont1.Print("PULSE - SUSTAINED FIREPOWER MUST BE CHARGED",160,300);
		mFont1.Print("MAGMA - MEDIUM POWER, FAST RATE OF FIRE",160,340);
		mFont1.Print("BUT ALSO UPGRADES ANY CURRENT BITS",160,365);
		mFont1.Print("THE ORB IS ATTACHED TO THE SHIP BY AN ELECTRICAL CHAIN",90,400);
		mFont1.Print("WHILE SECONDARY IS DEPRESSED, THE ORB CAN BE ROTATED",90,420);
		mFont1.Print("ABOUT THE SHIP. WHEN RELEASED IT WILL LOCK",90,440);

	}
}
