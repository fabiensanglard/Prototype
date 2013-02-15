#include "Log.h"
//================================================================================================//
						/**************************
						** global logging system **	
						***************************/
//================================================================================================//
Log gLog;
#define PRINT_TO_CONSOLE//if defined will print to the console aswell as the logfile

void Log::OutPut(string str)
{
	if(!bKeepLog)
		return;
#ifdef PRINT_TO_CONSOLE
	printf(str.c_str());
#endif
	ofstream f;
	if(!lcount)
	{
		f.open("Logfile.txt");
		f << "Logfile created on " << __DATE__ << endl;
	}
	else
		f.open("Logfile.txt",ios::app);

	if(f.is_open())
	{
		f << str.c_str();
		f.close();
	}
	lcount++;
}
void Log::SetLogState(int state)
{
	bool l = state?true:false;
	if(l && !bKeepLog)
	{
		bKeepLog = l;
		OutPut(">>Logging Enabled<<\n");
	}
	else if(!l && bKeepLog)
	{
		bKeepLog = l;
		OutPut(">>Logging Disabled<<\n");
	}
}