//linux version oc zfbasicfs

#include "zfbasicfs.h"

#ifndef WIN32

bool ZFBasicFS::DirExist(const char* acName)
{
	vector<string> pkFiles;
	pkFiles.clear();
	
	if(ListDir(&pkFiles,acName))
	{
		pkFiles.clear();
		return true;
	}
	
	pkFiles.clear();
	return false;
}

bool ZFBasicFS::ListDir(vector<string>* pkFiles,const char* acName,bool bOnlyMaps)
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
		
		
		if(bOnlyMaps)
		{
			string temp= acName;
			temp+="/";
			temp+=kNamn;
		
			if(DirExist(temp.c_str()))
				pkFiles->push_back(kNamn);			
		}
		else		
			pkFiles->push_back(kNamn);
	}
	
	return true;
}

bool ZFBasicFS::CreateDir(const char* acName)
{
	if(mkdir(acName,S_IRWXU)==-1)
		return false;
	
	return true;
}

bool ZFBasicFS::RemoveDir(const char* acName)
{
	if(rmdir(acName)==-1)
		return false;

	return true;
}

char* ZFBasicFS::GetCWD()
{
	getcwd(m_acDir,2048);
	return m_acDir;
}

#endif




