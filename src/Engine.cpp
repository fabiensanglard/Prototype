#include "engine.h"
//#define PROFILE_TIME 100//set your profile time here (if you want to profile for specific set length)
//================================================================================================//
						/***********************
						** Engine constructor **	
						************************/
//================================================================================================//
Engine *gpEngine;
Engine::Engine(int width, int height, bool fscreen, char* winName)
{
	gpEngine = this;
	//registering profiler funcs
	PROFILE_REG_SLICE("ENGINE_Pump");
	PROFILE_REG_SLICE("ENGINE_Update");
	PROFILE_REG_SLICE("ENGINE_Render");
	//basic vars
	bQuit = false;
	bPaused = false;
	winWidth = width;
	winHeight = height;
	int vsync=0, bpp=32, log=1, joystick=1;
	int fullscreen = fscreen?1:0;
	int renderTargets=1;
	int FPS=0;
	int scanlines = 1;
	gSerializer.PutComment("Engine.cfg","[Display Properties]");
/*	gSerializer.ReadVariable("Engine.cfg","winWidth",winWidth);
	gSerializer.ReadVariable("Engine.cfg","winHeight",winHeight);
	gSerializer.ReadVariable("Engine.cfg","bpp",bpp);
*/	gSerializer.ReadVariable("Engine.cfg","Vsync",vsync);
	gSerializer.ReadVariable("Engine.cfg","PBuffers",renderTargets);
	gSerializer.ReadVariable("Engine.cfg","ShowFPS",FPS);
	gSerializer.ReadVariable("Engine.cfg","Fullscreen",fullscreen);
	gSerializer.ReadVariable("Engine.cfg","Scanlines",scanlines);
	gSerializer.PutComment("Engine.cfg","[Engine Properties]");
	gSerializer.ReadVariable("Engine.cfg","Keeplog",log);
	gSerializer.ReadVariable("Engine.cfg","Joystick",joystick);
	FPS?bShowFps = true: bShowFps= false;
	scanlines?bScanlines=true:bScanlines=false;
	renderTargets
		?bRenderTargetSupport=true
		:bRenderTargetSupport=false;
	gLog.SetLogState(log);
	fscreen = fullscreen?true:false;

	//init
	if(!UTIL_SDL::InitSDL(winName,winWidth,winHeight,bpp,vsync?true:false,fscreen))
		return;
	InitializeRenderTargets();

	//init joystick support
	if(joystick)
	{
		if(UTIL_SDL::InitJoystick())
		{
			bJoystickSupported = true;
			oldPadAxisH=oldPadAxisV=0;
		}
		else
			bJoystickSupported = false;
	}
	else
		bJoystickSupported = false;
	InitializeKeys();

	InitDevil();
	InitFmod();

	LoadScores();
	//InitPhysics();
	iStage = 1;
	bPauseScrolling = false;
	if(!LoadConstantData())
		return;

	//Display Loading message
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);
	glColor4f(1,1,1,1);
	mFont1.SetColor(0,1,1,1);
	mFont1.SetAlignment(ALIGN_LEFT);
	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(0.5f,1.0f);
	mFont1.Print("LOADING...",550,430);
	SDL_GL_SwapBuffers();

	fGameDifficulty = 3;//easy
	float delayStartup = mTimer.GetTime()+2;
	while(mTimer.GetTime()<delayStartup)
	{
		mTimer.Update();
	}
	
	curDemo=1;
	bDemo = false;
	mWeaponCheats = false;
	bHyperMode = false;
	bWimpMode = false;
	bBossFight = false;
	bCompileWithEditor = false;
	StartFrontEnd();
	SetState(&Engine::UpdateIntro,&Engine::RenderIntro);

	//start rolling
	Pump();

	//output a profiler log
	PROFILE_LOG_FINAL_STATS;
}
//================================================================================================//
						/**********************
						** Engine destructor **	
						***********************/
//================================================================================================//
Engine::~Engine(void)
{
	gLog.OutPut("\n[Engine Shutdown]\n");
	ilShutDown();
	FSOUND_Close();  
	SDL_Quit();
	SaveScores();
	gLog.OutPut("Complete...\n\n");
}
//================================================================================================//
						/*********************
						** Initialize Devil **	
						**********************/
//================================================================================================//
void Engine::InitDevil()
{
	gLog.OutPut("\n[Initializing Devil]\n");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	gLog.OutPut("Complete...\n\n");
}
//================================================================================================//
						/********************
						** Initialize FMOD **	
						*********************/
//================================================================================================//
void Engine::InitFmod()
{
	gLog.OutPut("\n[Initializing FMOD]\n");
	FSOUND_Init(44100, 32, 0);
	gLog.OutPut("Complete...\n\n");

}
//================================================================================================//
						/*********************
						** Engine Main-Loop **	
						**********************/
