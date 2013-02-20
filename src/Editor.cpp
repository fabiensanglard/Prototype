#include "Engine.h"
#include "UTIL_BitOp.h"
//================================================================================================//
						/************************
						** Set editor defaults **	
						*************************/
//================================================================================================//
void Engine::StartEditor()
{
	fMapSpeed = 0;
	bShowTools = true;
	iEditTool = 0;
	iEditLayer = 1;
	bShowTileTool = false;
	iTileFrame = 0;
	iTileFrameEnd = 0;
	edTileProperties = 0;
	iEdScroll = 0;
	bPlayBackMode = false;
	bShowEntityTool = false;
	bShowEntityProps = false;
	bPlaceEntity = false;
	bPlaceAnim = false;
	bShowAnimProps = false;
	oScroll = Scroll;
	mpTempGenerator = NULL;
	bRecording = false;
	bWimpMode = true;
}
//================================================================================================//
						/******************
						** Editor Update **	
						*******************/
//================================================================================================//
void Engine::UpdateEditor(float delta)
{
	if(bShowTools)//selecting a tool to use
	{
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			iEditTool>0?iEditTool--:iEditTool=4;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			iEditTool<4?iEditTool++:iEditTool=0;
		}

		if(KEY_UI_SELECT.state == JUST_RELEASED || KEY_ATTACK1.state == JUST_RELEASED)
		{
			switch(iEditTool)
			{
			case 0://select tile
				bShowTools = false;
				bShowTileTool = true;
				bPlaceEntity = false;
				iTileFrameEnd = iTileFrame = 0;
				break;
			case 1://flip tile
				{
					bool FlipH = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_HORIZONTAL);
					bool FlipV = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_VERTICAL);
					if(edTileProperties==0)
						UTIL_SetBit<unsigned char>(edTileProperties,FLIP_HORIZONTAL);
					else if(FlipH && !FlipV)
					{
						UTIL_ClearBits<unsigned char>(edTileProperties);
		 				UTIL_SetBit<unsigned char>(edTileProperties,FLIP_VERTICAL);
					}
					else if(FlipV && !FlipH)
					{
						UTIL_ClearBits<unsigned char>(edTileProperties);
						UTIL_SetBit<unsigned char>(edTileProperties,FLIP_HORIZONTAL);
		 				UTIL_SetBit<unsigned char>(edTileProperties,FLIP_VERTICAL);
					}
					else if(FlipV && FlipH)
						UTIL_ClearBits<unsigned char>(edTileProperties);
				}
				break;
			case 2://entity selector tile
				bShowTools = false;
				bShowEntityTool = true;
				iTileFrameEnd = iTileFrame = 0;
				break;
			case 3://anim selector
				bShowTools = false;
				bShowAnimProps = true;
				iTileFrameEnd = iTileFrame = 0;
				break;
			case 4://record a demo
				bShowTools = false;
				bRecording = true;
				bUseEditor = false;
				bPlayBackMode = true;
				bShowEntityProps = false;
				Scroll = (float)(iEdScroll*32);
				pRecordEnt = (ControlledEntity*)&mPlayer;
				pRecordEnt->RegisterControl(mButtons);
				pRecordEnt->StartRecording();
				fGameDifficulty=1.5f;
				InitializeGame();
				Scroll = (float)(iEdScroll*32);
				InitializeGenerators();
				mPlayer.iSpawnOffset = iEdScroll*32;
				mPlayer.Spawn(Vec2((float)(iEdScroll*32),180));
				break;
			}
		}
	}
	else if(bShowTileTool)//selecting a tile to use
	{
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			if(iTileFrame>0)
				iTileFrame--;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			if(iTileFrame<1023)iTileFrame++;
		}
		if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
		{
			if(iTileFrame>31)
				iTileFrame-=32;
		}
		if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
		{
			if(iTileFrame<992)iTileFrame+=32;
		}
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
			iTileFrameEnd = iTileFrame;
		if(KEY_UI_SELECT.state == JUST_RELEASED || KEY_ATTACK1.state == JUST_RELEASED)
			bShowTileTool = false;
	}
	else if(bPlayBackMode)
	{
		UpdateGame(TIME_STEP);
		if(bRecording && KEY_PAUSE.state == JUST_PRESSED)//stop recording
		{
			bRecording = false;
			pRecordEnt->SelfControl();
			if(pRecordEnt == &mPlayer)//was recording demo
			{
				demoSystem dem;
				dem.iScrollOffset = iEdScroll*32;
				dem.mKeyBuffer = mPlayer.mKeyBuffer;
				SaveDemo(dem);
			}
			else
				bShowEntityProps = true;
			pRecordEnt = NULL;
			bUseEditor = true;
			bPlayBackMode = false;
			iTileFrame = 0;
		}
	}
	else if(bShowEntityTool)
	{
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			if(iTileFrame>0)
				iTileFrame--;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			if(iTileFrame<255)iTileFrame++;
		}
		if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
		{
			if(iTileFrame>15)
				iTileFrame-=16;
		}
		if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
		{
			if(iTileFrame<240)iTileFrame+=16;
		}
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
		{
			bShowEntityTool = false;
			bPlaceEntity = true;
		}
	}
	else if(bPlaceEntity)//place entity mode
	{
		oScroll = Scroll;
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			if(EdPos.x>0)
				EdPos.x--;
			if(EdPos.x==iEdScroll && iEdScroll>0)
				iEdScroll--;
			oScroll = Scroll = (float)iEdScroll*32;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			if(EdPos.x<399)
				EdPos.x++;
			if(EdPos.x==iEdScroll+20 && iEdScroll<380)
				iEdScroll++;
			oScroll = Scroll = (float)iEdScroll*32;
		}
		if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
		{
			if(EdPos.y>0)
				EdPos.y--;
		}
		if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
		{
			if(EdPos.y<14)
				EdPos.y++;
		}
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
		{
			Generator g;
			if(!(g.mpEntity = CreateEntity(iTileFrame)))
				return;
			g.Pos.x = EdPos.x*32;
			g.Pos.y = EdPos.y*32;
			g.ScrollOffset = iEdScroll*32;
			g.iEntType = iTileFrame;
			mGenerators.push_back(g);
			mpTempGenerator = &mGenerators.back();
			bPlaceEntity = false;
			bShowEntityProps = true;
			iTileFrame = 0;
		}
		if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
		{
			RemoveGeneratorsByEdit();
		}
	}
	else if(bShowEntityProps)
	{
		UpdateEntityProps();
	}
	else if(bShowAnimProps)
	{
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			if(iTileFrame>0)
				iTileFrame--;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			if(iTileFrame<255)iTileFrame++;
		}
		if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
		{
			if(iTileFrame>15)
				iTileFrame-=16;
		}
		if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
		{
			if(iTileFrame<240)iTileFrame+=16;
		}
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
		{
			bShowAnimProps = false;
			bPlaceAnim = true;
		}
	}
	else if(bPlaceAnim)
	{
		oScroll = Scroll;
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			if(EdPos.x>0)
				EdPos.x--;
			if(EdPos.x==iEdScroll && iEdScroll>0)
				iEdScroll--;
			oScroll = Scroll = (float)iEdScroll*32;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			if(EdPos.x<399)
				EdPos.x++;
			if(EdPos.x==iEdScroll+20 && iEdScroll<380)
				iEdScroll++;
			oScroll = Scroll = (float)iEdScroll*32;
		}
		if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
		{
			if(EdPos.y>0)
				EdPos.y--;
		}
		if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
		{
			if(EdPos.y<14)
				EdPos.y++;
		}
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
		{
			AnimGenerator g;
			
			g.iAnimType = iTileFrame;
			g.mFX = GetAnimForEditor((EdPos*32));
			g.ScrollOffset = (EdPos.x*32)-640;
			if(g.ScrollOffset<0)
				g.ScrollOffset = 0;

			g.iLayer = iEditLayer;
			mAnimGenerators.push_back(g);
			bPlaceAnim = false;
			bShowAnimProps = true;
			iTileFrame = 0;
		}
		if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
		{
			RemoveGeneratorsByEdit();
		}
	}
	else//we are placing tiles
	{
		oScroll = Scroll;
		if(KEY_UI_LEFT.state == JUST_PRESSED || KEY_LEFT.state == JUST_PRESSED)
		{
			if(EdPos.x>0)
				EdPos.x--;
			if(EdPos.x==iEdScroll && iEdScroll>0)
				iEdScroll--;
			oScroll = Scroll = (float)iEdScroll*32;
		}
		if(KEY_UI_RIGHT.state == JUST_PRESSED || KEY_RIGHT.state == JUST_PRESSED)
		{
			if(EdPos.x<399)
				EdPos.x++;
			if(EdPos.x==iEdScroll+20 && iEdScroll<380)
				iEdScroll++;
			oScroll = Scroll = (float)iEdScroll*32;
		}
		if(KEY_UI_UP.state == JUST_PRESSED || KEY_UP.state == JUST_PRESSED)
		{
			if(EdPos.y>0)
				EdPos.y--;
		}
		if(KEY_UI_DOWN.state == JUST_PRESSED || KEY_DOWN.state == JUST_PRESSED)
		{
			if(EdPos.y<14)
				EdPos.y++;
		}
		if(KEY_UI_SELECT.state == JUST_PRESSED || KEY_ATTACK1.state == JUST_PRESSED)
		{
			int w,h,tx,ty,tx2,ty2;
			if(iTileFrameEnd == iTileFrame)
			{
				switch(iEditLayer)
				{
				case 1:
					mTilesLayer1[(int)EdPos.x][(int)EdPos.y].ID = iTileFrame;
					mTilesLayer1[(int)EdPos.x][(int)EdPos.y].properties = edTileProperties;
					break;
				case 2:
					mTilesLayer2[(int)EdPos.x][(int)EdPos.y].ID = iTileFrame;
					mTilesLayer2[(int)EdPos.x][(int)EdPos.y].properties = edTileProperties;
					break;
				case 3:
					mTilesLayer3[(int)EdPos.x][(int)EdPos.y].ID = iTileFrame;
					mTilesLayer2[(int)EdPos.x][(int)EdPos.y].properties = edTileProperties;
					break;
				}
			}
			else
			{
				ty2 = iTileFrame/32;
				tx2 = iTileFrame-(ty2*32);
				ty = iTileFrameEnd/32;
				tx = iTileFrameEnd-(ty*32);
				w=tx2-tx;
				h=ty2-ty;
				bool FlipH = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_HORIZONTAL);
				bool FlipV = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_VERTICAL);
				for(int x= FlipH? w:0;(FlipH? x>=0 : x<w+1);(FlipH? x-- : x++))
				{
					for(int y= FlipV? h:0;(FlipV? y>=0 : y<h+1); (FlipV? y-- : y++))
					{
						if(EdPos.y+y>14 || EdPos.x+x>399)
							continue;
						switch(iEditLayer)
						{
						case 1:
							if(FlipH && !FlipV)
								mTilesLayer1[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+(y*32)+((w+1)-(x+1));
							else if (FlipV && !FlipH)
								mTilesLayer1[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+x+(((h+1)-(y+1))*32);
							else if (FlipV && FlipH)
								mTilesLayer1[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+(((h+1)-(y+1))*32)+((w+1)-(x+1));
							else
								mTilesLayer1[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+x+(y*32);
							mTilesLayer1[(int)EdPos.x+x][(int)EdPos.y+y].properties = edTileProperties;
							break;
						case 2:
							if(FlipH && !FlipV)
								mTilesLayer2[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+(y*32)+((w+1)-(x+1));
							else if (FlipV && !FlipH)
								mTilesLayer2[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+x+(((h+1)-(y+1))*32);
							else if (FlipV && FlipH)
								mTilesLayer2[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+(((h+1)-(y+1))*32)+((w+1)-(x+1));
							else
								mTilesLayer2[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+x+(y*32);
							mTilesLayer2[(int)EdPos.x+x][(int)EdPos.y+y].properties = edTileProperties;
							break;
						case 3:
							if(FlipH && !FlipV)
								mTilesLayer3[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+(y*32)+((w+1)-(x+1));
							else if (FlipV && !FlipH)
								mTilesLayer3[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+x+(((h+1)-(y+1))*32);
							else if (FlipV && FlipH)
								mTilesLayer3[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+(((h+1)-(y+1))*32)+((w+1)-(x+1));
							else
								mTilesLayer3[(int)EdPos.x+x][(int)EdPos.y+y].ID = iTileFrameEnd+x+(y*32);
							mTilesLayer3[(int)EdPos.x+x][(int)EdPos.y+y].properties = edTileProperties;
							break;
						}
					}
				}
			}
		}
		if(KEY_UI_SELECT2.state == JUST_PRESSED || KEY_ATTACK2.state == JUST_PRESSED)
		{
			int w,h,tx,ty,tx2,ty2;
			ty2 = iTileFrame/32;
			tx2 = iTileFrame-(ty2*32);
			ty = iTileFrameEnd/32;
			tx = iTileFrameEnd-(ty*32);
			w=tx2-tx;
			h=ty2-ty;
			bool FlipH = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_HORIZONTAL);
			bool FlipV = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_VERTICAL);
			for(int x= FlipH? w:0;(FlipH? x>=0 : x<w+1);(FlipH? x-- : x++))
			{
				for(int y= FlipV? h:0;(FlipV? y>=0 : y<h+1); (FlipV? y-- : y++))
				{
					if(EdPos.y+y>14 || EdPos.x+x>399)
						continue;
					switch(iEditLayer)
					{
					case 1:
						mTilesLayer1[(int)EdPos.x+x][(int)EdPos.y+y].ID = -1;
						mTilesLayer1[(int)EdPos.x+x][(int)EdPos.y+y].properties = 0;
						break;
					case 2:
						mTilesLayer2[(int)EdPos.x+x][(int)EdPos.y+y].ID = -1;
						mTilesLayer2[(int)EdPos.x+x][(int)EdPos.y+y].properties = 0;
						break;
					case 3:
						mTilesLayer3[(int)EdPos.x+x][(int)EdPos.y+y].ID = -1;
						mTilesLayer3[(int)EdPos.x+x][(int)EdPos.y+y].properties = 0;
						break;
					}	
				}
			}
		}

	}

}
//================================================================================================//
						/******************
						** Editor Render **	
						*******************/
//================================================================================================//
void Engine::RenderEditor(const float interp)
{
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UTIL_GL::GL2D::SetOrtho((float)winWidth,(float)winHeight);

	if(bPlayBackMode)
	{
		RenderGame(interp);

		//draw text overlays
		char sz[64];
		mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		mFont1.SetScale(0.5f,0.5f);
		mFont1.SetColor(1,1,1,1);
		mFont1.SetAlignment(ALIGN_LEFT);
		sprintf(sz,"SCROLL OFFSET: %i",iEdScroll);
		mFont1.Print(sz,10,360);
		sprintf(sz,"Active Generators: %i",(int)mActiveGenerators.size());
		mFont1.Print(sz,10,380);
		sprintf(sz,"Active Entites: %i",(int)mGameEnts.size()+(int)mTopGameEnts.size());
		mFont1.Print(sz,10,400);
		sprintf(sz,"ACTIVE FX: 1: %i 2: %i 3: %i",(int)mEffectsLayer1.size(),(int)mEffectsLayer2.size(),(int)mEffectsLayer3.size());
		mFont1.Print(sz,10,420);
		sprintf(sz,"REMAINING FX GENERATORS: %i",(int)mLocalAnimGenerators.size());
		mFont1.Print(sz,10,440);
		sprintf(sz,"ACTIVE BULLETS: %i",(int)mBullets.size()+(int)mPlayerBullets.size());
		mFont1.Print(sz,10,460);
		return;
	}

	int n;
	pair<float,float>st;
	pair<float,float>scale;
	float s,t;
	float tw;
	float th;

	glTranslatef(-(float)(iEdScroll*32),0,0);
	RenderMapLayer(3,0,true);
	RenderMapLayer(2,0,true);
	RenderMapLayer(1,0,true);
	//draw editor ents
	UTIL_GL::SetBlend(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,1);
	list<Generator>::iterator iter;
	for(iter = mGenerators.begin(); iter != mGenerators.end(); iter++)
	{
		RenderSprite(sprEntities,(*iter).iEntType,(*iter).Pos.x,(*iter).Pos.y,32,32);
	}
	list<AnimGenerator>::iterator iter2;
	for(iter2 = mAnimGenerators.begin(); iter2 != mAnimGenerators.end(); iter2++)
	{
		RenderSprite(sprAnims,(*iter2).iAnimType,(*iter2).mFX.mPos.x,(*iter2).mFX.mPos.y,32,32);
	}


	mFont1.SetBlendMode(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	mFont1.SetScale(0.5f,0.5f);
	mFont1.SetColor(1,1,1,1);

	//draw cursor tile
	if(!bShowTileTool && !bShowEntityTool && !bPlaceEntity && !bShowEntityProps && !bShowAnimProps && !bPlaceAnim)
	{
		int	ty2 = iTileFrame/32;
		int	tx2 = iTileFrame-(ty2*32);
		int	ty = iTileFrameEnd/32;
		int	tx = iTileFrameEnd-(ty*32);
		int w = (tx2-tx)+1, h = (ty2-ty)+1;

		scale = sprMapTiles.GetTextureScale();
		tw = scale.first;
		th = scale.second;
		glColor4f(1,1,1,1);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,sprMapTiles.imgID);
		st = sprMapTiles.GetFrameOffsets(iTileFrame);
		s = st.first;
		t = st.second;
		float x,x2,y,y2;
		bool FlipH = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_HORIZONTAL);
		bool FlipV = UTIL_IsBitSet<unsigned char>(edTileProperties,FLIP_VERTICAL);
		if(FlipH)
		{
			x2=(32*EdPos.x);
			x=(32*EdPos.x)+(32*w);
		}
		else
		{
			x=(32*EdPos.x);
			x2=(32*EdPos.x)+(32*w);
		}
		if(FlipV)
		{
			y2=(32*EdPos.y);
			y=(32*EdPos.y)+(32*h);
		}
		else
		{
			y=(32*EdPos.y);
			y2=(32*EdPos.y)+(32*h);
		}
		glBegin(GL_QUADS);
		glTexCoord2f(s-(tw*(w-1)),(t-(th*(h-1))));
		glVertex2f(x,y);
		glTexCoord2f((s-(tw*(w-1)))+(tw*w),(t-(th*(h-1))));
		glVertex2f(x2,y);
		glTexCoord2f((s-(tw*(w-1)))+(tw*w),(t-(th*(h-1)))+(th*h));
		glVertex2f(x2,y2);
		glTexCoord2f(s-(tw*(w-1)),(t-(th*(h-1)))+(th*h));
		glVertex2f(x,y2);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor4f(1,1,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex2f((32*EdPos.x),(32*EdPos.y));
		glVertex2f((32*EdPos.x)+(32*w),(32*EdPos.y));
		glVertex2f((32*EdPos.x)+(32*w),(32*EdPos.y)+(32*h));
		glVertex2f((32*EdPos.x),(32*EdPos.y)+(32*h));
		glEnd();
	}
	//draw cursor during entity placement
	if(bPlaceEntity)
	{
		
		

		scale = sprEntities.GetTextureScale();
		tw = scale.first;
		th = scale.second;
		glColor4f(1,1,1,1);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,sprEntities.imgID);
		st = sprEntities.GetFrameOffsets(iTileFrame);
		s = st.first;
		t = st.second;
		float x,y;
		x=(32*EdPos.x);
		y=(32*EdPos.y);
		glBegin(GL_QUADS);
		glTexCoord2f(s,t);
		glVertex2f(x,y);
		glTexCoord2f(s+tw,t);
		glVertex2f(x+32,y);
		glTexCoord2f(s+tw,t+th);
		glVertex2f(x+32,y+32);
		glTexCoord2f(s,t+th);
		glVertex2f(x,y+32);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor4f(1,1,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex2f((32*EdPos.x),(32*EdPos.y));
		glVertex2f((32*EdPos.x)+32,(32*EdPos.y));
		glVertex2f((32*EdPos.x)+32,(32*EdPos.y)+32);
		glVertex2f((32*EdPos.x),(32*EdPos.y)+32);
		glEnd();
	}
	//draw cursor during anim placement
	if(bPlaceAnim)
	{
		
		

		scale = sprAnims.GetTextureScale();
		tw = scale.first;
		th = scale.second;
		glColor4f(1,1,1,1);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,sprAnims.imgID);
		st = sprAnims.GetFrameOffsets(iTileFrame);
		s = st.first;
		t = st.second;
		float x,y;
		x=(32*EdPos.x);
		y=(32*EdPos.y);
		glBegin(GL_QUADS);
		glTexCoord2f(s,t);
		glVertex2f(x,y);
		glTexCoord2f(s+tw,t);
		glVertex2f(x+32,y);
		glTexCoord2f(s+tw,t+th);
		glVertex2f(x+32,y+32);
		glTexCoord2f(s,t+th);
		glVertex2f(x,y+32);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor4f(1,1,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex2f((32*EdPos.x),(32*EdPos.y));
		glVertex2f((32*EdPos.x)+32,(32*EdPos.y));
		glVertex2f((32*EdPos.x)+32,(32*EdPos.y)+32);
		glVertex2f((32*EdPos.x),(32*EdPos.y)+32);
		glEnd();
	}
	glTranslatef((float)(iEdScroll*32),0,0);
	//DoPostEffects(interp);


	//draw the tool selector
	if(bShowTools)
	{
		scale = sprIcons.GetTextureScale();
		tw = scale.first;
		th = scale.second;

		glColor4f(0.3f,0.6f,0.8f,0.8f);
		RenderUIPanel(240,220,160,40);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,sprIcons.imgID);
		glColor4f(1,1,0,0.95f);
		glBegin(GL_QUADS);
		for(n=0;n<4;n++)
		{
			st = sprIcons.GetFrameOffsets(n);
			s = st.first;
			t = st.second;
			glTexCoord2f(s,t);
			glVertex2f(254+(32*(float)n),224);
			glTexCoord2f(s+tw,t);
			glVertex2f(254+(32*(float)n)+32,224);
			glTexCoord2f(s+tw,t+th);
			glVertex2f(254+(32*(float)n)+32,224+32);
			glTexCoord2f(s,t+th);
			glVertex2f(254+(32*(float)n),224+32);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor4f(1,1,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex2f(254+(32*(float)iEditTool),224);
		glVertex2f(254+(32*(float)iEditTool)+32,224);
		glVertex2f(254+(32*(float)iEditTool)+32,224+32);
		glVertex2f(254+(32*(float)iEditTool),224+32);
		glEnd();

		mFont1.SetAlignment(ALIGN_CENTER);
		switch(iEditTool)
		{
		case 0: mFont1.Print("SELECT TILE",320,200);break;
		case 1: mFont1.Print("FLIP TILE",320,200);break;
		case 2: mFont1.Print("PLACE ENTITY",320,200);break;
		case 3: mFont1.Print("PLACE ANIMATION",320,200);break;
		case 4: mFont1.Print("RECORD DEMO",320,200);break;
		}
	}
	//draw the tile tool selector
	if(bShowTileTool)
	{
		glColor4f(0.3f,0.6f,0.8f,0.8f);
		RenderUIPanel(184,104,272,272);
		glColor4f(1,1,1,1);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,sprMapTiles.imgID);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(192,112);
		glTexCoord2f(1,0);
		glVertex2f(192+256,112);
		glTexCoord2f(1,1);
		glVertex2f(192+256,112+256);
		glTexCoord2f(0,1);
		glVertex2f(192,112+256);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		int ty,tx,ty2,tx2;
		if(KEY_UI_SELECT.state == PRESSED || KEY_ATTACK1.state == PRESSED)
		{
			ty2 = iTileFrame/32;
			tx2 = iTileFrame-(ty2*32);
			ty = iTileFrameEnd/32;
			tx = iTileFrameEnd-(ty*32);
		}
		else
		{
			ty = iTileFrame/32;
			tx = iTileFrame-(ty*32);
			tx2 = tx;
			ty2 = ty;
		}
		glColor4f(1,1,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex2f(192+(8*(float)tx),112+(8*(float)ty));
		glVertex2f(192+(8*(float)tx2)+8,112+(8*(float)ty));
		glVertex2f(192+(8*(float)tx2)+8,112+(8*(float)ty2)+8);
		glVertex2f(192+(8*(float)tx),112+(8*(float)ty2)+8);
		glEnd();
	}
	//draw entity tool selector
	if(bShowEntityTool)
	{
		glColor4f(0.3f,0.6f,0.8f,0.8f);
		RenderUIPanel(184,104,272,272);
		glColor4f(1,1,1,1);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,sprEntities.imgID);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(192,112);
		glTexCoord2f(1,0);
		glVertex2f(192+256,112);
		glTexCoord2f(1,1);
		glVertex2f(192+256,112+256);
		glTexCoord2f(0,1);
		glVertex2f(192,112+256);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		int ty,tx,ty2,tx2;
		ty = iTileFrame/16;
		tx = iTileFrame-(ty*16);
		tx2 = tx;
		ty2 = ty;
		glColor4f(1,1,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex2f(192+(16*(float)tx),112+(16*(float)ty));
		glVertex2f(192+(16*(float)tx2)+16,112+(16*(float)ty));
		glVertex2f(192+(16*(float)tx2)+16,112+(16*(float)ty2)+16);
		glVertex2f(192+(16*(float)tx),112+(16*(float)ty2)+16);
		glEnd();
	}
	if(bShowEntityProps)
	{
		glColor4f(0.3f,0.6f,0.8f,0.8f);
		RenderUIPanel(100,50,440,380);
		glColor4f(1,1,1,1);
		RenderEntityProps();
	}
	//draw anim tool selector
	if(bShowAnimProps)
	{
		glColor4f(0.3f,0.6f,0.8f,0.8f);
		RenderUIPanel(184,104,272,272);
		glColor4f(1,1,1,1);
		UTIL_GL::SetBlend(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		UTIL_GL::BindTexture(GL_TEXTURE_2D,sprAnims.imgID);
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(192,112);
		glTexCoord2f(1,0);
		glVertex2f(192+256,112);
		glTexCoord2f(1,1);
		glVertex2f(192+256,112+256);
		glTexCoord2f(0,1);
		glVertex2f(192,112+256);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		int ty,tx,ty2,tx2;
		ty = iTileFrame/16;
		tx = iTileFrame-(ty*16);
		tx2 = tx;
		ty2 = ty;
		glColor4f(1,1,0,1);
		glBegin(GL_LINE_LOOP);
		glVertex2f(192+(16*(float)tx),112+(16*(float)ty));
		glVertex2f(192+(16*(float)tx2)+16,112+(16*(float)ty));
		glVertex2f(192+(16*(float)tx2)+16,112+(16*(float)ty2)+16);
		glVertex2f(192+(16*(float)tx),112+(16*(float)ty2)+16);
		glEnd();
	}
	//draw text overlays
	char sz[64];
	mFont1.SetColor(1,1,1,1);
	mFont1.SetAlignment(ALIGN_LEFT);
	sprintf(sz,"SCROLL OFFSET: %i",iEdScroll);
	mFont1.Print(sz,10,380);
	sprintf(sz,"EDITING LAYER: %i",iEditLayer);
	mFont1.Print(sz,10,400);
}
