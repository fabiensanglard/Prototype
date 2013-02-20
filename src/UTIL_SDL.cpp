#include "UTIL_openGL.h"
#include "UTIL_SDL.h"
//================================================================================================//
									/*****************
									 ** SDL helpers **
									 *****************/
//================================================================================================//
namespace UTIL_SDL
{
//================================================================================================//
									/********************
									 ** Initialize SDL **
									 ********************/
//================================================================================================//
	bool InitSDL(const char* winName, int width, int height, int bpp, bool vsync, bool fscreen)
	{
		gLog.OutPut("\n[Initializing Video Settings]\n");
		const SDL_VideoInfo* info = NULL;
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) < 0 || !(info = SDL_GetVideoInfo()))
		{
			gLog.OutPut("\n[Failed to initialize Video Settings]\n");
			return false;
		}
		SDL_WM_SetIcon(SDL_LoadBMP("icon1.bmp"), NULL);
		int flags = SDL_OPENGL | (fscreen?SDL_FULLSCREEN:0);
/*		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	//	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
*/
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 0);

		if(SDL_SetVideoMode(width, height, bpp, flags) == 0)
		{
			SDL_Quit();
			return false;
		}
		stringstream(str);
		str << "Resolution Set: " << width << "x" << height << "x" << bpp << endl;
		gLog.OutPut(str.str());
#ifdef _WIN32
		if(!vsync)
		{
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT  = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
			if(wglSwapIntervalEXT==NULL)
				PostQuitMessage(0);
			wglSwapIntervalEXT(0);
			gLog.OutPut("Vsync Disabled.\n");
		}
#endif
/*		else
		{
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT  = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
			if(wglSwapIntervalEXT==NULL)
				PostQuitMessage(0);
			wglSwapIntervalEXT(1);
			gLog.OutPut("Vsync Enabled.\n");
		}
*/		SDL_ShowCursor(0);
		SDL_WM_SetCaption(winName, NULL);
		gLog.OutPut("Complete...\n\n");
		return true;
	}
//================================================================================================//
									/*************************
									 ** Initialize joystick **
									 *************************/
//================================================================================================//
	bool InitJoystick()
	{
		SDL_Joystick* pad;
		gLog.OutPut("\n[Searching for Joystick]\n");

		SDL_Init(SDL_INIT_JOYSTICK);
		int numJoy=0;
		if(!(numJoy=SDL_NumJoysticks()))
		{
			gLog.OutPut("No joysticks found...\n");
			return false;
		}
		bool bJoyFound = false;
		for(int n=0; n<numJoy; n++)
		{
			char sz[256];
			pad = SDL_JoystickOpen(n);
			if(!pad)
			{
				sprintf(sz,"Joystick %i could not be opened!\n" ,n);
				gLog.OutPut(sz);
				//return false;
			}
			else
			{
				sprintf(sz,"Joystick %i found and intialized.\n",n);
				gLog.OutPut(sz);
				bJoyFound = true;
			}
		}
		gLog.OutPut("Complete...\n\n");
		if(bJoyFound)
			return true;
		return false;
	}
//================================================================================================//
									/*********************
									 ** pbuffer related **
									 *********************/
//================================================================================================//
#ifdef SDL_VERSION_1_3
	SDL_RenderTarget* Create_pBuffer(unsigned int width, unsigned int height)
	{
		SDL_RenderTarget* pTarget=NULL;
		GLuint tID;
		glGenTextures(1, &tID);
		UTIL_GL::BindTexture(GL_TEXTURE_2D, tID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		pTarget = SDL_GL_CreateRenderTarget(width, height, NULL);
		if(pTarget)
		{
#ifdef DEBUG_VERBOSE
			int value;
			stringstream(str);
			str << "Created render target:\n";
			gLog.OutPut(str.str());str.clear();
			SDL_GL_GetRenderTargetAttribute( pTarget, SDL_GL_RED_SIZE, &value );
			str << "SDL_GL_RED_SIZE:" << value << endl;
			gLog.OutPut(str.str());str.clear();
			SDL_GL_GetRenderTargetAttribute( pTarget, SDL_GL_GREEN_SIZE, &value );
			str << "SDL_GL_GREEN_SIZE:" << value << endl;
			gLog.OutPut(str.str());str.clear();
			SDL_GL_GetRenderTargetAttribute( pTarget, SDL_GL_BLUE_SIZE, &value );
			str << "SDL_GL_BLUE_SIZE:" << value << endl;
			gLog.OutPut(str.str());str.clear();
			SDL_GL_GetRenderTargetAttribute( pTarget, SDL_GL_ALPHA_SIZE, &value );
			str << "SDL_GL_ALPHA_SIZE:" << value << endl;
			gLog.OutPut(str.str());str.clear();
			SDL_GL_GetRenderTargetAttribute( pTarget, SDL_GL_DEPTH_SIZE, &value );
			str << "SDL_GL_DEPTH_SIZE:" << value << endl;
			gLog.OutPut(str.str());str.clear();
#endif
			SDL_GL_BindRenderTarget(pTarget, tID);
		}
		return pTarget;
	}
	void Unlock_pBuffer(SDL_RenderTarget* pTarget)
	{
		if(pTarget)
			SDL_GL_UnlockRenderTarget(pTarget);
	}
	void Lock_pBuffer(SDL_RenderTarget* pTarget)
	{
		if(pTarget)
		{
			SDL_GL_LockRenderTarget(pTarget);
			glViewport(0,0,pTarget->w,pTarget->h);
		}
	}
	void Bind_pBuffer(SDL_RenderTarget pTarget)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,pTarget.texture);
	}