//================================================================================================//
void Engine::Pump()
{
	char FPS[16];
	int tick=0,oldtick=0;
	float accumulator=0;
	SDL_Event event;
	while(!bQuit)
	{
//	gLog.OutPut("CorePump START\n");

PROFILE_FRAME_START;
PROFILE_START_SLICE("ENGINE_Pump");
#ifdef PROFILE_TIME
	if(mTimer.GetTime()>=PROFILE_TIME)
		bQuit = true;
#endif
		mTimer.Update();
		if(bPaused)//ugly hack to fix time related code breaking
			mTimer.m_fTime -= mTimer.GetDelta();

		accumulator+=mTimer.GetDelta();
		while(accumulator>=(bHyperMode? 0.01f:TIME_STEP))
		{
			while(SDL_PollEvent(&event)>0)
			{

				switch(event.type)
				{
					case SDL_KEYDOWN:
					KeyStateAlltered(event.key.keysym.sym,true);
					break;
					case SDL_KEYUP:
					KeyStateAlltered(event.key.keysym.sym,false);
					break;
					case SDL_MOUSEBUTTONDOWN:
					KeyStateAlltered(event.button.button, true);
					break;
					case SDL_MOUSEBUTTONUP:
					KeyStateAlltered(event.button.button, false);
					break;
					//joystick buttons
					case SDL_JOYBUTTONDOWN:
						{
							if(!bJoystickSupported)
								break;
							int b = event.jbutton.button;
							if(b<20)
								KeyStateAlltered(JOYSTICK_BUTTON+b, true);				
						}
						break;
					case SDL_JOYBUTTONUP:
						{
							if(!bJoystickSupported)
								break;
							int b = event.jbutton.button;
							if(b<20)
								KeyStateAlltered(JOYSTICK_BUTTON+b, false);				
						}
						break;
					//joystick motion
					case SDL_JOYAXISMOTION:
						if(!bJoystickSupported)
							break;
						CheckJoystickMotion(event);
						break;

					case SDL_QUIT:
					bQuit = true;
					break;
				}
			}
PROFILE_START_SLICE("ENGINE_Update");
			if(mbTimedState)
			{
				if(mTimer.GetTime()>=mfStateTime)
				{
					SetState(pTimedUpdate,pTimedRender);
					mbTimedState = false;
					if(pCallBack)
						(this->*pCallBack)();
					iOption = 0;
				}
			}
			if(this->pUpdate)
				(this->*pUpdate)((float)(TIME_STEP));
PROFILE_END_SLICE("ENGINE_Update");

			KeysPostUpdate();
			accumulator-=(bHyperMode? 0.01f:TIME_STEP);
		}
	const float interp = accumulator/TIME_STEP;
PROFILE_START_SLICE("ENGINE_Render");
	if(!bPaused)
	{
		if(this->pRender)
			(this->*pRender)(interp);
	}
	else
	{
		if(this->pRender)
			(this->*pRender)(0.0f);
	}
	//draw FPS, comment out after
	if(bShowFps && mTimer.GetTime()>5)
	{
		sprintf(FPS,"FPS: %i",(int)mTimer.GetFPS());
		mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		mFont1.SetScale(0.5f,1);
		mFont1.SetAlignment(ALIGN_LEFT);
		mFont1.SetColor(1,1,1,1);
		mFont1.Print(FPS,540,16);
	}
PROFILE_END_SLICE("ENGINE_Render");
	if(bScanlines)
		RenderScanLines();
	SDL_GL_SwapBuffers();

PROFILE_END_SLICE("ENGINE_Pump");
PROFILE_FRAME_END;
//		gLog.OutPut("CorePump END\n");
	}
}
//================================================================================================//
						/****************************
						** Initialize default keys **	
						*****************************/
//================================================================================================//
void Engine::InitializeKeys()
{
	mKeys[0].PrimaryKey = SDLK_UP;
	strcpy(mKeys[0].KeyName,"UP");
	mKeys[1].PrimaryKey = SDLK_DOWN;
	strcpy(mKeys[1].KeyName,"DOWN");
	mKeys[2].PrimaryKey = SDLK_RIGHT;
	strcpy(mKeys[2].KeyName,"RIGHT");
	mKeys[3].PrimaryKey = SDLK_LEFT;
	strcpy(mKeys[3].KeyName,"LEFT");
	mKeys[4].PrimaryKey = SDLK_z;
	strcpy(mKeys[4].KeyName,"ATTACK");
	mKeys[5].PrimaryKey = SDLK_x;
	strcpy(mKeys[5].KeyName,"ATTACK2");
	if(bJoystickSupported)
	{
		mKeys[0].SecondaryKey = JOYSTICK_UP;
		mKeys[1].SecondaryKey = JOYSTICK_DOWN;
		mKeys[2].SecondaryKey = JOYSTICK_RIGHT;
		mKeys[3].SecondaryKey = JOYSTICK_LEFT;
		mKeys[4].SecondaryKey = JOYSTICK_BUTTON+1;
		mKeys[5].SecondaryKey = JOYSTICK_BUTTON;
	}
	else
	{
		mKeys[0].SecondaryKey = SDLK_w;
		mKeys[1].SecondaryKey = SDLK_s;
		mKeys[2].SecondaryKey = SDLK_d;
		mKeys[3].SecondaryKey = SDLK_a;
		mKeys[4].SecondaryKey = SDLK_RCTRL;
		mKeys[5].SecondaryKey = SDLK_SPACE;
	}

	gSerializer.PutComment("Controls.cfg","[Control Configuration]");
	for(int n=0; n<6;n++)
	{
		char sz[256];
		sprintf(sz,"%s_Primary",mKeys[n].KeyName);
		gSerializer.ReadVariable("Controls.cfg",sz,mKeys[n].PrimaryKey);
		sprintf(sz,"%s_Secondary",mKeys[n].KeyName);
		gSerializer.ReadVariable("Controls.cfg",sz,mKeys[n].SecondaryKey);
	}
}
//================================================================================================//
						/********************
						** Configure a Key **	
						*********************/
