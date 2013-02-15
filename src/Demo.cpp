#include "Demo.h"
#include "FileIO.h"
void SaveDemo(demoSystem dem)
{
	CFileIO fIO;
	fIO.GetFile("Data/Demo1.dem","wb");
	fIO.WriteBinary(&dem.iScrollOffset,1);
	int bufsize = (int)dem.mKeyBuffer.size();
	fIO.WriteBinary(&bufsize,1);
	if(bufsize>0)
	{
		list<keyBuffer>::iterator i;
		for(i = dem.mKeyBuffer.begin(); i != dem.mKeyBuffer.end(); i++)
		{
			fIO.WriteBinary(&(*i).tick,1);
			fIO.WriteBinary(&(*i).KeyStateChange,1);
			fIO.WriteBinary(&(*i).KeyStates,1);
		}
	}
}
bool LoadDemo(demoSystem& dem, int demo)
{
	char name[256];
	sprintf(name,"Data/Demo%i.dem",demo);
	dem.mKeyBuffer.clear();
	CFileIO fIO;
	if(!fIO.GetFile(name,"rb"))
		return false;
	fIO.ReadBinary(&dem.iScrollOffset,1);

	int bufsize,n;
	fIO.ReadBinary(&bufsize,1);
	if(bufsize>0)
	{
		for(n=0;n<bufsize;n++)
		{
			keyBuffer kb;
			fIO.ReadBinary(&kb.tick,1);
			fIO.ReadBinary(&kb.KeyStateChange,1);
			fIO.ReadBinary(&kb.KeyStates,1);
			dem.mKeyBuffer.push_back(kb);
		}
	}
	return true;
}