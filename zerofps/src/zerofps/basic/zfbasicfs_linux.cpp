//linux version oc zfbasicfs

#include "zfbasicfs.h"

#ifndef WIN32

bool ZFBasicFS::ListDirectory(vector<string>* pkFiles,const char* acName)
{

	DIR* kDir;
	
	kDir=opendir(acName);
	if(kDir==NULL)
		return false;
	
	
	dirent* kDirEnt;
	while( (kDirEnt=readdir(kDir)) != NULL)
	{
		string kNamn;
		kNamn=kDirEnt->d_name;
		pkFiles->push_back(kNamn);
	}
	
	return true;
}

bool ZFBasicFS::CreateDirectory(const char* acName)
{
	if(mkdir(acName,S_IRWXU)==-1)
		return false;
	
	return true;
}

bool ZFBasicFS::RemoveDirectory(const char* acName)
{
	if(rmdir(acName)==-1)
		return false;

	return true;
}

#endif