//================================================================================================//
void Engine::ConfigureKey(int keyIndex, bool set, int keyPressed)
{
	set?
		mKeys[keyIndex].PrimaryKey = keyPressed:
		mKeys[keyIndex].SecondaryKey = keyPressed;
}
//================================================================================================//
						/**************************
						** Check joystick motion **	
						***************************/
//================================================================================================//
void Engine::CheckJoystickMotion(SDL_Event event)
{
//	gLog.OutPut("CheckJoystick START\n");

	if(event.jaxis.axis == 0)//horizontal?
	{
		int axis;
		if(event.jaxis.value<0)
			axis=event.jaxis.value/(32768/2);
		else
			axis=event.jaxis.value/(32767/2);
		
		if(axis!=oldPadAxisH)
		{
			if(oldPadAxisH<0)
				KeyStateAlltered(JOYSTICK_LEFT, false);
			if(oldPadAxisH>0)
				KeyStateAlltered(JOYSTICK_RIGHT, false);
		}
		if(axis<0)
			KeyStateAlltered(JOYSTICK_LEFT, true);
		if(axis>0)
			KeyStateAlltered(JOYSTICK_RIGHT, true);
		oldPadAxisH = axis;
	}
	if(event.jaxis.axis == 1)//Vertical?
	{
		int axis;
		if(event.jaxis.value<0)
			axis=event.jaxis.value/(32768/2);
		else
			axis=event.jaxis.value/(32767/2);
		
		if(axis!=oldPadAxisV)
		{
			if(oldPadAxisV<0)
				KeyStateAlltered(JOYSTICK_UP, false);
			if(oldPadAxisV>0)
				KeyStateAlltered(JOYSTICK_DOWN, false);
		}
		if(axis<0)
			KeyStateAlltered(JOYSTICK_UP, true);
		if(axis>0)
			KeyStateAlltered(JOYSTICK_DOWN, true);
		oldPadAxisV = axis;
	}
//	gLog.OutPut("CheckJoystick END\n");
}
//================================================================================================//
						/*************************
						** Keystate has changed **	
						**************************/
