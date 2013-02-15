#pragma once
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
//================================================================================================//
						/**************************
						** global logging system **	
						***************************/
//================================================================================================//
class Log
{
public:
	Log(){ bKeepLog = false; lcount=0;}
	~Log(){}

	void OutPut(string str);
	void SetLogState(int state);
private:
	bool bKeepLog;
	int lcount;
};

extern Log gLog;