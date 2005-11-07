//linux version oc zfbasicfs

#include "zfbasicfs.h"

#ifndef WIN32

#include <cstdio>
#include <iostream>

bool ZSSBasicFS::DirExist(const char* acName)
{
	vector<string> pkFiles;
	//pkFiles.clear();
	
	if(ListDir(&pkFiles,acName))
	{
		//pkFiles.clear();
		return true;
	}
	
	//pkFiles.clear();
	return false;
}

bool ZSSBasicFS::ListDir(vector<string>* pkFiles,const char* acName,bool bOnlyMaps)
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

bool ZSSBasicFS::ListDirFilter(vector<string>* pkFiles,const vector<string>& pkFilters, const char* acName, bool bIgnoreMaps)
{  
	DIR* kDir;
	
	kDir=opendir(acName);
	if(kDir==NULL)
		return false;
	
	string kNamn;
	dirent* kDirEnt;
	while( (kDirEnt=readdir(kDir)) != NULL)
	{
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
 			if(kNamn.find(pkFilters[i]) != -1)
			{
				pkFiles->push_back(kNamn);
				break;
			}
		}			
	}
	
	closedir(kDir);
	return true;
}



bool ZSSBasicFS::CreateDir(const char* acName)
{
	if(mkdir(acName,S_IRWXU)==-1)
		return false;
	
	return true;
}

bool ZSSBasicFS::RemoveDir(const char* acName)
{
	if(rmdir(acName)==-1)
		return false;

	return true;
}

bool ZSSBasicFS::RemoveFile(const char* acName)
{
	if(unlink(acName)==-1)
		return false;

	return true;
}

char* ZSSBasicFS::GetCWD()
{
	getcwd(m_acDir,2048);
	return m_acDir;
}

bool ZSSBasicFS::FileExist(const char* szFile)
{
	bool bOK = false;
	FILE* f = fopen(szFile, "r"); 
	if(f) { fclose(f); bOK = true; }
	return bOK;
}

bool ZSSBasicFS::IsDirectory(const char* szName)
{
	// write evil linux code here :)
}


#endif




