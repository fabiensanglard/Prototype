#pragma
#ifdef WIN32
#include <sdl.h>
#else
#include <SDL/SDL.h>
#endif
#include <vector>
#include <string>
#include <fstream>
using namespace std;
//#define PROFILE
#ifdef PROFILE
#define PROFILE_FRAME_START	gProfiler.StartFrame()
#define PROFILE_FRAME_END gProfiler.EndFrame()
#define PROFILE_REG_SLICE(slice) gProfiler.NewSlice(slice)
#define PROFILE_START_SLICE(slice) gProfiler.StartSlice(slice)
#define PROFILE_END_SLICE(slice) gProfiler.EndSlice(slice)
#define PROFILE_GET_STATS(outSlice,outTotalFrameDelta,outTotalFrameTicks,outPercent,outCount) gProfiler.GetStats(outSlice,outTotalFrameDelta,outTotalFrameTicks,outPercent,outCount)
#define PROFILE_GET_FINAL_STATS(outSlice,outTotalTime,outTotalTicks,outPercent,outCount) gProfiler.GetFinalStats(outSlice,outTotalTime,outTotalTicks,outPercent,outCount)
#define PROFILE_LOG_FINAL_STATS gProfiler.LogFinalStats()
#else
#define PROFILE_FRAME_START
#define PROFILE_FRAME_END
#define PROFILE_REG_SLICE(slice)
#define PROFILE_START_SLICE(slice)
#define PROFILE_END_SLICE(slice)
#define PROFILE_GET_STATS(outSlice,outTotalFrameDelta,outTotalFrameTicks,outPercent,outCount)
#define PROFILE_GET_FINAL_STATS(outSlice,outTotalTime,outTotalTicks,outPercent,outCount)
#define PROFILE_LOG_FINAL_STATS
#endif
//================================================================================================//
				/******************************************************************
				 ** each object given to the profiler will recieve its own slice **
				 ******************************************************************/
//================================================================================================//
class Slice
{
private:
	unsigned int m_uiStartTime;
	unsigned int m_uiEndTime;
	float m_fDelta;

	unsigned int m_uiCount;//how many times per frame the slice was run
	unsigned int m_uiTotalCount;//final app output
	unsigned int m_uiTotalFrameTicks;//perframe
	unsigned int m_uiTotalTicks;//for final app output
	float m_fTotalFrameDelta;//delta for the entire frame
	float m_fTotalTime;//final app output
	float m_fPercentage;//frame percentage

public:
	Slice();
	void StartCount();
	void EndCount();
	unsigned int GetTicks() { return m_uiEndTime; }
	unsigned int GetTotalFrameTicks() { return m_uiTotalFrameTicks; }
	unsigned int GetTotalTicks() { return m_uiTotalTicks; }
	float GetDelta() { return m_fDelta; }
	float GetTotalFrameDelta() { return m_fTotalFrameDelta; }
	float GetTotalTime() { return m_fTotalTime; }
	unsigned int GetCountPerFrame() { return m_uiCount; }
	unsigned int GetTotalCount() { return m_uiTotalCount; }
	void ResetCount();
	void SetPercentage(float percent) { m_fPercentage = percent; }
	float GetPercentage() { return m_fPercentage; }
	string ID;
};

class Profiler
{
private:
	vector<Slice>	m_Slices;
	vector<Slice>::iterator m_SliceIter;
	unsigned int m_uiStartFrameTicks;
	unsigned int m_uiEndFrameTicks;
	unsigned int m_uiTotalTicks;//final output

public:
	Profiler();
	void NewSlice(string slice);//all slices should be passed at the start of the program
	void StartSlice(string slice);
	void EndSlice(string slice);

	void StartFrame();
	void EndFrame();
	bool GetStats(string& outSlice, float& outTotalFrameDelta, unsigned int& outTotalFrameTicks, float& outPercent, unsigned int& outCount);//use a while loop on this function, when it returns false all stats have been output
	bool GetFinalStats(string& outSlice, float& outTotalTime, unsigned int& outTotalTicks, float& outPercent, unsigned int& outCount);//use a while loop on this function, when it returns false all stats have been output
	void LogFinalStats();
};
#ifdef PROFILE
	extern Profiler gProfiler;
#endif
