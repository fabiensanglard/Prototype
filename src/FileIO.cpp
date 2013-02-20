#include "FileIO.h"
#include <string.h>


bool CFileIO::FileOpen(){
		if(m_szFileName==NULL)
			return false;
		
		if(!(fPTR=fopen(m_szFileName,m_szReadMode)))
			return false;

		return true;
	}



bool CFileIO::AppendFile(){
	if(m_szFileName==NULL)
		return false;
	if(!(fPTR=fopen(m_szFileName,"a")))
		return false;

	return true;
}

bool CFileIO::AppendFileBinary(){
	if(m_szFileName==NULL)
		return false;
	if(!(fPTR=fopen(m_szFileName,"ab")))
		return false;

	return true;
}


void CFileIO::ShutFile(){
		//get new file OFFSET
		fseek(fPTR,0,SEEK_CUR);
		m_lFileOFFSET = ftell(fPTR);
		//close file and return line
		fclose(fPTR);
}


bool CFileIO::GetFile(const char* filename,const char* ReadMode ){

	strcpy(m_szFileName,filename);
	strcpy(m_szReadMode,ReadMode);
		
	if(!FileOpen())
		return false;
	
	fseek(fPTR,0,SEEK_SET);
	m_lFileOFFSET=ftell(fPTR);
	fclose(fPTR);

	return true;
}




char* CFileIO::ReadLine()
	{
		if(!FileOpen())
			return NULL;//cant open file

		//make sure its not the EOF
		fseek(fPTR,0,SEEK_END);
		if(m_lFileOFFSET==ftell(fPTR))
			return NULL;//end of file

		//move to file offset and get line
		fseek(fPTR,m_lFileOFFSET,SEEK_SET);//move to file offset
		fgets(m_szLine,256,fPTR);//read line from file

		//get new file OFFSET
		fseek(fPTR,0,SEEK_CUR);
		m_lFileOFFSET = ftell(fPTR);

		//close file and return line
		fclose(fPTR);
		return m_szLine;
	}

	char* CFileIO::FindStringInFile(char* string)//returns remaining line
	{
		if(!FileOpen())
			return NULL;//cant open file

		fseek(fPTR,0,SEEK_END);
		long end = ftell(fPTR);
		m_lFileOFFSET=0;
		fseek(fPTR,m_lFileOFFSET,SEEK_SET);//beginning of file
		char* line;
		while(m_lFileOFFSET!=end)
		{
			line = ReadLine();
			if(!line)
				continue;
			char* tok = strtok(line," ");
			if(!strcmp(tok,string))
			{
				m_lFileOFFSET=0;
				tok = strtok(NULL,"\n");
				return tok;
			}
		}
		m_lFileOFFSET=0;
		fclose(fPTR);
		return NULL;
	}







	bool CFileIO::WriteLine(char* txt)
	{
		if(!AppendFile())
			return false;//cant open file

		fwrite(txt,sizeof(char),strlen(txt),fPTR);//write the line
		//get new file OFFSET
		fseek(fPTR,0,SEEK_CUR);
		m_lFileOFFSET = ftell(fPTR);
		//close file and return line
		fclose(fPTR);

		return true;
	}




	void CFileIO::ReleaseFile()	{}