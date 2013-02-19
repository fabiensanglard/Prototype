#pragma once
#ifdef WIN32
#include <sdl.h>
#else
#include <SDL/SDL.h>
#endif
class Timer
{
private:
	Uint32 m_uiLastTime;
	Uint32 m_uiCurTime;
	float m_fDelta;
	float m_fOldTime;
	unsigned int m_uiCountFPS;
	unsigned int m_uiFPS;
public:
	float m_fTime;
	Timer(void);
	~Timer(void);
	void Update();

	float GetTime() { return m_fTime; }
	float GetDelta() { return m_fDelta<0.1f? m_fDelta: 0; }
	unsigned int GetFPS() { return m_uiFPS; }
	unsigned int GetFPSCounter() { return m_uiCountFPS; }
	void LockFrameRate(int fps) {
		Uint32 tick = SDL_GetTicks()-m_uiCurTime;
//		if(tick>5000)
//			tick=0;		
		while(tick<(unsigned int)fps)
		{
			tick = SDL_GetTicks()-m_uiCurTime;
		}
	}
};
