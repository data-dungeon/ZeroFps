//linux version oc zfbasicfs

#include "zfbasicfs.h"

#ifndef WIN32

#include <cstdio>

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
	
	
	closedir(kDir);
	return true;
}

bool ZFBasicFS::ListDirFilter(vector<string>* pkFiles, vector<string>& pkFilters, 
							  const char* acName, bool bIgnoreMaps)
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
		
		//if ignore maps is aktive..check for map and continue if its a map
		if(bIgnoreMaps)
		{
			string temp= acName;
			temp+="/";
			temp+=kNamn;
		
			if(DirExist(temp.c_str()))
				continue;
				//pkFiles->push_back(kNamn);			
		}
			
		for(unsigned int i=0; i<pkFilters.size(); i++)
		{
			if(strstr(kNamn.c_str(), (char*) pkFilters[i].c_str()) != NULL)
			{
				pkFiles->push_back(kNamn);
				break;
			}
		}			
	}
	
	closedir(kDir);
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

bool ZFBasicFS::RemoveFile(const char* acName)
{
	if(unlink(acName)==-1)
		return false;

	return true;
}

char* ZFBasicFS::GetCWD()
{
	getcwd(m_acDir,2048);
	return m_acDir;
}

bool ZFBasicFS::FileExist(const char* szFile)
{
	bool bOK = false;
	FILE* f = fopen(szFile, "r"); 
	if(f) { fclose(f); bOK = true; }
	return bOK;
}


#endif




