#include <iostream.h>
#include <stdio.h>
#include "zfobject.h"
#include "zfobjectmanger.h"

#include "zfvfs.h"

using namespace std;

extern ZFObjectManger g_ZFObjSys;

ZFVFile::ZFVFile()
{
	m_pkFileSystem = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	m_pkFilePointer = NULL;
	
}

ZFVFile::~ZFVFile()
{
	Close();
}

bool ZFVFile::Open(string strFileName, int iOptions, bool bWrite)
{
	m_pkFilePointer = m_pkFileSystem->Open(strFileName, iOptions,bWrite);
	if(m_pkFilePointer)
		return true;

	return false;
}

bool ZFVFile::Close()
{
	if(m_pkFilePointer)
		fclose(m_pkFilePointer);

	m_pkFilePointer = NULL;
	return true;
}

bool ZFVFile::Read  (void* pkData, int iSize, int iCount)
{
	return fread(pkData, iSize, iCount, m_pkFilePointer);
}

bool ZFVFile::Write (void* pkData, int iSize, int iCount)
{
	return fwrite(pkData, iSize, iCount, m_pkFilePointer);
}

void ZFVFile::Seek(int iPos, int iOrigin)
{
	fseek(m_pkFilePointer,iPos, iOrigin);
}

int ZFVFile::Tell()
{
	return ftell(m_pkFilePointer);
}

int ZFVFile::GetSize()
{
	return 0;
}











ZFVFileSystem::ZFVFileSystem()
: ZFObject("ZFVFileSystem")
{
	m_pkBasicFS = static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"));		

}

ZFVFileSystem::~ZFVFileSystem()
{

}

FILE* ZFVFileSystem::Open(string strFileName, int iOptions, bool bWrite)
{
	FILE*	pkFp;
	string	strRootMerge;
	char*	szOptions;
	if(bWrite)
		szOptions = "wb";
	else
		szOptions = "rb";
	
	cout << "=) =) =)";

	// Try to open file directly.
//	pkFp = fopen(strFileName.c_str(), szOptions);
//	if(pkFp)
//		return pkFp;

	// Try to open from all active RootPaths.
	for(int i=0; i <m_kstrRootPath.size(); i++) {
		strRootMerge = m_kstrRootPath[i] + strFileName;
		pkFp = fopen(strRootMerge.c_str(), szOptions);
		if(pkFp) {
			cout << "Path: " << strRootMerge.c_str();
			cout << "\n";
			return pkFp;
			
			}
		}

	// Failed to open file.
	return NULL;
}

void ZFVFileSystem::AddRootPath(string strRootPath)
{
	cout << "Adding '" << strRootPath.c_str();
	cout << "' to VFS root table.\n";
	m_kstrRootPath.push_back(strRootPath);
}

string	ZFVFileSystem::GetCurrentWorkingDir()
{
	string strWorkDir = m_pkBasicFS->GetCWD();
	return strWorkDir;
}

bool ZFVFileSystem::CreateDir(string strDir)
{
	return m_pkBasicFS->CreateDir(strDir.c_str());
}

bool ZFVFileSystem::RemoveDir(string strDir)
{
	return m_pkBasicFS->RemoveDir(strDir.c_str());
}

bool ZFVFileSystem::ListDir(vector<string>* pkFiles, string strName, bool bOnlyMaps)
{
	return m_pkBasicFS->ListDir(pkFiles, strName.c_str(), bOnlyMaps);
}

bool ZFVFileSystem::ListDirFilter(vector<string>* pkFiles, vector<string>& pkFilters, 
	string strName, bool bIgnoreMaps)
{
	return m_pkBasicFS->ListDirFilter(pkFiles, pkFilters, strName.c_str(), bIgnoreMaps);

}

bool ZFVFileSystem::DirExist(string strName)
{
	return m_pkBasicFS->DirExist(strName.c_str());
}

