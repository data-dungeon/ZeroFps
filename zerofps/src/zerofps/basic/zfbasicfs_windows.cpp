//windows version oc zfbasicfs

#include "zfbasicfs.h"

#ifdef WIN32

#include <windows.h>
#include "Shlwapi.h"
#include <iostream>

bool ZFBasicFS::ListDir(vector<string>* pkFiles, const char* acName, bool bOnlyMaps)
{  
	WIN32_FIND_DATA finddata;

	char searchPath[MAX_PATH];
	strcpy(searchPath, acName);

	if(strnicmp("\\*",searchPath+strlen(searchPath)-2,2))
		strcat(searchPath, "\\*");
		
	HANDLE hFind = FindFirstFile(searchPath, &finddata);         
	BOOL bMore = (hFind != (HANDLE) -1);   
	BOOL bIsDir;
		
	while (bMore) 
	{                  
		bIsDir = (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);                 

		if(!(bOnlyMaps == true && bIsDir == false))
			pkFiles->push_back(string(finddata.cFileName));
             
		bMore = FindNextFile(hFind, &finddata);         
	}

	FindClose(hFind);

	return true;
}

bool ZFBasicFS::CreateDir(const char* acName)
{
	if(!CreateDirectory(acName, NULL))
		return false;
	return true;
}

bool ZFBasicFS::RemoveDir(const char* acName)
{
	if(!RemoveDirectory(acName))
		return false;
	return true;
}

char* ZFBasicFS::GetCWD()
{
	GetCurrentDirectory(2048, m_acDir);
	return m_acDir;
}

bool ZFBasicFS::DirExist(const char* acName)
{
	if(CreateFile(acName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,0) == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

#endif

