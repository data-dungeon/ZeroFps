//windows version oc zfbasicfs

#include "zfbasicfs.h"

#ifdef WIN32

#include <windows.h>
#include "Shlwapi.h"
#include <iostream>

bool ZFBasicFS::ListDir(vector<string>* pkFiles, const char* acName)
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
              
		if (!bIsDir)
		{
			pkFiles->push_back(string(finddata.cFileName));
		}
             
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

#endif