//================================================================================================//
void Engine::KeyStateAlltered(int key,bool state)
{
//	gLog.OutPut("KeyStateAlltered START\n");

	//configure key mode
	if(bConfigureKey)
	{
		if(state==true)
		{
			ConfigureKey(iConfigKey,bConfigPrimary,key);
			bConfigureKey = false;
			return;
		}
	}

	//configurable keys
	for(int n=0;n<6;n++)
	{
		if(key == mKeys[n].PrimaryKey || key == mKeys[n].SecondaryKey)
		{
			mButtons[n].Update(state);
			aKeyPressed = true;
			break;
		}
	}

	//fixed keys
	switch(key)
	{
	case SDLK_ESCAPE:
		KEY_QUIT.Update(state);
		if(state)
		{
			if(!bInGame)
				bQuit = true;
			else
			{
				bWimpMode = false;
				bHyperMode = false;
				bBossFight = false;
				StartFrontEnd();
				SetState(&Engine::UpdateSplash,&Engine::RenderSplash);
				SetStateTimed(&Engine::UpdateSplash,&Engine::RenderSplash,0);
				mFade.StartFadeIn(mTimer.GetTime(),2,0.5f);
				fUIDelay = mTimer.GetTime();
				iOption = 0;
			}
		}
		break;
	case SDLK_PAUSE:
		KEY_PAUSE.Update(state);
		break;

	//UI keys
	case SDLK_UP:
		KEY_UI_UP.Update(state);
		break;
	case SDLK_DOWN:
		KEY_UI_DOWN.Update(state);
		break;
	case SDLK_RIGHT:
		KEY_UI_RIGHT.Update(state);
		break;
	case SDLK_LEFT:
		KEY_UI_LEFT.Update(state);
		break;
	case SDLK_LCTRL:
	case SDLK_RETURN:
		KEY_UI_SELECT.Update(state);
		break;
	case SDLK_SPACE:
		KEY_UI_SELECT2.Update(state);
		break;

	//cheat stuff
	case SDLK_m:
		KEY_M.Update(state);
		break;
	case SDLK_a:
		KEY_A.Update(state);
		break;
	case SDLK_r:
		KEY_R.Update(state);
		break;
	case SDLK_y:
		KEY_Y.Update(state);
		break;
	//editor stuff
/*	case SDLK_TAB:
		if(state)
		{
			bUseEditor = true;
			bShowTools = !bShowTools;
			bPlayBackMode = false;
			bShowTileTool = false;
			bShowEntityTool = false;
			bShowAnimProps = false;
			bPlaceAnim = false;
		}		
		break;

	case SDLK_1:
	case SDLK_2:
	case SDLK_3:
		if(state)
			iEditLayer = key-48;
		break;

	case SDLK_F1:
		if(state)
		{
			bUseEditor = !bUseEditor;
			bPlayBackMode = !bPlayBackMode;
			if(!bUseEditor)
			{
				bPlaceEntity =false;
				bShowEntityTool = false;
				bShowEntityProps = false;
				bShowAnimProps = false;
				bPlaceAnim = false;
				InitializeGame();
				Scroll = (float)(iEdScroll*32);
				InitializeGenerators();
				mPlayer.iSpawnOffset = iEdScroll*32;
				mPlayer.Spawn(Vec2((float)(iEdScroll*32),180));
				mPlayer.RegisterControl(mButtons);
			}
		}
		break;

	case SDLK_F11:
		if(state)
			SaveMap();
		break;

	case SDLK_PAGEUP:
		if(state)
		{
		iEdScroll+=20;
		EdPos.x+=20;
		if(iEdScroll>379)
			iEdScroll=379;
		if(EdPos.x>399)
			EdPos.x=399;
		}
		break;
	case SDLK_PAGEDOWN:
		if(state)
		{
		iEdScroll-=20;
		EdPos.x-=20;
		if(iEdScroll<0)
			iEdScroll=0;
		if(EdPos.x<0)
			EdPos.x=0;
		}
		break;
/*	case SDLK_F5:
		if(state)
			int hello=1;
		break;*/
	}
//	gLog.OutPut("KeyStateAlltered END\n");
	
}
//================================================================================================//
						/*************************
						** Keyboard Post-Update **	
						**************************/
//================================================================================================//
void Engine::KeysPostUpdate()
{
//	gLog.OutPut("KeyPostUpdate START\n");
	if(pRecordEnt)
	{
		bool bKeyFound=false;
		for(int n=0;n<6;n++)
		{
			if(mButtons[n].state != mPrevButtons[n].state)
			{
				if(pRecordEnt->ClientControlled && !bKeyFound)
					pRecordEnt->RecordKeyState();
				bKeyFound = true;
			}
			mPrevButtons[n] = mButtons[n];
		}
	}
	KEY_QUIT.PostUpdate();
	KEY_PAUSE.PostUpdate();

	KEY_M.PostUpdate();
	KEY_A.PostUpdate();
	KEY_R.PostUpdate();
	KEY_Y.PostUpdate();

	KEY_UI_UP.PostUpdate();
	KEY_UI_DOWN.PostUpdate();
	KEY_UI_RIGHT.PostUpdate();
	KEY_UI_LEFT.PostUpdate();
	KEY_UI_SELECT.PostUpdate();
	KEY_UI_SELECT2.PostUpdate();

	for(int n=0; n<6 ;n++)
	{
		mButtons[n].PostUpdate();
	}
//	gLog.OutPut("KeyPostUpdate END\n");
}
//================================================================================================//
						/*************************************************
						** Set the function pointer state of the engine **	
						**************************************************/
//================================================================================================//
void Engine::SetState(void(Engine::*update)(float),void(Engine::*render)(const float))
{
//	gLog.OutPut("SetState Called\n");
	pUpdate = update;
	pRender = render;
}
void Engine::SetStateTimed(void(Engine::*update)(float),void(Engine::*render)(const float),float duration)
{
//	gLog.OutPut("SetStateTimed Called\n");
	mfStateTime = mTimer.GetTime()+duration;
	pTimedUpdate = update;
	pTimedRender = render;
	pCallBack = NULL;
	mbTimedState = true;
}
void Engine::SetStateTimed(void(Engine::*update)(float),void(Engine::*render)(const float),float duration, void(Engine::*pCallMe)(void))
{
//	gLog.OutPut("SetStateTimed Called\n");
	mfStateTime = mTimer.GetTime()+duration;
	pTimedUpdate = update;
	pTimedRender = render;
	pCallBack = pCallMe;
	mbTimedState = true;
}//================================================================================================//
							/******************************
							** Check for pbuffer support **	
							******************************/
