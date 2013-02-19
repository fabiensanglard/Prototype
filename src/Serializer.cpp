#include "Serializer.h"
//#define PRINT_SERIAL
//================================================================================================//
						/**************************************************
						** Serializer:	Will read/write config variables **	
						***************************************************/
//================================================================================================//
Serializer gSerializer;

//================================================================================================//
						/********************************************************
						** Comments are only written if the file did not exist **	
						*********************************************************/
//================================================================================================//
void Serializer::PutComment(string file, string comment)
{
	if(SearchFileGenerate(file))
	{
		ofstream f;
		f.open(file.c_str(),ios::app);
		f << comment << endl;
		f.close();
	}
}
//================================================================================================//
						/************************************************************************************
						** Read variable will search the file for variable and load it into the parsed var **
						** If the file did not exist, it will be written instead (auto-generation)         **
						*************************************************************************************/
//================================================================================================//
void Serializer::ReadVariable(string file, string varname, int& value)
{
#ifdef _WIN32
	if(SearchFileGenerate(file))//need to write to the cfg
	{
		ofstream f;
		f.open(file.c_str(),ios::app);
		f << varname << "	= " << value << endl;
		f.close();
	}
	else//need to read from the config
	{
		ifstream f;
		f.open(file.c_str());
		string s;
		while(!f.eof())
		{
			getline(f,s);
			string tok;
			stringstream(s) >> tok;
			if(tok == varname)
			{
				string tok2;
				stringstream(s) >> tok >> tok2 >> value;
#ifdef PRINT_SERIAL
				printf("%s read from file %s\n",tok.c_str(),file.c_str());
#endif
				break;
			}
#ifdef PRINT_SERIAL
			if(f.eof())
			{
				printf("Failed to read %s from file %s\n",varname.c_str(),file.c_str());
			}
#endif
		}
		f.close();
	}
#endif
}
void Serializer::ReadVariable(string file, string varname, float& value)
{
    #ifdef _WIN32
	if(SearchFileGenerate(file))
	{
		ofstream f;
		f.open(file.c_str(),ios::app);
		f << varname << "	= " << value << endl;
		f.close();
	}
	else//need to read from the config
	{
		ifstream f;
		f.open(file.c_str());
		string s;
		while(!f.eof())
		{
			getline(f,s);
			string tok;
			stringstream(s) >> tok;
			if(tok == varname)
			{
				string tok2;
				stringstream(s) >> tok >> tok2 >> value;
#ifdef PRINT_SERIAL
				printf("%s read from file %s\n",tok.c_str(),file.c_str());
#endif
				break;
			}
#ifdef PRINT_SERIAL
			if(f.eof())
			{
				printf("Failed to read %s from file %s\n",varname.c_str(),file.c_str());
			}
#endif
		}
		f.close();
	}
#endif
}
void Serializer::ReadVariable(string file, string varname, string value)
{
    #ifdef _WIN32
	if(SearchFileGenerate(file))
	{
		ofstream f;
		f.open(file.c_str(),ios::app);
		f << varname << "	= " << value << endl;
		f.close();
	}
	else//need to read from the config
	{
		ifstream f;
		f.open(file.c_str());
		string s;
		value.clear();
		while(!f.eof())
		{
			getline(f,s);
			string tok;
			stringstream(s) >> tok;
			if(tok == varname)
			{
				string tok2;
				stringstream(s) >> tok >> tok2 >> value;
#ifdef PRINT_SERIAL
				printf("%s read from file %s\n",tok.c_str(),file.c_str());
#endif
				break;
			}
#ifdef PRINT_SERIAL
			if(f.eof())
			{
				printf("Failed to read %s from file %s\n",varname.c_str(),file.c_str());
			}
#endif
		}
		f.close();
	}
#endif
}
//================================================================================================//
						/************************************************************************
						** Search through the gen file list, see if this file needs generating **	
						*************************************************************************/
//================================================================================================//
bool Serializer::SearchFileGenerate(string file)
{
	ifstream f;
	f.open(file.c_str());
	if(!f.is_open())//if the file doesnt exit, then it really need to be generated
	{
		m_GenFiles.push_back(file);
#ifdef PRINT_SERIAL
		printf("Failed to find file %s Now generating...\n",file.c_str());
#endif
		return true;
	}
	f.close();

	//ok it does exist allredy, but we might still need to continue generation
	vector<string>::iterator i;
	for(i=m_GenFiles.begin(); i<m_GenFiles.end(); i++)
	{
		if(file == (*i))
			return true;//uh oh, need to write out
	}
	return false;//safe to read in
}