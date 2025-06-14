//windows version oc zfbasicfs

#include "zfbasicfs.h"

#ifdef WIN32

#include <windows.h>
#include "Shlwapi.h"
#include <iostream>
#include <cstdio>
#include "zfsystem.h"
#include "zfvfs.h"


bool ZSSBasicFS::ListDir(vector<string>* pkFiles, const char* acName, bool bOnlyMaps)
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
		{
			if( !(strlen(finddata.cFileName) == 1 && finddata.cFileName[0] == '.') )
				pkFiles->push_back(string(finddata.cFileName));
		}
             
		bMore = FindNextFile(hFind, &finddata);         
	}

	FindClose(hFind);

	return true;
}

bool ZSSBasicFS::IsDirectory(const char* szName)
{
	//DWORD attributes = GetFileAttributes(szName);
	//if(attributes == INVALID_FILE_ATTRIBUTES)
	//	return false;

	//return (attributes & FILE_ATTRIBUTE_DIRECTORY);



	bool direxists=false;
	DWORD atr=GetFileAttributes(szName);
	if (atr!=0xFFFFFFFF)
	{
		if (atr & FILE_ATTRIBUTE_DIRECTORY)
		{
			direxists=true;
		}
	}
	return direxists;
}

bool ZSSBasicFS::ListDirFilter(vector<string>* pkFiles, const vector<string>& pkFilters, 
							  const char* acName, bool bIgnoreMaps)
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

		if(bIsDir == TRUE || strcmp(finddata.cFileName, "..") == 0)
		{
			if(bIgnoreMaps == false)
				pkFiles->push_back(string(finddata.cFileName));
		}
		else
		{
			for(unsigned int i=0; i<pkFilters.size(); i++)
			{
				if(strstr(finddata.cFileName, (char*) pkFilters[i].c_str()) != NULL)
				{
					pkFiles->push_back(string(finddata.cFileName));
					break;
				}
			}
		}
			
		bMore = FindNextFile(hFind, &finddata);         
	}

	FindClose(hFind);

	return true;
}

bool ZSSBasicFS::CreateDir(const char* acName)
{
	if(!CreateDirectory(acName, NULL))
		return false;
	return true;
}

bool ZSSBasicFS::RemoveDir(const char* acName)
{
	if(!RemoveDirectory(acName))
		return false;
	return true;
}

static void FormatString(char* strPath)
{
	int length = strlen(strPath);
	for(int i=0; i<length; i++)
	{
		if(strPath[i] == '\\')
			strPath[i] = '/';
	}
}

char* ZSSBasicFS::GetCWD()
{
	GetCurrentDirectory(2048, m_acDir);
	FormatString(m_acDir);
	return m_acDir;
}

bool ZSSBasicFS::DirExist(const char* acName)
{
	if(acName == NULL)
		return false;

	char* szPath = new char[strlen(acName)+5];
	strcpy(szPath, acName);

	if(strnicmp("\\*",acName+strlen(acName)-2,2))
		strcat(szPath, "\\*");

	HANDLE res;
	WIN32_FIND_DATA finddata;
	res = FindFirstFile(szPath, &finddata);

	bool bExist = (res == INVALID_HANDLE_VALUE) ? false : true;

	FindClose(res);

	return bExist;
}



bool ZSSBasicFS::FileExist(const char* szFile)
{
	bool bOK = false;
	FILE* f = fopen(szFile, "r"); 
	if(f) { fclose(f); bOK = true; }
	return bOK;
}

bool ZSSBasicFS::RemoveFile(const char* acName)
{
	bool bRes = DeleteFile(acName);
	if(bRes)
		return true;
	else
	{
		DWORD kHora = GetLastError();
		int iGalla = 2;
		return false;
	}
}

#endif

