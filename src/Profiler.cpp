#include "Profiler.h"
Profiler gProfiler;
//================================================================================================//
											/************
											 ** Slices **
											 ************/
//================================================================================================//
Slice::Slice()
{
	m_uiStartTime =0;
	m_uiEndTime =0;
	m_uiTotalFrameTicks =0;
	m_uiTotalTicks =0;
	m_uiCount =0;
	m_uiTotalCount =0;
	m_fDelta =0;
	m_fTotalFrameDelta =0;
	m_fTotalTime =0;
	m_fPercentage =0;
}
void Slice::StartCount()
{
	m_uiStartTime = SDL_GetTicks();
}
void Slice::EndCount()
{
	m_uiCount++;
	m_uiTotalCount++;
	m_uiEndTime = (SDL_GetTicks() - m_uiStartTime);
	m_fDelta = (float)(m_uiEndTime) * 0.001f;
	if(m_uiEndTime<1)
		m_uiEndTime=1;
	m_uiTotalFrameTicks+=m_uiEndTime;
	m_fTotalFrameDelta+=m_fDelta;
	m_uiTotalTicks+=m_uiEndTime;
	m_fTotalTime+=m_fDelta;
//	printf("Slice: %s TickCount: %i \n",ID.c_str(),(int)GetTicks());
}
void Slice::ResetCount()
{
	m_uiCount =0;
	m_uiTotalFrameTicks =0;
	m_fTotalFrameDelta =0;
	m_fPercentage =0;
}
//================================================================================================//
											/**************
											 ** Profiler **
											 **************/
//================================================================================================//
Profiler::Profiler()
{
	m_uiStartFrameTicks =0;
	m_uiEndFrameTicks =0;
	m_uiTotalTicks =0;
	m_SliceIter = m_Slices.begin();
}
void Profiler::NewSlice(string slice)
{
	Slice newSlice;
	newSlice.ID = slice;
	m_Slices.push_back(newSlice);
//	printf("New Slice: %s was created.\n",slice.c_str());
}
void Profiler::StartSlice(string slice)
{
	vector<Slice>::iterator i;
	for(i=m_Slices.begin();i<m_Slices.end();i++)
	{
		if(slice == (*i).ID)
		{
			(*i).StartCount();
			return;
		}
	}
//	printf("Error Slice: %s was not found.\n",slice.c_str());
}
void Profiler::EndSlice(string slice)
{
	vector<Slice>::iterator i;
	for(i=m_Slices.begin();i<m_Slices.end();i++)
	{
		if(slice == (*i).ID)
		{
			(*i).EndCount();
			return;
		}
	}
//	printf("Error Slice: %s was not found.\n",slice.c_str());
}
void Profiler::StartFrame()
{
	m_SliceIter = m_Slices.begin();
	m_uiStartFrameTicks = SDL_GetTicks();
	vector<Slice>::iterator i;
	for(i=m_Slices.begin();i<m_Slices.end();i++)
	{
		(*i).ResetCount();
	}
}
void Profiler::EndFrame()
{
	m_uiEndFrameTicks = (SDL_GetTicks() - m_uiStartFrameTicks);
	if(m_uiEndFrameTicks<1)
		m_uiEndFrameTicks=1;
	vector<Slice>::iterator i;
	for(i=m_Slices.begin();i<m_Slices.end();i++)
	{
		float ticks = (float)(*i).GetTotalFrameTicks();
		ticks/=m_uiEndFrameTicks;
		(*i).SetPercentage(ticks*100);
//		printf("Slice: %s Percentage: %.1f\n",(*i).ID.c_str(),ticks*100);
	}
	m_uiTotalTicks += m_uiEndFrameTicks;
}
bool Profiler::GetStats(string& outSlice, float& outTotalFrameDelta, unsigned int& outTotalFrameTicks, float& outPercent, unsigned int& outCount)
{
	if(m_SliceIter==m_Slices.end())
	{
		m_SliceIter = m_Slices.begin();
		return false;
	}
	outSlice = (*m_SliceIter).ID;
	outTotalFrameDelta = (*m_SliceIter).GetTotalFrameDelta();
	outTotalFrameTicks = (*m_SliceIter).GetTotalFrameTicks();
	outPercent = (*m_SliceIter).GetPercentage();
	outCount = (*m_SliceIter).GetCountPerFrame();
	m_SliceIter++;
	return true;
}
bool Profiler::GetFinalStats(string& outSlice, float& outTotalTime, unsigned int& outTotalTicks, float& outPercent, unsigned int& outCount)
{
	if(m_uiTotalTicks<1)
		return false;
	if(m_SliceIter==m_Slices.end())
	{
		m_SliceIter = m_Slices.begin();
		return false;
	}

	outSlice = (*m_SliceIter).ID;
	outTotalTime = (*m_SliceIter).GetTotalTime();
	outTotalTicks = (*m_SliceIter).GetTotalTicks();
	outPercent = ((float)(*m_SliceIter).GetTotalTicks()/(float)m_uiTotalTicks)*100;
	outCount = (*m_SliceIter).GetTotalCount();
	m_SliceIter++;
	return true;
}
void Profiler::LogFinalStats()
{
	fstream fs;
	fs.open("Profile_Log.txt",fstream::out);
	string slice;
	float time, percent;
	unsigned int ticks,count;
	while(GetFinalStats(slice,time,ticks,percent,count))
	{
		char sz[1024];
		sprintf(sz,"<ID: %s >| Time: %.2f| Ticks: %i| Percent: %.2f| Count: %i|\n",slice.c_str(),time,ticks,percent,count);
		fs.write(sz,(int)strlen(sz));
	}
	fs.close();
}