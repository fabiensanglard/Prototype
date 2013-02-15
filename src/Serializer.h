#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
//================================================================================================//
						/**************************************************
						** Serializer:	Will read/write config variables **	
						***************************************************/
//================================================================================================//
class Serializer
{
public:
	Serializer(){}
	~Serializer(){}
	void PutComment(string file, string comment);
//================================================================================================//
						/************************************************************************************
						** Read variable will search the file for variable and load it into the parsed var **
						** If the file did not exist, it will be written instead (auto-generation)         **
						*************************************************************************************/
//================================================================================================//
	void ReadVariable(string file, string varname, int& value);
	void ReadVariable(string file, string varname, float& value);
	void ReadVariable(string file, string varname, string& value);

private:
	bool SearchFileGenerate(string file);
	vector<string>	m_GenFiles;//store filenames that couldnt be loaded, so we can generate them now
};

extern Serializer gSerializer;