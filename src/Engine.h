#pragma once

#ifdef WIN32
    #include <windows.h>
    #include <sdl.h>
#else
    #include <SDL/SDL.h>
    #include <unistd.h>
    #include <cstring>
    #define DeleteFile ::unlink
    #define stricmp ::strcasecmp
    #define _TCHAR char
#define _tmain main
#endif

#include <fstream>

#include "Sound.h"
#include "UTIL_openGL.h"
#include "UTIL_SDL.h"
#include "UTIL_Misc.h"
#include "UTIL_Fade.h"
#include "SDL_KeyMap.h"
#include "Timer.h"
#include "Button.h"
#include "Serializer.h"
#include "Profiler.h"
#include "Font.h"
#include "Log.h"
#include "SpriteSheet.h"
//#include "Geo2D.h"
#include "Entity.h"
#include "Player.h"
#include "Tile.h"
#include "Generator.h"
#include "AnimGenerator.h"
#include "Turret.h"
#include "LadyBird.h"
#include "Serpent.h"
#include "Rocket.h"
#include "Rhino.h"
#include "PowerDroid.h"
#include "SuperTurret.h"
#include "Bug.h"
#include "BreakScenery.h"
#include "StarBurst.h"
#include "Fighter.h"
#include "MidBoss.h"
#include "Bullet.h"
#include "Explosion.h"
#include "Beam.h"
#include "SpriteEffect.h"
#include "Sound_Macros.h"
#include "CheckPoint.h"
#include "Demo.h"
#include "Boss.h"
#include "BrainBoss.h"
#include "TriggerSpeedUp.h"
#include "TriggerSlowDown.h"
#include "Fire.h"
#include "SolarFlare.h"
#include "fireBug.h"
#include "SpaceTrash.h"
#include <list>
using namespace std;




struct Key
{
	int PrimaryKey,SecondaryKey;
	char KeyName[32];
};

#define TIME_STEP 0.02f//50frames sec
#define SCROLL_SPEED 0.4f
class Engine
{
private:
	//basic variables of engine
	bool bQuit,bPaused;
	int winWidth,winHeight;
	bool bRenderTargetSupport;
	bool bShowFps;
	bool bScanlines;
	bool bHyperMode;//super fast mode lol
	bool bInGame;

	//initialization
	bool InitSDL(bool fscreen, char* winName, int vsync, int bpp);
	void InitDevil();
	void InitFmod();
	void InitializeRenderTargets();
	bool LoadConstantData();
	//void InitPhysics();

	void SetState(void(Engine::*update)(float),void(Engine::*render)(const float));
	void (Engine::*pUpdate)(float);
	void (Engine::*pRender)(const float interp);
	void SetStateTimed(void(Engine::*update)(float),void(Engine::*render)(const float),float duration);
	void (Engine::*pTimedUpdate)(float);
	void (Engine::*pTimedRender)(const float interp);
	void (Engine::*pCallBack)();
	void SetStateTimed(void(Engine::*update)(float),void(Engine::*render)(const float),float duration, void(Engine::*pCallMe)(void));
	float mfStateTime;//the time at which a request was called
	bool mbTimedState;

	void Pump();//main loop of engine


	// KEY RELATED VARIABLES AND FUNCTIONS
		bool aKeyPressed;//at least one key was pressed in this frame

		Button 	KEY_QUIT, KEY_PAUSE, KEY_UI_UP, KEY_UI_DOWN, KEY_UI_LEFT, KEY_UI_RIGHT, KEY_UI_SELECT, KEY_UI_SELECT2;
		Button  KEY_M, KEY_A, KEY_R, KEY_Y;
		Button mButtons[6];//game keys
		Button mPrevButtons[6];//previous state
		Key mKeys[6];//configurable keys

		bool bJoystickSupported;
		int oldPadAxisH,oldPadAxisV;
		bool bConfigureKey;//configure key mode
		int iConfigKey;//index into key array
		bool bConfigPrimary;//primary or secondary bind?

		void InitializeKeys();
		void CheckJoystickMotion(SDL_Event e);
		void ConfigureKey(int keyIndex, bool set, int keyPressed);
		void KeyStateAlltered(int key,bool state);
		void KeysPostUpdate();

	//FRONTEND RELATED
		int iOption;
		void UpdateSplash(float delta);
		void UpdateSplash2(float delta);
		void RenderSplash(const float interp);
		void UpdateConfig(float delta);
		void RenderConfig(const float interp);
		void UpdateGameOver(float delta);
		void RenderGameOver(const float interp);
		float fGameOverFrame;
		bool bGameOverFrame;
		void StartFadeIn();
		void StartFadeOut();
		void EndGameOver();
		void UpdateIntro(float delta);
		void RenderIntro(const float interp);
		void UpdateIntro2(float delta);
		void RenderIntro2(const float interp);
		void UpdateIntro3(float delta);
		void RenderIntro3(const float interp);
		void UpdateIntro4(float delta);
		void RenderIntro4(const float interp);
		void UpdateHiScores(float delta);
		void RenderHiScores(const float interp);
		void UpdateEnterHiScore(float delta);
		void RenderEnterHiScore(const float interp);
		int iScoreEntryDigit, iScoreEntryCharacter,iHiScoreOverWrite;
		float fUIDelay;
		float IntroTimer;
		void RenderScanLines();
		void UpdateCongratulations(float delta);
		void RenderCongratulations(const float interp);
		void UpdateHowToPlay(float delta);
		void RenderHowToPlay(const float interp);
		

