#ifndef _FILEIO_H_
#define _FILEIO_H_
//////FILEIO WRAPPER///////
#include <stdio.h>

//char* ScanString(char* line, char* string);
class CFileIO
{
private:
	FILE* fPTR;//file pointer
	char m_szFileName[256];//remember the file for re-opening
	char m_szReadMode[12];//read/write mode
	long m_lFileOFFSET;//stored file offset from last read
	char m_szLine[256];//last read line
	
	bool FileOpen();
	bool AppendFile();
	bool AppendFileBinary();

public:
	CFileIO()
	{
		fPTR = NULL;
	}
	~CFileIO()
	{}
	void ShutFile();

	//open a file for reading or writing
	bool GetFile(const char* filename,const char* ReadMode );

	char* ReadLine();

	char* FindStringInFile(char* string);

	bool WriteLine(char* txt);

	template<class T>
	bool WriteBinary(T* data)
		{
		if(!AppendFileBinary())
			return false;//cant open file

		fwrite(data,sizeof(T),1,fPTR);//write the line
		//get new file OFFSET
		fseek(fPTR,0,SEEK_CUR);
		m_lFileOFFSET = ftell(fPTR);
		//close file and return line
		fclose(fPTR);

		return true;
	}

	template<class T>
	bool WriteBinary(T* data, int size)
		
	{
		if(!AppendFileBinary())
			return false;//cant open file

		fwrite(data,sizeof(T),size,fPTR);//write the line
		//get new file OFFSET
		fseek(fPTR,0,SEEK_CUR);
		m_lFileOFFSET = ftell(fPTR);
		//close file and return line
		fclose(fPTR);

		return true;
	}

	template<class T>
	bool ReadBinary(T* data)
		{
		if(!FileOpen())
			return NULL;//cant open file

		fseek(fPTR,m_lFileOFFSET,SEEK_SET);//move to file offset
		fread(data,sizeof(T),1,fPTR);//write the line
		//get new file OFFSET
		fseek(fPTR,0,SEEK_CUR);
		m_lFileOFFSET = ftell(fPTR);
		//close file and return line
		fclose(fPTR);

		return true;
	}
	
	template<class T>
	bool ReadBinary(T* data, int size)
		{
		if(!FileOpen())
			return NULL;//cant open file

		fseek(fPTR,m_lFileOFFSET,SEEK_SET);//move to file offset
		fread(data,sizeof(T),size,fPTR);//write the line
		//get new file OFFSET
		fseek(fPTR,0,SEEK_CUR);
		m_lFileOFFSET = ftell(fPTR);
		//close file and return line
		fclose(fPTR);

		return true;
	}

	void ReleaseFile();
	
};


#endif