//================================================================================================//
void Engine::InitializeRenderTargets()
{
	if(!bRenderTargetSupport)
		return;


	if(!(pMainTarget = UTIL_SDL::Create_pBuffer(640,480)))
	{
		gLog.OutPut("Render Targets Not supported.\n");
		bRenderTargetSupport = false;
		return;
	}
	UTIL_SDL::Unlock_pBuffer(pMainTarget);
	
	if(!(p64x64Target = UTIL_SDL::Create_pBuffer(64,64)))
	{
		gLog.OutPut("Render Targets Not supported.\n");
		bRenderTargetSupport = false;
		return;
	}
	UTIL_SDL::Unlock_pBuffer(p64x64Target);
	

	gLog.OutPut("Render Targets supported.\n");
	//bRenderTargetSupport = false;

}
//================================================================================================//
						/*****************************************
						** Load data that doesnt need reloading **	
						******************************************/
//================================================================================================//
bool Engine::LoadConstantData()
{
	if(!mFont1.LoadFont("Data/Gfx/Fonts.psd","Data/Gfx/FontShadow.psd"))
		return false;
	if(!mFont2.LoadFont("Data/Gfx/Fonts2.psd","Data/Gfx/Fonts2Shadow.psd"))
		return false;
	if(!(imgUI=UTIL_GL::Image::LoadImage("Data/Gfx/GUI.psd",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgScanlines=UTIL_GL::Image::LoadImage("Data/Gfx/Scanlines.psd",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgHudBeam=UTIL_GL::Image::LoadImage("Data/Gfx/HudBeam.png",GL_NEAREST,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;

	unsigned int ID=0;
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/PlayerShip.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprPlr.Split(ID,512,512,128,128);

	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/ForceCharge.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprForceCharge.Split(ID,512,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Orb1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprOrb1.Split(ID,512,512,64,64);

	if(!(imgChain=UTIL_GL::Image::LoadImage("Data/Gfx/ChainParticle.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;

	//entity sprites - if things get too much overhead, we may have to only load those that are neccesary
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/LadyBird.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprLadyBird.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/turret1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprTurret.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/SuperTurret.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprSuperTurret.Split(ID,256,128,64,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/SerpentHead.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprSerpent.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/SerpentBody.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprSerpentBody.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/SerpentTail.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprSerpentTail.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Rocket.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprRocket.Split(ID,256,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/GroundTroop_Walk.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprRhinoWalk.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/GroundTroop_Turn.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprRhinoTurn.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/GroundTroop_Aim.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprRhinoAim.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/PowerDroid.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprPowerDroid.Split(ID,512,512,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Bug.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBug.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Starburst.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprStarBurst.Split(ID,256,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Fighter.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprFighter.Split(ID,512,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/RoboticTorso.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprRobot.Split(ID,512,256,128,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Thruster.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprThruster.Split(ID,256,128,64,64);
	if(!(imgBoss=UTIL_GL::Image::LoadImage("Data/Gfx/Boss.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgBossTube=UTIL_GL::Image::LoadImage("Data/Gfx/BossTube.psd",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgBossTubeSpec=UTIL_GL::Image::LoadImage("Data/Gfx/BossTubeSpec.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgBossTubeFluid=UTIL_GL::Image::LoadImage("Data/Gfx/BossTubeFluid.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgBossArm=UTIL_GL::Image::LoadImage("Data/Gfx/BossArm.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgBossPush=UTIL_GL::Image::LoadImage("Data/Gfx/BossPush.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BossEye1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBossEye1.Split(ID,128,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BossEye2.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBossEye2.Split(ID,256,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BossEye3.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBossEye3.Split(ID,64,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BossEye4.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBossEye4.Split(ID,256,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BossEye5.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBossEye5.Split(ID,128,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BossCharge.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBossCharge.Split(ID,512,256,64,64);
	if(!(imgBossBrain=UTIL_GL::Image::LoadImage("Data/Gfx/BossBrain.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgBossCell=UTIL_GL::Image::LoadImage("Data/Gfx/BossBrainCell.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BossSperm.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBossSperm.Split(ID,128,64,32,32);
//	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/SolarFlare.psd",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
//		return false;
//	sprSolarFlare.Split(ID,128,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/FireBug.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprFireBug.Split(ID,128,128,128,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/SpaceTrash.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprTrash.Split(ID,128,64,32,32);


	if(!(imgMidBoss=UTIL_GL::Image::LoadImage("Data/Gfx/MidBoss.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgMidBossSeg=UTIL_GL::Image::LoadImage("Data/Gfx/MidBossSeg.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgMidBossCore=UTIL_GL::Image::LoadImage("Data/Gfx/MidBossCore.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgMidBossGun=UTIL_GL::Image::LoadImage("Data/Gfx/MidBossGun.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	if(!(imgMidBossFoot=UTIL_GL::Image::LoadImage("Data/Gfx/MidBossFoot.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;

	//bullets
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BulletSet1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBulletSet1.Split(ID,64,128,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Forceblast.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBulletForce.Split(ID,128,64,128,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/GlowBullets.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprGlowBullets.Split(ID,64,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BulletHit.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBulletHit.Split(ID,128,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BulletHit2.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBulletHit2.Split(ID,128,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BulletHit3.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBulletHit3.Split(ID,128,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/ClusterBomb.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprCluster.Split(ID,128,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Missile.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprMissile.Split(ID,256,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/FireBall.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprFireBall.Split(ID,512,512,128,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/HomingMissile.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprHomingMissile.Split(ID,256,256,64,64);

	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/PowerUp1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprPowerUp.Split(ID,512,512,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BitUpgrade.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBit.Split(ID,128,64,32,32);

	//explosions
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/ExplosionSmall.psd",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprExplosionSmall.Split(ID,1024,512,128,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Explosion2.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprExplosion2.Split(ID,512,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Implosion.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprImplosion.Split(ID,512,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/BlueExplosion.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBlueExplosion.Split(ID,512,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/SmokePuff.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprSmokePuff.Split(ID,128,128,32,32);

	//sprite effects
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/PlayerSpeed.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprSpeedBoost.Split(ID,512,256,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Anim_back1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprAnimBack1.Split(ID,256,128,64,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Anim_back2.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprAnimBack2.Split(ID,128,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Anim_Fore1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprAnimFore1.Split(ID,128,128,64,64);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Anim_Fore2.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprAnimFore2.Split(ID,128,128,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Anim_Fore3.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprAnimFore3.Split(ID,256,128,64,32);
	//breakable scenery
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/DamageTile1.png",GL_LINEAR,GL_LINEAR,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprBreak1.Split(ID,256,128,64,128);

	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/water.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprWater.Split(ID,32,32,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/watersplash.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprWaterSplash.Split(ID,64,64,32,32);

	//splash related
	if(!(imgSplash=UTIL_GL::Image::LoadImage("Data/Gfx/Splash.psd",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgSplashFire=UTIL_GL::Image::LoadImage("Data/Gfx/Fire.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgSplashBack=UTIL_GL::Image::LoadImage("Data/Gfx/SplashBack.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgStage1=UTIL_GL::Image::LoadImage("Data/Gfx/Stage1.psd",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgGameOver=UTIL_GL::Image::LoadImage("Data/Gfx/GameOver.psd",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgProtoTypeMk1=UTIL_GL::Image::LoadImage("Data/Gfx/Prototypemk1.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(imgCongrats=UTIL_GL::Image::LoadImage("Data/Gfx/Congrats.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/GameOverSeq.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprGameOver.Split(ID,512,512,128,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/HiScoreSeq.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprHiScore.Split(ID,512,512,128,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/LiftOff2.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprIntro.Split(ID,512,512,128,128);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/LiftOff.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprIntro2.Split(ID,512,256,128,128);


	//level tiles
	char sz[256];
	sprintf(sz,"Data/Gfx/Stage%iLayer%i.png",iStage,1);
	if(!(ID=UTIL_GL::Image::LoadImage(sz,GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprMapTiles.Split(ID,1024,1024,32,32);


	//sounds
	if(!smpLaser1.Load("Data/Sound/Laser.wav",false))
		return false;
	if(!smpLaser2.Load("Data/Sound/Laser2.wav",false))
		return false;
	if(!smpLaser3.Load("Data/Sound/Laser3.wav",false))
		return false;
	if(!smpLaser4.Load("Data/Sound/Laser4.wav",false))
		return false;
	if(!smpLaser5.Load("Data/Sound/Laser5.wav",false))
		return false;
	if(!smpBigLaser1.Load("Data/Sound/BigLaser1.wav",false))
		return false;
	if(!smpForceShot.Load("Data/Sound/Force.wav",false))
		return false;
	if(!smpForceCharge.Load("Data/Sound/ForceCharge.wav",true))
		return false;
	if(!smpDeflect.Load("Data/Sound/Deflection.wav",false))
		return false;
	if(!smpSmallExplode.Load("Data/Sound/SmallExplosion.wav",false))
		return false;
	if(!smpMedExplode.Load("Data/Sound/MedExplosion.wav",false))
		return false;
	if(!smpLargeExplode.Load("Data/Sound/LargeExplosion.wav",false))
		return false;
	if(!smpLargeExplode2.Load("Data/Sound/LargeExplosion2.wav",false))
		return false;
	if(!smpPickup.Load("Data/Sound/Pickup.wav",false))
		return false;
	if(!smpOrbSpawn.Load("Data/Sound/OrbSpawn.wav",false))
		return false;
	if(!smpLaser6.Load("Data/Sound/Laser6.wav",false))
		return false;
	if(!smpBigLaser2.Load("Data/Sound/BigLaser2.wav",false))
		return false;
	if(!smpLaser7.Load("Data/Sound/Laser7.wav",false))
		return false;

	if(!smpUIselect.Load("Data/Sound/UI_select.wav",false))
		return false;
	if(!smpUImove.Load("Data/Sound/UI_move.wav",false))
		return false;

	if(!stmStage.Load("Data/Sound/Stage1.ogg",true))
		return false;
	if(!stmBoss.Load("Data/Sound/Boss.ogg",true))
		return false;

	gLog.OutPut("Total Texture Mem usage:");
	sprintf(sz,"%i\n",(int)(textureMem/1024)/1024);
	gLog.OutPut(sz);
	return true;
}
//================================================================================================//
						/******************************
						** Load editor specific data **	
						******************************/
//================================================================================================//
bool Engine::LoadEditorData()
{
	unsigned int ID=0;
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/ED_ico.png",GL_NEAREST,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE)))
		return false;
	sprIcons.Split(ID,64,64,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Entities.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprEntities.Split(ID,512,512,32,32);
	if(!(ID=UTIL_GL::Image::LoadImage("Data/Gfx/Anims.png",GL_LINEAR,GL_LINEAR,GL_REPEAT,GL_REPEAT)))
		return false;
	sprAnims.Split(ID,512,512,32,32);

	return true;
}
//================================================================================================//
						/***************************
						** Load map specific data **	
						****************************/
//================================================================================================//
bool Engine::LoadMapData()
{
	char sz[256];
	ClearMapArrays();
	ClearMapEntities();

	CFileIO fIO;
	sprintf(sz,"Data/Maps/Stage%i.stg",iStage);
	if(!fIO.GetFile(sz,"rb"))
		return false;
	int x,y;
	//read block data
	for(x=0;x<400;x++)
	{
		for(y=0;y<15;y++)
		{
			fIO.ReadBinary(&mTilesLayer1[x][y].ID,1);
			fIO.ReadBinary(&mTilesLayer1[x][y].properties,1);
			fIO.ReadBinary(&mTilesLayer2[x][y].ID,1);
			fIO.ReadBinary(&mTilesLayer2[x][y].properties,1);
			fIO.ReadBinary(&mTilesLayer3[x][y].ID,1);
			fIO.ReadBinary(&mTilesLayer3[x][y].properties,1);
		}
	}
	//read entities
	int numEnt,n;
	fIO.ReadBinary(&numEnt,1);
	for(n=0;n<numEnt;n++)
	{
		Generator g;
		g.LoadFromFile(fIO);
		mGenerators.push_back(g);
	}
	fIO.ReadBinary(&numEnt,1);
	for(n=0;n<numEnt;n++)
	{
		AnimGenerator g;
		g.LoadFromFile(fIO);
		mAnimGenerators.push_back(g);
	}
	return true;
}
//================================================================================================//
						/***************************
						** load map specific ents **	
						***************************/
//================================================================================================//
bool Engine::ClearMapEntities()
{
	mGenerators.clear();
	mAnimGenerators.clear();
	mBullets.clear();
	mPlayerBullets.clear();
	mExplosions.clear();
	return true;
}
//================================================================================================//
						/***************************
						** save map specific data **	
						****************************/
//================================================================================================//
void Engine::SaveMap()
{
	CFileIO fIO;
	char sz[256];
	sprintf(sz,"Data/Maps/Stage%i.stg",iStage);
	if(!fIO.GetFile(sz,"wb"))
		return;
	int x,y;
	//write block data
	for(x=0;x<400;x++)
	{
		for(y=0;y<15;y++)
		{
			fIO.WriteBinary(&mTilesLayer1[x][y].ID,1);
			fIO.WriteBinary(&mTilesLayer1[x][y].properties,1);
			fIO.WriteBinary(&mTilesLayer2[x][y].ID,1);
			fIO.WriteBinary(&mTilesLayer2[x][y].properties,1);
			fIO.WriteBinary(&mTilesLayer3[x][y].ID,1);
			fIO.WriteBinary(&mTilesLayer3[x][y].properties,1);
		}
	}
	//write generators
	int n = (int)mGenerators.size();
	fIO.WriteBinary(&n,1);
	if(n>0)
	{
		list<Generator>::iterator g;
		for(g = mGenerators.begin(); g != mGenerators.end(); g++)
		{
			(*g).WriteToFile(fIO);
		}
	}
	n = (int)mAnimGenerators.size();
	fIO.WriteBinary(&n,1);
	if(n>0)
	{
		list<AnimGenerator>::iterator g;
		for(g = mAnimGenerators.begin(); g != mAnimGenerators.end(); g++)
		{
			(*g).WriteToFile(fIO);
		}
	}

	fIO.ReleaseFile();
}
//================================================================================================//
						/*******************************
						** Clear up map specific data **	
						********************************/
//================================================================================================//
void Engine::CleanUpMapData()
{
	if(sprMapTiles.imgID>0)//free up texture space
		glDeleteTextures(1,&sprMapTiles.imgID);
}
//================================================================================================//
						/***************
						** Extensions **	
						****************/
//================================================================================================//

/*bool Core::InitExtensions(void)
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("Failed To Init GLEW!\n");
		return false;
	}
	printf("GLEW Initialized.\n");
	if(!glewIsSupported("GL_ARB_imaging"))
	{
		printf("Required Extension not found: GL_ARB_imaging!\n");
		printf("Min requirement Geforce256 or equivalent,\n");
		printf("Will result to low quality backgrounds.\n");
		bLowSpec = true;
		return true;
	}

	printf("Extensions Init Ok.\n");
	return true;
}*/

void Engine::LoadScores()
{
	iHiScore[0]= 10000;
	iHiScore[1]= 9000;
	iHiScore[2]= 8000;
	iHiScore[3]= 7000;
	iHiScore[4]= 6000;
	iHiScore[5]= 5000;
	iHiScore[6]= 4000;
	iHiScore[7]= 3000;
	iHiScore[8]= 2000;
	iHiScore[9]= 1000;
	string s1("HOLD");
	string s2("DOWN");
	string s3("KEYS");
	string s4("MARY");
	string s5("ON");
	string s6("MAIN");
	string s7("PAGE");
	string s8("FOR");
	string s9("CHEAT");
	string s10("*****");

	gSerializer.ReadVariable("score.dat","1a",iHiScore[0]);
	gSerializer.ReadVariable("score.dat","1b",s1);
	gSerializer.ReadVariable("score.dat","2a",iHiScore[1]);
	gSerializer.ReadVariable("score.dat","2b",s2);
	gSerializer.ReadVariable("score.dat","3a",iHiScore[2]);
	gSerializer.ReadVariable("score.dat","3b",s3);
	gSerializer.ReadVariable("score.dat","4a",iHiScore[3]);
	gSerializer.ReadVariable("score.dat","4b",s4);
	gSerializer.ReadVariable("score.dat","5a",iHiScore[4]);
	gSerializer.ReadVariable("score.dat","5b",s5);
	gSerializer.ReadVariable("score.dat","6a",iHiScore[5]);
	gSerializer.ReadVariable("score.dat","6b",s6);
	gSerializer.ReadVariable("score.dat","7a",iHiScore[6]);
	gSerializer.ReadVariable("score.dat","7b",s7);
	gSerializer.ReadVariable("score.dat","8a",iHiScore[7]);
	gSerializer.ReadVariable("score.dat","8b",s8);
	gSerializer.ReadVariable("score.dat","9a",iHiScore[8]);
	gSerializer.ReadVariable("score.dat","9b",s9);
	gSerializer.ReadVariable("score.dat","10a",iHiScore[9]);
	gSerializer.ReadVariable("score.dat","10b",s10);
	strcpy(szHiScore[0],s1.c_str());
	strcpy(szHiScore[1],s2.c_str());
	strcpy(szHiScore[2],s3.c_str());
	strcpy(szHiScore[3],s4.c_str());
	strcpy(szHiScore[4],s5.c_str());
	strcpy(szHiScore[5],s6.c_str());
	strcpy(szHiScore[6],s7.c_str());
	strcpy(szHiScore[7],s8.c_str());
	strcpy(szHiScore[8],s9.c_str());
	strcpy(szHiScore[9],s10.c_str());
}
void Engine::SaveScores()
{
	DeleteFile("score.dat");
	gSerializer.ReadVariable("score.dat","1a",iHiScore[0]);
	gSerializer.ReadVariable("score.dat","1b",string(szHiScore[0]));
	gSerializer.ReadVariable("score.dat","2a",iHiScore[1]);
	gSerializer.ReadVariable("score.dat","2b",string(szHiScore[1]));
	gSerializer.ReadVariable("score.dat","3a",iHiScore[2]);
	gSerializer.ReadVariable("score.dat","3b",string(szHiScore[2]));
	gSerializer.ReadVariable("score.dat","4a",iHiScore[3]);
	gSerializer.ReadVariable("score.dat","4b",string(szHiScore[3]));
	gSerializer.ReadVariable("score.dat","5a",iHiScore[4]);
	gSerializer.ReadVariable("score.dat","5b",string(szHiScore[4]));
	gSerializer.ReadVariable("score.dat","6a",iHiScore[5]);
	gSerializer.ReadVariable("score.dat","6b",string(szHiScore[5]));
	gSerializer.ReadVariable("score.dat","7a",iHiScore[6]);
	gSerializer.ReadVariable("score.dat","7b",string(szHiScore[6]));
	gSerializer.ReadVariable("score.dat","8a",iHiScore[7]);
	gSerializer.ReadVariable("score.dat","8b",string(szHiScore[7]));
	gSerializer.ReadVariable("score.dat","9a",iHiScore[8]);
	gSerializer.ReadVariable("score.dat","9b",string(szHiScore[8]));
	gSerializer.ReadVariable("score.dat","10a",iHiScore[9]);
	gSerializer.ReadVariable("score.dat","10b",string(szHiScore[9]));
}

void Engine::RenderScanLines()
{
	UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	UTIL_GL::BindTexture(GL_TEXTURE_2D, imgScanlines);
	glColor4f(1,1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(0,0);
	glTexCoord2f(1,0);
	glVertex2f(640,0);
	glTexCoord2f(1,20);
	glVertex2f(640,480);
	glTexCoord2f(0,20);
	glVertex2f(0,480);
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}