	//UI
		void RenderUIPanel(float x, float y, float w, float h);

	//game related
		void StartGame();
		void UpdateGame(float delta);
		void RenderGame(float interp);
		void UpdateEntities();
		void RenderEntities(float interp);
		void UpdateBullets();
		void RenderBullets(const float interp);
		void CheckCollisions();
		void UpdateExplosions();
		void RenderExplosions(const float interp);
		void UpdateSpriteEffects();
		void RenderSpriteEffects(const float interp, int layer);
		int DisplayStageTicks;
		bool mWeaponCheats;
	//map related
		bool LoadMapData();
		void SaveMap();
		bool ClearMapEntities();
		void CleanUpMapData();
		void ClearMapArrays();

		void UpdateMap();
		void RenderMapLayer(int layer, float interp, bool Editing);
		void RenderWaterTransparent(const float interp);

	// render target related
		SDL_RenderTarget *pMainTarget, *p64x64Target;

		void BindMainContext();
		void FinalizeMainContext();
		void RenderMainContext();
		void DoPostEffects(float interp);
		void PostCharge(float interp);
		void PostWater(float interp);
	//HUD
		void RenderHud(float interp);
public:

	Engine(int width, int height, bool fscreen, const char* winName);
	~Engine(void);
	//components
	Timer	mTimer;
	Font	mFont1,mFont2;
	Fade	mFade;

	//generator related
		Entity* CreateEntity(int type);
		void GiveEntityToList(Entity* pEnt);
		void UpdateGenerators();	
		void InitializeGenerators();//get the generators ready for the current state
		void RemoveGeneratorsByEdit();

	// Game entities
		int iDelayGameTicks;//if this is greater than 0 the game is stalled
		float fForceChargeStart;	//the time at which the visual effect for charge distortion is started
		Player mPlayer;
		list<Entity*>	mGameEnts;
		list<Entity*>	mTopGameEnts;//this is used for game ents that must be above the scenery
		list<Generator>	mGenerators;//generators for the level (save reloading) - copy into locals on stage start
		list<Generator>	mLocalGenerators;//generators for the current state of game (pop off front as they become active)
		list<Generator>	mActiveGenerators;//when a generator becomes active push it into this list and update from there.
		list<Bullet> mPlayerBullets;//player only bullets
		list<Bullet> mBullets;//enemy and such bullets
		list<Beam> mBeams;//beams/lighting etc
		list<Explosion> mExplosions;
		list<SpriteEffect> mEffectsLayer1;
		list<SpriteEffect> mEffectsLayer2;
		list<SpriteEffect> mEffectsLayer3;
		list<AnimGenerator>	mAnimGenerators;//generators for the level (save reloading) - copy into locals on stage start
		list<AnimGenerator>	mLocalAnimGenerators;//generators for the current state of game (pop off front as they become active)
		void SpawnBullet(Bullet b, bool IsPlayers);
		void SpawnExplosion(SpriteSheet spr, Vec2 pos, float size, float duration, float rotation, bool glow);
		void SpawnBeam(vector<Vec2> p, int segs, int random, float color[4], float duration, float thickness);
		void SpawnSpriteEffect(int layer, Vec2 pos, Vec2 size, SpriteSheet sprite, float duration, bool glow, Vec2* target, bool playOnce);
		void InitializeGame();
		void StartGameOver();
		void StartEnterHiScore();
		void StartCompleted();
		void FadeInEndCompleted();
		void EndCompleted();
		bool bGameCompleted;
		float fMapSpeed;

		float fGameDifficulty;
	// map related
		int iStage;
		float Scroll, oScroll;
		Tile mTilesLayer1[400][15];
		Tile mTilesLayer2[400][15];
		Tile mTilesLayer3[400][15];
		bool bPauseScrolling;//for bosses and such