#else

#ifdef _WIN32
	// FrameBuffer (FBO) gen, bin and texturebind
	PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffers ;
	PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer ;
	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2D ;
	PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatus ;

	int windowsOpenGLInitialized = 0;

	void getOpenGLFunctionPointers(void){
		glGenFramebuffers		= (PFNGLGENFRAMEBUFFERSEXTPROC)		wglGetProcAddress("glGenFramebuffersEXT");
		glBindFramebuffer		= (PFNGLBINDFRAMEBUFFEREXTPROC)		wglGetProcAddress("glBindFramebufferEXT");
		glFramebufferTexture2D	= (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
		glCheckFramebufferStatus	= (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	}

#elif defined(__linux__)
	void initializeGLEW(void)
	{
	  static bool is_initialized = false;
	  if (is_initialized == false)
	  {
	    const GLenum error = glewInit();
	    if (error != GLEW_OK)
	      printf("glewInit error: %x\n", error);
	    else
	      {
		if (glGenFramebuffers == NULL)
		  {
		    glGenFramebuffers = glGenFramebuffersEXT;
		    glBindFramebuffer = glBindFramebufferEXT;
		    glFramebufferTexture2D = glFramebufferTexture2DEXT;
		    glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
		  }
		if (glGenFramebuffersEXT == NULL)
		  printf("FrameBuffer NOT supported\n");
	      }
	    is_initialized = true;
	  }
	}
#endif



	typedef struct pBuffer{
		unsigned int id;
		GLuint fboID;
		GLuint textureID;
		unsigned int w;
		unsigned int h;
	} pBuffer_t; 

	int renderBuffersCounter=0;
	pBuffer_t renderBuffers[10] = {};
	

	SDL_RenderTarget* Create_pBuffer(unsigned int width, unsigned int height){
		
		printf("Create_pBuffer(%ud,%ud);\n",width,height);

		#ifdef _WIN32
		if (!windowsOpenGLInitialized){
			getOpenGLFunctionPointers();
			windowsOpenGLInitialized=1;
		}
		#elif defined(__linux__)
		initializeGLEW();
		#endif

		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

		// create a framebuffer object
		GLuint fboId;
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);

		// attach the texture to FBO color attachment point
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, textureId, 0);
		int attachError = glGetError();
		if (attachError != GL_NO_ERROR){
			switch(attachError){
				printf("error in glFramebufferTexture2D.\n");
			}
		}
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE){
			printf("An error occured creating the pBuffer.\n");
			switch (status)
			{
				case GL_FRAMEBUFFER_UNDEFINED : printf("GL_FRAMEBUFFER_UNDEFINED \n"); break ;
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT  : printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT  \n"); break ;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT  : printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT  \n"); break ;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER  : printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER  \n"); break ;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER  : printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER  \n"); break ;
				case GL_FRAMEBUFFER_UNSUPPORTED  : printf("GL_FRAMEBUFFER_UNSUPPORTED  \n"); break ;
				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE  : printf("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE  \n"); break ;
				//case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE  : printf("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE  \n"); break ;
				//case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS   : printf("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS   \n"); break ;
				default: printf("error code unknow: %d",status);
			}
		}

		renderBuffers[renderBuffersCounter].fboID = fboId;
		renderBuffers[renderBuffersCounter].textureID = textureId;
		renderBuffers[renderBuffersCounter].w = width;
		renderBuffers[renderBuffersCounter].h = height;
		renderBuffers[renderBuffersCounter].id = renderBuffersCounter;
		renderBuffersCounter++;

		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		printf("pBuffer created ID= %d.\n",renderBuffers[renderBuffersCounter-1].id);
		return &renderBuffers[renderBuffersCounter-1].id;
	}

	void Unlock_pBuffer(SDL_RenderTarget* pTarget){
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0,0,640,480);
	}

	void Lock_pBuffer(SDL_RenderTarget* pTarget){

		if(!pTarget)
			return;

		pBuffer_t* pBuffer = &renderBuffers[*pTarget];

			
		glBindFramebuffer(GL_FRAMEBUFFER, pBuffer->fboID);
		glViewport(0,0,pBuffer->w,pBuffer->h);
	}

	void Bind_pBuffer(SDL_RenderTarget pTarget){
		
		pBuffer_t* pBuffer = &renderBuffers[pTarget];
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,pBuffer->fboID);
		
		//glViewport(0,0,1024,512);
	}

#endif
}