	//image variables
		unsigned int imgUI;
		unsigned int imgScanlines;
		SpriteSheet sprPlr;
		SpriteSheet sprMapTiles;
		SpriteSheet sprForceCharge;
		SpriteSheet sprOrb1;
		unsigned int imgChain;
		SpriteSheet sprLadyBird;
		SpriteSheet sprTurret;
		SpriteSheet sprSuperTurret;
		SpriteSheet sprRocket;
		SpriteSheet sprRhinoWalk;
		SpriteSheet sprRhinoTurn;
		SpriteSheet sprRhinoAim;
		SpriteSheet sprSerpent;
		SpriteSheet sprSerpentBody;
		SpriteSheet sprSerpentTail;
		SpriteSheet sprBug;
		SpriteSheet sprStarBurst;
		SpriteSheet sprFighter;
		SpriteSheet sprRobot;
		SpriteSheet sprPowerDroid;
		SpriteSheet sprThruster;
		SpriteSheet sprPowerUp;
		SpriteSheet sprBit;
		SpriteSheet sprBulletSet1;
		SpriteSheet sprMissile;
		SpriteSheet sprBulletForce;
		SpriteSheet sprGlowBullets;
		SpriteSheet sprCluster;
		SpriteSheet sprFireBall;
		SpriteSheet sprHomingMissile;
		SpriteSheet sprBulletHit;
		SpriteSheet sprBulletHit2;
		SpriteSheet sprBulletHit3;
		SpriteSheet sprExplosionSmall;
		SpriteSheet sprExplosion2;
		SpriteSheet sprImplosion;
		SpriteSheet sprBlueExplosion;
		SpriteSheet sprSmokePuff;
		SpriteSheet sprSpeedBoost;
		SpriteSheet sprWater;
		SpriteSheet sprWaterSplash;
		SpriteSheet sprSolarFlare;
		SpriteSheet sprFireBug;
		SpriteSheet sprTrash;
		unsigned int imgHudBeam;

		SpriteSheet sprAnimBack1;
		SpriteSheet sprAnimBack2;
		SpriteSheet sprAnimFore1;
		SpriteSheet sprAnimFore2;
		SpriteSheet sprAnimFore3;
		SpriteSheet sprBreak1;

		unsigned int imgBoss;
		unsigned int imgBossTube;
		unsigned int imgBossTubeSpec;
		unsigned int imgBossTubeFluid;
		unsigned int imgBossArm;
		unsigned int imgBossPush;
		SpriteSheet sprBossEye1;
		SpriteSheet sprBossEye2;
		SpriteSheet sprBossEye3;
		SpriteSheet sprBossEye4;
		SpriteSheet sprBossEye5;
		SpriteSheet sprBossCharge;

		unsigned int imgBossBrain;
		unsigned int imgBossCell;
		SpriteSheet sprBossSperm;

		unsigned int imgMidBoss;
		unsigned int imgMidBossSeg;
		unsigned int imgMidBossCore;
		unsigned int imgMidBossGun;
		unsigned int imgMidBossFoot;

		unsigned int imgSplash;
		unsigned int imgSplashFire;
		unsigned int imgSplashBack;
		unsigned int imgStage1;
		unsigned int imgGameOver;
		unsigned int imgProtoTypeMk1;
		unsigned int imgCongrats;
		SpriteSheet	sprGameOver;
		SpriteSheet sprIntro;
		SpriteSheet sprIntro2;
		SpriteSheet	sprHiScore;
	//sounds effects
		Sample	smpLaser1;
		Sample	smpLaser2;
		Sample	smpLaser3;
		Sample	smpLaser4;
		Sample	smpLaser5;
		Sample	smpBigLaser1;
		Sample	smpSmallExplode;
		Sample	smpMedExplode;
		Sample	smpLargeExplode;
		Sample	smpLargeExplode2;
		Sample	smpForceShot;
		Sample	smpForceCharge;
		Sample	smpDeflect;
		Sample  smpUIselect;
		Sample  smpUImove;
		Sample  smpPickup;
		Sample  smpOrbSpawn;
		Sample  smpLaser6;
		Sample  smpLaser7;
		Sample  smpBigLaser2;

		Stream  stmSplash;
		Stream  stmStage;
		Stream  stmBoss;
	//editor related
		bool bUseEditor;
		bool bShowTools;
		bool bShowTileTool;//show the tile selector
		bool bShowEntityTool;//this will hold different ents, depending on stage
		bool bPlaceEntity;//placing entity mode
		bool bShowEntityProps;//show the generator properties
		bool bShowAnimProps;
		bool bPlaceAnim;
		bool bPlayBackMode;
		bool bRecording;//if in record mode
		int iTileFrame;//currently selected tile
		int iTileFrameEnd;//so we can copy/paste sections of tiles
		unsigned char edTileProperties;
		int iEditMap;//which map to edit
		int iEditTool;//which tool to use
		int iEditLayer;
		int iEdScroll;//scroll offset when editing
		SpriteSheet sprIcons;//editor icons
		SpriteSheet sprEntities;//entity icons
		SpriteSheet sprAnims;//anim icons
		Vec2 EdPos;//where we are editing
		ControlledEntity* pRecordEnt;
		bool LoadEditorData();
		void UpdateEditor(float delta);
		void RenderEditor(const float interp);
		void StartEditor();
		void UpdateEntityProps();
		void RenderEntityProps();
		SpriteEffect GetAnimForEditor(Vec2 pos);
		Generator* mpTempGenerator;//for editor
		bool bCompileWithEditor;


		//score related
		void LoadScores();
		void SaveScores();
		int iHiScore[10];
		char szHiScore[10][6];

		//cheat related
		bool bWimpMode;//player is given a shield
		bool bBossFight;//skip to boss


		//more frontend
		void StartFrontEnd();
		bool bDemo;
		int curDemo;
};
extern Engine* gpEngine;
